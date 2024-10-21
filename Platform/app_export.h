//--------------------------------------------------
// Экспортируемая конфигурация
//--------------------------------------------------
#ifndef TESTMODULE_142_EXPORT_H
#define TESTMODULE_142_EXPORT_H

// Псевдонимы для светодиодов
//--------------------------------------------------
#define TM_142_LED_RED    LED_1
#define TM_142_LED_BLUE   LED_2
#define TM_142_LED_GREEN  LED_3
#define TM_142_LED_YELLOW LED_4
#define TM_142_LED_MAX    LED_5

// Псевдонимы для реле
//--------------------------------------------------
// пара рэле задает подключение одного из трех модулей
// 1 - измеритель U0
// 2 - имитатор датчика
// 3 - генератор сигнала для измерения задержки прохождения
//     сигнала через барьер
#define TM_142_RELAY_U0     RELAY_1
#define TM_142_RELAY_SENSOR RELAY_2

// выбор ключа на выходе сигнала и ошибки
#define TM_142_RELAY_INPUT  RELAY_3
#define TM_142_RELAY_ERROR  RELAY_4

// Подача питания на барьер
#define TM_142_RELAY_POWER  RELAY_5

// псевдонимы состояний реле (для удобства)
#define TM_142_U0_ENABLE  STATE_ON
#define TM_142_U0_DISABLE STATE_OFF

#define TM_142_SENSOR_DIG STATE_ON
#define TM_142_SENSOR_ANA STATE_OFF

#define TM_142_TOP_SW STATE_ON
#define TM_142_BOT_SW STATE_OFF

// Псевдоним для ЦАП
//--------------------------------------------------
#define TM_142_DAC DAC_OUTPUT_1

// Псевдонимы для АЦП
//--------------------------------------------------
#define TM_142_ADC_FEEDBACK ADC_INPUT_1
#define TM_142_ADC_OPENCIRC ADC_INPUT_2

// Псевдонимы для дискретных входов
//--------------------------------------------------
#define TM_142_INPUT_INPUT INPUT_1
#define TM_142_INPUT_ERROR INPUT_2

// Псевдоним для кнопок
//--------------------------------------------------
#define TM_142_BUTTON_LEFT  BUTTON_1
#define TM_142_BUTTON_GO    BUTTON_2
#define TM_142_BUTTON_RIGHT BUTTON_3

// Псевдонимы для версий
//--------------------------------------------------
#define TM_142_VERSION_MODULE VERSION_1
#define TM_142_VERSION_FW     VERSION_2
#define TM_142_VERSION_LPA    VERSION_3

// Псевдонимы для данных
//--------------------------------------------------
#define TM_142_DATA_FIRMWARE DATA_ID_1
#define TM_142_DATA_CRC      DATA_ID_2

// Всё, что касается скриптов
//--------------------------------------------------
#define TM_142_SCRIPTS_COUNT 1
#define TM_142_SCRIPT_SIZE 3072

//--------------------------------------------------
#endif
