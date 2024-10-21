//--------------------------------------------------
// Модуль управления дискретными сигналами
//--------------------------------------------------
#ifndef GPIO_IF_H
#define GPIO_IF_H

//--------------------------------------------------
#include "common.h"

// Неуправляемые снаружи модуля сигналы
//--------------------------------------------------
/*{{{*/
#define FRAME_START  (GPIOA->BSRR = GPIO_BSRR_BR_15)
#define FRAME_STOP   (GPIOA->BSRR = GPIO_BSRR_BS_15)

#define ENABLE_BOOT  (GPIOD->BSRR = GPIO_BSRR_BS_7)
#define DISABLE_BOOT (GPIOD->BSRR = GPIO_BSRR_BR_7)

#define FRAME_IN_PIN	GPIO_PIN_12
#define FRAME_IN_STATE	HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12)

#define IS_PUSHED     (HAL_GPIO_ReadPin(Encoder_S_GPIO_Port, Encoder_S_Pin) == GPIO_PIN_RESET)
#define IS_NOT_PUSHED (HAL_GPIO_ReadPin(Encoder_S_GPIO_Port, Encoder_S_Pin) == GPIO_PIN_SET)

#define SSD_RESET   (GPIOB->BSRR = GPIO_BSRR_BR_11)
#define SSD_UNRESET (GPIOB->BSRR = GPIO_BSRR_BS_11)

#define SSD_COMMAND (GPIOB->BSRR = GPIO_BSRR_BR_13)
#define SSD_DATA    (GPIOB->BSRR = GPIO_BSRR_BS_13)
/*}}}*/
// Канальные сигналы
//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

extern uint8_t led_set(const led_id_t led, const state_t state);
extern uint8_t led_tgl(const led_id_t led);

extern uint8_t input_read(const input_id_t input, const channels_t ch, state_t *state);

extern uint8_t id_read(void);

#ifdef __cplusplus
}
#endif

//--------------------------------------------------
#endif
