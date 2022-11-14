#include "bc_scanner_app_i.h"
#include <furi.h>
#include <furi_hal.h>
#include <storage/storage.h>
#include <lib/toolbox/path.h>
#include <gui/gui.h>

int32_t bc_scanner_app(void* p) {
    UNUSED(p);
    ViewPort* view_port = view_port_alloc();
    Gui* gui = furi_record_open("gui");
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    furi_delay_ms(2000);

    gui_remove_view_port(gui, view_port);
    furi_record_close("gui");
    view_port_free(view_port);
    return 0;
}
