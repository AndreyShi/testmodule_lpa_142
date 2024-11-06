//--------------------------------------------------
// Интерфейс к Измерителю задержек прохождения
// сигнала через барьер
//--------------------------------------------------
#ifndef TIM_IF_H
#define TIM_IF_H

//--------------------------------------------------
#include "common.h"

//--------------------------------------------------
typedef enum _tim_error {/*{{{*/
    TIM_OK,
    TIM_CHAN,
    TIM_BUSY,
    TIM_NO_DATA
} tim_error_t;
/*}}}*/
//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

extern enum _tim_error tim_get_delay(enum _channels channel, uint16_t *val);

extern void delay_tim_isr(TIM_HandleTypeDef *htim);

#ifdef __cplusplus
}
#endif

//--------------------------------------------------
#endif
