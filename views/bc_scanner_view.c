#include "bc_scanner_view.h"
#include "../bc_scanner_script.h"
#include <gui/elements.h>
#include <assets_icons.h>

#define MAX_NAME_LEN 64

struct BarCodeView {
    View* view;
    BarCodeOkCallback callback;
    void* context;
};

typedef struct {
    char file_name[MAX_NAME_LEN];
    BarCodeState state;
    uint8_t anim_frame;
} BarCodeModel;


/*
static void bc_scanner_draw_callback(Canvas* canvas, void* _model) {
    BarCodeModel* model = _model;

    FuriString* disp_str;
    disp_str = furi_string_alloc_set(model->file_name);
    elements_string_fit_width(canvas, disp_str, 128 - 2);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 8, furi_string_get_cstr(disp_str));
    furi_string_reset(disp_str);

    canvas_draw_icon(canvas, 22, 20, &I_UsbTree_48x22);

    if((model->state.state == BadUsbStateIdle) || (model->state.state == BadUsbStateDone)) {
        elements_button_center(canvas, "Run");
    } else if((model->state.state == BadUsbStateRunning) || (model->state.state == BadUsbStateDelay)) {
        elements_button_center(canvas, "Stop");
    }

    if(model->state.state == BadUsbStateNotConnected) {
        canvas_draw_icon(canvas, 4, 22, &I_Clock_18x18);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 127, 27, AlignRight, AlignBottom, "Connect");
        canvas_draw_str_aligned(canvas, 127, 39, AlignRight, AlignBottom, "to USB");
    } else if(model->state.state == BadUsbStateFileError) {
        canvas_draw_icon(canvas, 4, 22, &I_Error_18x18);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 127, 27, AlignRight, AlignBottom, "File");
        canvas_draw_str_aligned(canvas, 127, 39, AlignRight, AlignBottom, "ERROR");
    } else if(model->state.state == BadUsbStateScriptError) {
        canvas_draw_icon(canvas, 4, 22, &I_Error_18x18);
        canvas_set_font(canvas, FontPrimary);
        canvas_draw_str_aligned(canvas, 127, 33, AlignRight, AlignBottom, "ERROR:");
        canvas_set_font(canvas, FontSecondary);
        furi_string_printf(disp_str, "line %u", model->state.error_line);
        canvas_draw_str_aligned(
            canvas, 127, 46, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        furi_string_reset(disp_str);
        canvas_draw_str_aligned(canvas, 127, 56, AlignRight, AlignBottom, model->state.error);
    } else if(model->state.state == BadUsbStateIdle) {
        canvas_draw_icon(canvas, 4, 22, &I_Smile_18x18);
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str_aligned(canvas, 114, 36, AlignRight, AlignBottom, "0");
        canvas_draw_icon(canvas, 117, 22, &I_Percent_10x14);
    } else if(model->state.state == BadUsbStateRunning) {
        if(model->anim_frame == 0) {
            canvas_draw_icon(canvas, 4, 19, &I_EviSmile1_18x21);
        } else {
            canvas_draw_icon(canvas, 4, 19, &I_EviSmile2_18x21);
        }
        canvas_set_font(canvas, FontBigNumbers);
        furi_string_printf(
            disp_str, "%u", ((model->state.line_cur - 1) * 100) / model->state.line_nb);
        canvas_draw_str_aligned(
            canvas, 114, 36, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        furi_string_reset(disp_str);
        canvas_draw_icon(canvas, 117, 22, &I_Percent_10x14);
    } else if(model->state.state == BadUsbStateDone) {
        canvas_draw_icon(canvas, 4, 19, &I_EviSmile1_18x21);
        canvas_set_font(canvas, FontBigNumbers);
        canvas_draw_str_aligned(canvas, 114, 36, AlignRight, AlignBottom, "100");
        furi_string_reset(disp_str);
        canvas_draw_icon(canvas, 117, 22, &I_Percent_10x14);
    } else if(model->state.state == BadUsbStateDelay) {
        if(model->anim_frame == 0) {
            canvas_draw_icon(canvas, 4, 19, &I_EviWaiting1_18x21);
        } else {
            canvas_draw_icon(canvas, 4, 19, &I_EviWaiting2_18x21);
        }
        canvas_set_font(canvas, FontBigNumbers);
        furi_string_printf(
            disp_str, "%u", ((model->state.line_cur - 1) * 100) / model->state.line_nb);
        canvas_draw_str_aligned(
            canvas, 114, 36, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        furi_string_reset(disp_str);
        canvas_draw_icon(canvas, 117, 22, &I_Percent_10x14);
        canvas_set_font(canvas, FontSecondary);
        furi_string_printf(disp_str, "delay %lus", model->state.delay_remain);
        canvas_draw_str_aligned(
            canvas, 127, 46, AlignRight, AlignBottom, furi_string_get_cstr(disp_str));
        furi_string_reset(disp_str);
    } else {
        canvas_draw_icon(canvas, 4, 22, &I_Clock_18x18);
    }

    furi_string_free(disp_str);
}
*/

static bool bc_scanner_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    BarCodeView* bar_code = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyOk) {
            consumed = true;
            furi_assert(bar_code->callback);
            bar_code->callback(InputTypeShort, bar_code->context);
        }
    }

    return consumed;
}

BarCodeView* bc_scanner_alloc() {
    BarCodeView* bar_code = malloc(sizeof(BarCodeView));

    bar_code->view = view_alloc();
    view_allocate_model(bar_code->view, ViewModelTypeLocking, sizeof(BarCodeModel));
    view_set_context(bar_code->view, bar_code);
    //view_set_draw_callback(bar_code->view, bc_scanner_draw_callback);
    view_set_input_callback(bar_code->view, bc_scanner_input_callback);

    return bar_code;
}

void bc_scanner_free(BarCodeView* bar_code) {
    furi_assert(bar_code);
    view_free(bar_code->view);
    free(bar_code);
}

View* bc_scanner_get_view(BarCodeView* bar_code) {
    furi_assert(bar_code);
    return bar_code->view;
}

void bc_scanner_set_ok_callback(BarCodeView* bar_code, BarCodeOkCallback callback, void* context) {
    furi_assert(bar_code);
    furi_assert(callback);
    with_view_model(
        bar_code->view,
        BarCodeModel * model,
        {
            UNUSED(model);
            bar_code->callback = callback;
            bar_code->context = context;
        },
        true);
}

void bc_scanner_set_file_name(BarCodeView* bar_code, const char* name) {
    furi_assert(name);
    with_view_model(
        bar_code->view,
        BarCodeModel * model,
        { strlcpy(model->file_name, name, MAX_NAME_LEN); },
        true);
}

void bc_scanner_set_state(BarCodeView* bar_code, BarCodeState* st) {
    furi_assert(st);
    with_view_model(
        bar_code->view,
        BarCodeModel * model,
        {
            memcpy(&(model->state), st, sizeof(BarCodeState));
            model->anim_frame ^= 1;
        },
        true);
}
