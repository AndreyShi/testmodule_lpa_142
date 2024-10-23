//--------------------------------------------------
// Калибровочные значения для стенда
//--------------------------------------------------
#ifndef CALIBRATION_H
#define CALIBRATION_H

//--------------------------------------------------
#include "common.h"

//--------------------------------------------------
// channel, K*x + B
extern float calibrations_dac[2][2];

//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

extern float    calibrate_u0 (uint16_t x, channels_t ch);
extern float    calibrate_i0 (uint16_t x, channels_t ch);
extern uint16_t calibrate_dac(float i,    channels_t ch);

#ifdef __cplusplus
}
#endif
//--------------------------------------------------
#endif
