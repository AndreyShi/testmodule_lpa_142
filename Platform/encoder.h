//--------------------------------------------------
// Модуль управления энкодером
//--------------------------------------------------
#ifndef ENCODER_H
#define ENCODER_H

//--------------------------------------------------
#include <stdint.h>

//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

extern void encoder_init(void);
extern void encoder_reset_position(uint16_t pos);
extern uint16_t encoder_position(void);

extern void enc_tim_isr(void);
extern void enc_callback(uint8_t dir, uint16_t value);

#ifdef __cplusplus
};
#endif

//--------------------------------------------------
#endif
