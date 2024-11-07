/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#define encoder_max 4096
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define D4_Pin GPIO_PIN_2
#define D4_GPIO_Port GPIOE
#define D5_Pin GPIO_PIN_3
#define D5_GPIO_Port GPIOE
#define D6_Pin GPIO_PIN_4
#define D6_GPIO_Port GPIOE
#define Ch1_gen_ref_Pin GPIO_PIN_5
#define Ch1_gen_ref_GPIO_Port GPIOE
#define Ch1_gen_out_Pin GPIO_PIN_6
#define Ch1_gen_out_GPIO_Port GPIOE
#define Led_4_Pin GPIO_PIN_13
#define Led_4_GPIO_Port GPIOC
#define osc_in_Pin GPIO_PIN_0
#define osc_in_GPIO_Port GPIOH
#define osc_out_Pin GPIO_PIN_1
#define osc_out_GPIO_Port GPIOH
#define Led_1_Pin GPIO_PIN_0
#define Led_1_GPIO_Port GPIOC
#define Led_2_Pin GPIO_PIN_1
#define Led_2_GPIO_Port GPIOC
#define Led_3_Pin GPIO_PIN_2
#define Led_3_GPIO_Port GPIOC
#define Disp_MOSI_Pin GPIO_PIN_3
#define Disp_MOSI_GPIO_Port GPIOC
#define Ch1_adc_fb_Pin GPIO_PIN_0
#define Ch1_adc_fb_GPIO_Port GPIOA
#define Ch1_adc_U0_Pin GPIO_PIN_1
#define Ch1_adc_U0_GPIO_Port GPIOA
#define Ch2_adc_fb_Pin GPIO_PIN_2
#define Ch2_adc_fb_GPIO_Port GPIOA
#define Ch2_adc_U0_Pin GPIO_PIN_3
#define Ch2_adc_U0_GPIO_Port GPIOA
#define Ch1_dac_Pin GPIO_PIN_4
#define Ch1_dac_GPIO_Port GPIOA
#define Ch2_dac_Pin GPIO_PIN_5
#define Ch2_dac_GPIO_Port GPIOA
#define Encoder_A_Pin GPIO_PIN_6
#define Encoder_A_GPIO_Port GPIOA
#define Encoder_B_Pin GPIO_PIN_7
#define Encoder_B_GPIO_Port GPIOA
#define Ch1_sensor_type_Pin GPIO_PIN_4
#define Ch1_sensor_type_GPIO_Port GPIOC
#define Ch1_U0_enable_Pin GPIO_PIN_5
#define Ch1_U0_enable_GPIO_Port GPIOC
#define Dbg_0_Pin GPIO_PIN_0
#define Dbg_0_GPIO_Port GPIOB
#define Dbg_1_Pin GPIO_PIN_1
#define Dbg_1_GPIO_Port GPIOB
#define Dbg_2_Pin GPIO_PIN_2
#define Dbg_2_GPIO_Port GPIOB
#define Dbg_3_Pin GPIO_PIN_7
#define Dbg_3_GPIO_Port GPIOE
#define Ch1_ref_Pin GPIO_PIN_9
#define Ch1_ref_GPIO_Port GPIOE
#define Ch1_err_Pin GPIO_PIN_10
#define Ch1_err_GPIO_Port GPIOE
#define Ch1_in_Pin GPIO_PIN_11
#define Ch1_in_GPIO_Port GPIOE
#define Ch1_in_control1_Pin GPIO_PIN_12
#define Ch1_in_control1_GPIO_Port GPIOE
#define Ch1_in_control2_Pin GPIO_PIN_13
#define Ch1_in_control2_GPIO_Port GPIOE
#define Ch1_err_control1_Pin GPIO_PIN_14
#define Ch1_err_control1_GPIO_Port GPIOE
#define Ch1_err_control2_Pin GPIO_PIN_15
#define Ch1_err_control2_GPIO_Port GPIOE
#define Disp_SCK_Pin GPIO_PIN_10
#define Disp_SCK_GPIO_Port GPIOB
#define Disp_Rst_Pin GPIO_PIN_11
#define Disp_Rst_GPIO_Port GPIOB
#define Disp_CS_Pin GPIO_PIN_12
#define Disp_CS_GPIO_Port GPIOB
#define Disp_DC_Pin GPIO_PIN_13
#define Disp_DC_GPIO_Port GPIOB
#define Ch2_gen_ref_Pin GPIO_PIN_14
#define Ch2_gen_ref_GPIO_Port GPIOB
#define Ch2_gen_out_Pin GPIO_PIN_15
#define Ch2_gen_out_GPIO_Port GPIOB
#define BGND_enable_Pin GPIO_PIN_8
#define BGND_enable_GPIO_Port GPIOD
#define id_2_Pin GPIO_PIN_9
#define id_2_GPIO_Port GPIOD
#define id_3_Pin GPIO_PIN_10
#define id_3_GPIO_Port GPIOD
#define id_4_Pin GPIO_PIN_11
#define id_4_GPIO_Port GPIOD
#define Ch2_in_control1_Pin GPIO_PIN_12
#define Ch2_in_control1_GPIO_Port GPIOD
#define Ch2_in_control2_Pin GPIO_PIN_13
#define Ch2_in_control2_GPIO_Port GPIOD
#define Ch2_err_control1_Pin GPIO_PIN_14
#define Ch2_err_control1_GPIO_Port GPIOD
#define Ch2_err_control2_Pin GPIO_PIN_15
#define Ch2_err_control2_GPIO_Port GPIOD
#define Ch2_ref_Pin GPIO_PIN_6
#define Ch2_ref_GPIO_Port GPIOC
#define Ch2_in_Pin GPIO_PIN_7
#define Ch2_in_GPIO_Port GPIOC
#define Ch2_err_Pin GPIO_PIN_8
#define Ch2_err_GPIO_Port GPIOC
#define Encoder_S_Pin GPIO_PIN_9
#define Encoder_S_GPIO_Port GPIOC
#define Addr_0_Pin GPIO_PIN_8
#define Addr_0_GPIO_Port GPIOA
#define Addr_1_Pin GPIO_PIN_9
#define Addr_1_GPIO_Port GPIOA
#define Addr_2_Pin GPIO_PIN_10
#define Addr_2_GPIO_Port GPIOA
#define Usb_M_Pin GPIO_PIN_11
#define Usb_M_GPIO_Port GPIOA
#define Usb_P_Pin GPIO_PIN_12
#define Usb_P_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define bus_frame_out_Pin GPIO_PIN_15
#define bus_frame_out_GPIO_Port GPIOA
#define bus_tx_Pin GPIO_PIN_10
#define bus_tx_GPIO_Port GPIOC
#define bus_rx_Pin GPIO_PIN_11
#define bus_rx_GPIO_Port GPIOC
#define bus_frame_in_Pin GPIO_PIN_12
#define bus_frame_in_GPIO_Port GPIOC
#define bus_frame_in_EXTI_IRQn EXTI15_10_IRQn
#define Ch2_sensor_type_Pin GPIO_PIN_0
#define Ch2_sensor_type_GPIO_Port GPIOD
#define Ch2_U0_enable_Pin GPIO_PIN_1
#define Ch2_U0_enable_GPIO_Port GPIOD
#define LPA_Power_Pin GPIO_PIN_2
#define LPA_Power_GPIO_Port GPIOD
#define Bus_Enable_Pin GPIO_PIN_3
#define Bus_Enable_GPIO_Port GPIOD
#define Addr_3_Pin GPIO_PIN_4
#define Addr_3_GPIO_Port GPIOD
#define boot_tx_Pin GPIO_PIN_5
#define boot_tx_GPIO_Port GPIOD
#define boot_rx_Pin GPIO_PIN_6
#define boot_rx_GPIO_Port GPIOD
#define boot_enable_Pin GPIO_PIN_7
#define boot_enable_GPIO_Port GPIOD
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define Dbg_4_Pin GPIO_PIN_4
#define Dbg_4_GPIO_Port GPIOB
#define Dbg_5_Pin GPIO_PIN_5
#define Dbg_5_GPIO_Port GPIOB
#define Dbg_6_Pin GPIO_PIN_6
#define Dbg_6_GPIO_Port GPIOB
#define Dbg_7_Pin GPIO_PIN_7
#define Dbg_7_GPIO_Port GPIOB
#define D0_Pin GPIO_PIN_8
#define D0_GPIO_Port GPIOB
#define D1_Pin GPIO_PIN_9
#define D1_GPIO_Port GPIOB
#define D2_Pin GPIO_PIN_0
#define D2_GPIO_Port GPIOE
#define D3_Pin GPIO_PIN_1
#define D3_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
