//--------------------------------------------------
// Интерфейс к АЦП
//--------------------------------------------------
#ifndef ADC_IF_H
#define ADC_IF_H

//--------------------------------------------------
#include "stm32f4xx_hal.h"
#include "adc.h"

#include "common.h"

//--------------------------------------------------
typedef enum _adc_error {/*{{{*/
    ADC_OK,
    ADC_CHAN,
    ADC_NO_DATA
} adc_error_t;
/*}}}*/
//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

extern enum _adc_error adc_get_value(enum _channels channel, enum _adc_input in, uint16_t *val);

#ifdef __cplusplus
}
#endif

//--------------------------------------------------
#endif
