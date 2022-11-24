#include "../bc_scanner_script.h"
#include "../bc_scanner_app_i.h"
#include "../views/bc_scanner_view.h"
#include "furi_hal.h"
#include "toolbox/path.h"

void bc_scanner_scene_work_ok_callback(InputType type, void* context) {
    furi_assert(context);
    BarCodeApp* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, type);
}

bool bc_scanner_scene_work_on_event(void* context, SceneManagerEvent event) {
    BarCodeApp* app = context;
    UNUSED(app);
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        //bad_usb_script_toggle(app->bad_usb_script);
        consumed = true;
    } else if(event.type == SceneManagerEventTypeTick) {
        //bc_scanner_set_state(app->bad_usb_view, bc_scanner_script_get_state(app->bad_usb_script));
    }
    return consumed;
}

void bc_scanner_scene_work_on_enter(void* context) {
    BarCodeApp* app = context;

    FuriString* file_name;
    file_name = furi_string_alloc();

    path_extract_filename(app->file_path, file_name, true);
    bc_scanner_set_file_name(app->bc_scanner_view, furi_string_get_cstr(file_name));
    //app->bad_usb_script = bad_usb_script_open(app->file_path);

    furi_string_free(file_name);

    //bad_usb_set_state(app->bad_usb_view, bad_usb_script_get_state(app->bad_usb_script));

    //bad_usb_set_ok_callback(app->bad_usb_view, bad_usb_scene_work_ok_callback, app);
    //view_dispatcher_switch_to_view(app->view_dispatcher, BadUsbAppViewWork);
}

void bc_scanner_scene_work_on_exit(void* context) {
    BarCodeApp* app = context;
    UNUSED(app);
    //bad_usb_script_close(app->bad_usb_script);
}
