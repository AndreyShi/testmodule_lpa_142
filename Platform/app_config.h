//--------------------------------------------------
// Конфигурация приложения
//--------------------------------------------------
#ifndef TESTMODULE_142_CONFIG_H
#define TESTMODULE_142_CONFIG_H

//--------------------------------------------------
#include "common.h"
#include "app_export.h"

// Идентификатор модуля
// (т.к. у разных модулей разные калибровки)
//--------------------------------------------------
#define MODULE_5501

// Псевдонимы для периферии
//--------------------------------------------------
#define boot_uart huart2
#define bus_uart  huart3

#define ch1_adc   hadc1
#define ch2_adc   hadc2

#define ch1_dac   hdac
#define ch2_dac   hdac

#define ssd_spi hspi2

#define ch1_gen_tim  htim9
#define ch1_meas_tim htim1

#define ch2_gen_tim  htim12
#define ch2_meas_tim htim8

#define bus_tim htim6

#define enc_tim htim3

// Настройки движка исполнения скриптов
//--------------------------------------------------
#define ENGINE_I_REG_COUNT 2
#define ENGINE_F_REG_COUNT 2

// Настройки расположения данных во флешке
//--------------------------------------------------
#define CALIB_SECTOR  FLASH_SECTOR_9
#define FW_SECTOR     FLASH_SECTOR_10
#define SCRIPT_SECTOR FLASH_SECTOR_11

//--------------------------------------------------
#endif
