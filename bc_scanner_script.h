#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>

typedef struct BarCodeScript BarCodeScript;

typedef enum {
    BarCodeStateInit,
    BarCodeStateIdle,
    BarCodeStateRunning,
    BarCodeStateDelay,
    BarCodeStateDone,
    BarCodeStateScriptError,
    BarCodeStateFileError,
} BarCodeWorkerState;

typedef struct {
    BarCodeWorkerState state;
    uint16_t line_cur;
    uint16_t line_nb;
    uint32_t delay_remain;
    uint16_t error_line;
    char error[64];
} BarCodeState;

BarCodeScript* bc_scanner_script_open(FuriString* file_path);
void bc_scanner_script_close(BarCodeScript* bc_script);

void bc_scanner_script_toggle(BarCodeScript* bc_script);

BarCodeState* bc_scanner_script_get_state(BarCodeScript* bc_script);



#ifdef __cplusplus
}
#endif
