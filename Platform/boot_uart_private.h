//--------------------------------------------------
// Интерфейс к UART связи с барьером,
// используется для обновления прошивки и
// для управления барьером во время тестирования
//--------------------------------------------------
#ifndef BOOT_UART_PRIVATE_H
#define BOOT_UART_PRIVATE_H

//--------------------------------------------------
typedef enum _boot_cmd {
    BOOT_GET          = 0x00,
    BOOT_VERSION      = 0x01,
    BOOT_GET_ID       = 0x02,
    BOOT_READ_MEM     = 0x11,
    BOOT_GO           = 0x21,
    BOOT_WRITE_MEM    = 0x31,
    BOOT_ERASE        = 0x43,
    BOOT_ERASE_EX     = 0x44,
    BOOT_WRITE_PROT   = 0x63,
    BOOT_WRITE_UNPROT = 0x73,
    BOOT_READ_PROT    = 0x82,
    BOOT_READ_UNPROT  = 0x92,

    BOOT_INIT = 0x7F,
} boot_cmd_t;

typedef enum _reply {
    REPLY_ACK  = 0x79,
    REPLY_NACK = 0x1F,
} reply_t;

//--------------------------------------------------
#endif
