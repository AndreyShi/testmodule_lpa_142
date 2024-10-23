//--------------------------------------------------
// Интерфейс управления ЦАП-ами
//--------------------------------------------------
#ifndef DAC_IF_H
#define DAC_IF_H

//--------------------------------------------------
#include "common.h"

//--------------------------------------------------
#define DAC_MAX (1<<12)-1

//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

extern void dac_start(void);
extern uint8_t dac_set(enum _channels channel, uint16_t value);
extern uint8_t dac_set_i(enum _channels channel, float value);

#ifdef __cplusplus
}
#endif

//--------------------------------------------------
#endif
