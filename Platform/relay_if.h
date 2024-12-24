//--------------------------------------------------
// Модуль управления дискретными выходами
//--------------------------------------------------
#ifndef RELAY_IF_H
#define RELAY_IF_H
#include "common.h"
//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

extern void relay_init(void);
extern uint8_t relay_set(const relay_id_t relay, const channels_t ch, const state_t state);
#ifdef __cplusplus
}
#endif

//--------------------------------------------------
#endif
