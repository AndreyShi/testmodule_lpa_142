//--------------------------------------------------
// Модуль управления диспелеем на чипе ssd1306
//--------------------------------------------------
#ifndef SSD_1306_H
#define SSD_1306_H

//--------------------------------------------------
#include <stdint.h>
#include <stdbool.h>
//--------------------------------------------------
extern uint8_t ssd_frame_buff[128*8];
//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

extern void ssd1306_init(void);
extern void ssd1306_task(void);

extern bool ssd1306_render_now(void);

extern void ssd_spi_isr(void);

#ifdef __cplusplus
};
#endif

//--------------------------------------------------
#endif
