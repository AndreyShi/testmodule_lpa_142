//--------------------------------------------------
// Автомат подавления дребезга кнопки
//--------------------------------------------------
#ifndef BUTTON_IF_H
#define BUTTON_IF_H

//--------------------------------------------------
#include <stdint.h>

#include "common.h"

//--------------------------------------------------
typedef struct {
    uint8_t pushed:  1;
    uint8_t clicked: 1;
    uint8_t hold_1:  1;
    uint8_t released:1;
} btn_flags;

enum _button_state {/*{{{*/
    BTN_RELEASED,
    BTN_PUSH_DEBOUNCE,
    BTN_HOLD_1,
    BTN_PUSHED,
    BTN_RELEASE_DEBOUNCE,
};
extern enum _button_state state;
extern int btn_context;
//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

extern void button_init(void);
extern void button_task(void);
extern btn_flags button_state(button_id_t btn);

#ifdef __cplusplus
}
#endif
//--------------------------------------------------
#endif
