#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <lib/toolbox/args.h>
#include <furi_hal_usb_cdc.h>
#include <storage/storage.h>
#include "bc_scanner_script.h"
#include <dolphin/dolphin.h>
#include "cli/cli_vcp.h"
#include "cli/cli.h"

#define TAG "BarCodeScanner"
#define WORKER_TAG TAG "Worker"
#define FILE_BUFFER_LEN 1

#define SCRIPT_STATE_ERROR (-1)
#define SCRIPT_STATE_END (-2)
#define SCRIPT_STATE_NEXT_LINE (-3)

#define USB_CDC_PKT_LEN CDC_DATA_SZ
#define USB_UART_RX_BUF_SIZE (USB_CDC_PKT_LEN * 5)

#define USB_CDC_BIT_DTR (1 << 0)
#define USB_CDC_BIT_RTS (1 << 1)

typedef enum {
    WorkerEvtToggle = (1 << 0),
    WorkerEvtEnd = (1 << 1),
    WorkerEvtConnect = (1 << 2),
    WorkerEvtDisconnect = (1 << 3),
} WorkerEvtFlags;

struct BarCodeScript {
    BarCodeState st;
    FuriString* file_path;
    uint32_t defdelay;
    FuriThread* thread;
    uint8_t file_buf[FILE_BUFFER_LEN];
    uint8_t buf_start;
    uint8_t buf_len;
    bool file_end;
    FuriString* line;

    UartConfig cfg;
    UartConfig cfg_new;

    FuriString* line_prev;
    uint32_t repeat_cnt;
};


static void usb_uart_serial_init() {
    furi_hal_usb_unlock();
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_session_close(cli);
    furi_record_close(RECORD_CLI);
    furi_check(furi_hal_usb_set_config(&usb_cdc_single, NULL) == true);
    furi_hal_console_disable();
    furi_hal_uart_set_br(FuriHalUartIdUSART1, 19200);
}

static void usb_uart_serial_deinit() {
    furi_hal_usb_unlock();
    furi_check(furi_hal_usb_set_config(&usb_cdc_single, NULL) == true);
    Cli* cli = furi_record_open(RECORD_CLI);
    cli_session_open(cli, &cli_vcp);
    furi_record_close(RECORD_CLI);
    furi_hal_console_enable();
}

static bool bc_script_preload(BarCodeScript* bc_script, File* script_file){
    bc_script->st.line_nb = 1;
    UNUSED(script_file);
    return true;
}

static bool is_bc_end(const char chr) {
    return ((chr == '\0') || (chr == '\r') || (chr == '\n'));//TODO SPACE NEED???
}

static int32_t bc_scanner_worker(void* context){
    BarCodeScript* bc_script = context;

    BarCodeWorkerState worker_state = BarCodeStateInit;
    int32_t delay_val = 0;
    UNUSED(delay_val);
    File* script_file = storage_file_alloc(furi_record_open(RECORD_STORAGE));

    FURI_LOG_I(WORKER_TAG, "Init");
    //uint8_t buff[5] = {'p', 'r', 'i', 'v', 'k'};
    //uint8_t state = 99;

    usb_uart_serial_init();
    
    while (1)
    {
            //state = furi_hal_cdc_get_ctrl_line_state(FuriHalUartIdUSART1);
            //FURI_LOG_I(WORKER_TAG, "STATTE: %d", state);
        if (worker_state == BarCodeStateInit)
        {
           if(storage_file_open(
                   script_file,
                   furi_string_get_cstr(bc_script->file_path),
                   FSAM_READ,
                   FSOM_OPEN_EXISTING)) {
                if((bc_script_preload(bc_script, script_file)) && (bc_script->st.line_nb > 0)) {
                    if(1) { //TODO Check USB Connect
                        worker_state = BarCodeStateIdle; // Ready to run
                    } else {
                        //worker_state = BadUsbStateNotConnected; // USB not connected
                    }
                } else {
                    worker_state = BarCodeStateScriptError; // Script preload error
                }
            } else {
                FURI_LOG_E(WORKER_TAG, "File open error");
                worker_state = BarCodeStateFileError; // File open error
            }
            bc_script->st.state = worker_state;
        }else if(worker_state == BarCodeStateIdle) { // State: ready to start
            uint32_t flags = furi_thread_flags_wait(
                WorkerEvtEnd | WorkerEvtConnect | WorkerEvtToggle,
                FuriFlagWaitAny,
                FuriWaitForever);
            furi_check((flags & FuriFlagError) == 0);
            if(flags & WorkerEvtEnd) {
                break;
            } else if(flags & WorkerEvtConnect) {
                worker_state = BarCodeStateIdle; // Ready to run
            } else if(flags & WorkerEvtToggle) {
                FURI_LOG_I(WORKER_TAG, "SendUART_MSG");
                uint16_t ret = 0;
                 do {
                    ret = storage_file_read(script_file, bc_script->file_buf, FILE_BUFFER_LEN);
                    if (is_bc_end((char)bc_script->file_buf[0]))
                    {
                        bc_script->st.line_nb++;
                        break;
                    }
                    
                    furi_hal_cdc_send(0, bc_script->file_buf, FILE_BUFFER_LEN);
                    furi_delay_ms(10);
                    if(storage_file_eof(script_file)) {
                        bc_script->st.line_nb++;
                        break;
                    }
                 }while (ret > 0);
                 
                worker_state = BarCodeStateIdle;
                bc_script->st.state = BarCodeStateDone;
                storage_file_seek(script_file, 0, true);
                continue;
            }
            bc_script->st.state = worker_state;
        }
        
    }

    usb_uart_serial_deinit();
    storage_file_close(script_file);
    storage_file_free(script_file);
    FURI_LOG_I(WORKER_TAG, "End");
    

    return 0;
}

BarCodeScript* bc_scanner_script_open(FuriString* file_path) {
    FURI_LOG_D(WORKER_TAG, "bc_scanner_script_open");
    furi_assert(file_path);

    BarCodeScript* bc_script = malloc(sizeof(BarCodeScript));
    bc_script->file_path = furi_string_alloc();
    furi_string_set(bc_script->file_path, file_path);

    bc_script->st.state = BarCodeStateInit;
    bc_script->st.error[0] = '\0';

    bc_script->thread = furi_thread_alloc_ex("BcScannerWorker", 2048, bc_scanner_worker, bc_script);
    furi_thread_start(bc_script->thread);
    return bc_script;
} //-V773

void bc_scanner_script_close(BarCodeScript* bc_script) {
    furi_assert(bc_script);
    furi_thread_flags_set(furi_thread_get_id(bc_script->thread), WorkerEvtEnd);
    furi_thread_join(bc_script->thread);
    furi_thread_free(bc_script->thread);
    furi_string_free(bc_script->file_path);
    free(bc_script);
    FURI_LOG_D(WORKER_TAG, "bc_scanner_script_close");
}

void bc_scanner_script_toggle(BarCodeScript* bc_script) {
    furi_assert(bc_script);
    furi_thread_flags_set(furi_thread_get_id(bc_script->thread), WorkerEvtToggle);
}

BarCodeState* bc_scanner_script_get_state(BarCodeScript* bc_script) {
    furi_assert(bc_script);
    return &(bc_script->st);
}


