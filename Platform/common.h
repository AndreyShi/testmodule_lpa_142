//--------------------------------------------------
// Общие определения для всей периферии
//--------------------------------------------------
#ifndef LIB_SERIALIZE_COMMON_H
#define LIB_SERIALIZE_COMMON_H

//--------------------------------------------------
#include <stdint.h>

//--------------------------------------------------
#define MAX_BUS_BUFF_SIZE 4096
#define MAX_CMD_STRING_SIZE 255
#define VERSION_STRING_SIZE 3*4+4 // format is "000.000.000.000\0"

//--------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

extern const char field_delim;

#ifdef __cplusplus
};
#endif
//--------------------------------------------------
#if (defined _MSC_VER) || (defined __MINGW32__) || (defined __MINGW64__)
#pragma pack(push, 1)
typedef enum _state:unsigned char {/*{{{*/
#else
typedef enum __attribute__ ((__packed__)) _state {
#endif
    STATE_BASE = 0,
    STATE_ON = 0,
    STATE_OFF
} state_t;/*}}}*/
#if (defined _MSC_VER) || (defined __MINGW32__) || (defined __MINGW64__)
typedef enum _channels:unsigned char {/*{{{*/
#else
typedef enum __attribute__ ((__packed__)) _channels {
#endif
    CH_BASE = 0x01,
    CH_AUTO = 0x00,

    CH_1 = 0x01,
    CH_2,
    CH_3,
    CH_4,
    CH_5,
    CH_6,
    CH_7,
    CH_8,

    CH_ALL  = 0xFF,
} channels_t;/*}}}*/
#if (defined _MSC_VER) || (defined __MINGW32__) || (defined __MINGW64__)
typedef enum _adc_input:unsigned char {/*{{{*/
#else
typedef enum __attribute__ ((__packed__)) _adc_input {
#endif
    ADC_INPUT_BASE = 0,
    ADC_INPUT_1 = 0,
    ADC_INPUT_2,
    ADC_INPUT_3,
    ADC_INPUT_4,
    ADC_INPUT_5,
    ADC_INPUT_6,
    ADC_INPUT_7,
    ADC_INPUT_8,
    ADC_INPUT_9,
    ADC_INPUT_10,
    ADC_INPUT_11,
    ADC_INPUT_12,
} adc_input_t;/*}}}*/
#if (defined _MSC_VER) || (defined __MINGW32__) || (defined __MINGW64__)
typedef enum _led_id:unsigned char {/*{{{*/
#else
typedef enum __attribute__ ((__packed__)) _led_id {
#endif
    LED_BASE = 1,
    LED_1 = 1,
    LED_2,
    LED_3,
    LED_4,
    LED_5,
    LED_6,
    LED_7,
    LED_8,
    LED_9,
    LED_10,
    LED_11,
    LED_12,
    LED_13,
    LED_14,
    LED_15,
    LED_16,
} led_id_t;/*}}}*/
#if (defined _MSC_VER) || (defined __MINGW32__) || (defined __MINGW64__)
typedef enum _relay_id:unsigned char {/*{{{*/
#else
typedef enum __attribute__ ((__packed__)) _relay_id {
#endif
    RELAY_BASE = 33,
    RELAY_1 = 33,
    RELAY_2,
    RELAY_3,
    RELAY_4,
    RELAY_5,
    RELAY_6,
    RELAY_7,
    RELAY_8,
    RELAY_9,
    RELAY_10,
    RELAY_11,
    RELAY_12,
    RELAY_13,
    RELAY_14,
    RELAY_15,
    RELAY_16,
    RELAY_17,
    RELAY_18,
    RELAY_19,
    RELAY_20,
    RELAY_21,
    RELAY_22,
    RELAY_23,
    RELAY_24,
    RELAY_25,
    RELAY_26,
    RELAY_27,
    RELAY_28,
    RELAY_29,
    RELAY_30,
    RELAY_31,
    RELAY_32,
    RELAY_33,
    RELAY_34,
    RELAY_35,
    RELAY_36,
    RELAY_37,
    RELAY_38,
    RELAY_39,
    RELAY_40,
    RELAY_41,
    RELAY_42,
    RELAY_43,
    RELAY_44,
    RELAY_45,
    RELAY_46,
    RELAY_47,
    RELAY_48,
    RELAY_49,
    RELAY_50,
    RELAY_51,
    RELAY_52,
    RELAY_53,
    RELAY_54,
    RELAY_55,
    RELAY_56,
    RELAY_57,
    RELAY_58,
    RELAY_59,
    RELAY_60,
    RELAY_61,
    RELAY_62,
    RELAY_63,
    RELAY_64,
} relay_id_t;/*}}}*/
#if (defined _MSC_VER) || (defined __MINGW32__) || (defined __MINGW64__)
typedef enum _dac_output_id:unsigned char {/*{{{*/
#else
typedef enum __attribute__ ((__packed__)) _dac_output_id {
#endif
    DAC_OUTPUT_BASE = 162,
    DAC_OUTPUT_1 = 162,
    DAC_OUTPUT_2,
    DAC_OUTPUT_3,
    DAC_OUTPUT_4,
    DAC_OUTPUT_5,
    DAC_OUTPUT_6,
    DAC_OUTPUT_7,
    DAC_OUTPUT_8,
} dac_output_t;/*}}}*/
#if (defined _MSC_VER) || (defined __MINGW32__) || (defined __MINGW64__)
typedef enum _button_id:unsigned char {/*{{{*/
#else
typedef enum __attribute__ ((__packed__)) _button_id {
#endif
    BUTTON_ALL = 170,

    BUTTON_BASE = 171,
    BUTTON_1 = 171,
    BUTTON_2,
    BUTTON_3,
    BUTTON_4,
    BUTTON_5,
    BUTTON_6,
    BUTTON_7,
    BUTTON_8,
} button_id_t;/*}}}*/
#if (defined _MSC_VER) || (defined __MINGW32__) || (defined __MINGW64__)
typedef enum _input_id:unsigned char {/*{{{*/
#else
typedef enum __attribute__ ((__packed__)) _input_id {
#endif
    INPUT_BASE = 180,
    INPUT_1 = 180,
    INPUT_2,
    INPUT_3,
    INPUT_4,
    INPUT_5,
    INPUT_6,
    INPUT_7,
    INPUT_8,
    INPUT_9,
    INPUT_10,
    INPUT_11,
    INPUT_12,
    INPUT_13,
    INPUT_14,
    INPUT_15,
    INPUT_16,
    INPUT_17,
    INPUT_18,
    INPUT_19,
    INPUT_20,
    INPUT_21,
    INPUT_22,
    INPUT_23,
    INPUT_24,
    INPUT_25,
    INPUT_26,
    INPUT_27,
    INPUT_28,
    INPUT_29,
    INPUT_30,
    INPUT_31,
    INPUT_32,
} input_id_t;/*}}}*/
#if (defined _MSC_VER) || (defined __MINGW32__) || (defined __MINGW64__)
typedef enum _version_id:unsigned char {/*{{{*/
#else
typedef enum __attribute__ ((__packed__)) _version_id {
#endif
    VERSION_BASE = 0,
    VERSION_1 = 0,
    VERSION_2,
    VERSION_3,
    VERSION_4,
    VERSION_5,
} version_id_t;/*}}}*/
#if (defined _MSC_VER) || (defined __MINGW32__) || (defined __MINGW64__)
typedef enum _data_id:unsigned char {/*{{{*/
#else
typedef enum __attribute__ ((__packed__)) _data_id {
#endif
    DATA_ID_BASE = 0,
    DATA_ID_1 = 0,
    DATA_ID_2,
    DATA_ID_3,
    DATA_ID_4,
    DATA_ID_5,
    DATA_ID_6,
    DATA_ID_7,
    DATA_ID_8,
    DATA_ID_9,
} data_id_t;/*}}}*/
#if (defined _MSC_VER) || (defined __MINGW32__) || (defined __MINGW64__)
typedef enum _script_id:unsigned char {/*{{{*/
#else
typedef enum __attribute__ ((__packed__)) _script_id {
#endif
    SCRIPT_ID_BASE = 100,
    SCRIPT_ID_1 = 100,
    SCRIPT_ID_2,
    SCRIPT_ID_3,
    SCRIPT_ID_4,
    SCRIPT_ID_5,
    SCRIPT_ID_6,
    SCRIPT_ID_7,
} script_id_t;/*}}}*/
#if (defined _MSC_VER) || (defined __MINGW32__) || (defined __MINGW64__)
typedef enum _uart_port:unsigned char {/*{{{*/
#else
typedef enum __attribute__ ((__packed__)) _uart_port {
#endif
    UART_PORT_BASE = 0,
    UART_PORT_1 = 0,
    UART_PORT_2,
    UART_PORT_3,
    UART_PORT_4,
    UART_PORT_5,
    UART_PORT_6,
    UART_PORT_7,
    UART_PORT_8,
} uart_port_t;/*}}}*/
#if (defined _MSC_VER) || (defined __MINGW32__) || (defined __MINGW64__)
typedef enum _timer_id:unsigned char {/*{{{*/
#else
typedef enum __attribute__ ((__packed__)) _timer_id {
#endif
    TIMER_ID_BASE = 0,
    TIMER_ID_1 = 0,
    TIMER_ID_2,
    TIMER_ID_3,
    TIMER_ID_4,
} timer_id_t;/*}}}*/
#if (defined _MSC_VER) || (defined __MINGW32__) || (defined __MINGW64__)
#pragma pack(pop)
#endif
//--------------------------------------------------
#endif
