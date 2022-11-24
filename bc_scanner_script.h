#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>

typedef struct BarCodeScript BarCodeScript;

typedef enum {
    BarCodeStateInit,
    BarCodeStateNotConnected,
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

#ifdef __cplusplus
}
#endif
