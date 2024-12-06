//--------------------------------------------------
// Интерфейс к UART связи с барьером,
// используется для обновления прошивки и
// для управления барьером во время тестирования
//--------------------------------------------------
#ifndef BOOT_UART_H
#define BOOT_UART_H

//--------------------------------------------------
#include <stdbool.h>
#include <stdint.h>

//--------------------------------------------------
extern bool skip_update;

//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

void boot_init(void);

bool boot_update(void);
uint8_t boot_update_nb(void);

bool set_lpa_mode(uint8_t mode);

void boot_uart_rx_isr(void);

#ifdef __cplusplus
}
#endif

//--------------------------------------------------
#endif
