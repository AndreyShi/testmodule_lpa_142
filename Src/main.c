/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "iwdg.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_otg.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_export.h"
#include "gpio_if.h"
#include "relay_if.h"
#include "adc_if.h"
#include "dac_if.h"
#include "ssd1306.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_DAC_Init();
  MX_SPI2_Init();
  //MX_TIM1_Init();
  //MX_TIM3_Init();
  //MX_TIM6_Init();
  //MX_TIM8_Init();
  //MX_TIM9_Init();
  //MX_TIM12_Init();
  //MX_USART2_UART_Init();
  //MX_USART3_UART_Init();
  //MX_USB_OTG_FS_PCD_Init();
  /* USER CODE BEGIN 2 */
  dac_start();
  ssd1306_init();
  

    /* initialize module state */
  relay_set(TM_142_RELAY_U0, CH_1, TM_142_U0_DISABLE);
  relay_set(TM_142_RELAY_U0, CH_2, TM_142_U0_DISABLE);
  relay_set(TM_142_RELAY_SENSOR, CH_1, TM_142_SENSOR_ANA);
  relay_set(TM_142_RELAY_SENSOR, CH_2, TM_142_SENSOR_ANA);
  relay_set(TM_142_RELAY_INPUT, CH_1, TM_142_BOT_SW);
  relay_set(TM_142_RELAY_INPUT, CH_2, TM_142_BOT_SW);
  relay_set(TM_142_RELAY_ERROR, CH_1, TM_142_BOT_SW);
  relay_set(TM_142_RELAY_ERROR, CH_2, TM_142_BOT_SW);
  input_read(TM_142_INPUT_INPUT, CH_1, 0);
  input_read(TM_142_INPUT_INPUT, CH_2, 0);
  input_read(TM_142_INPUT_ERROR, CH_1, 0);
  input_read(TM_142_INPUT_ERROR, CH_2, 0);

  dac_set(CH_ALL, 0);

  adc_get_value(CH_1, TM_142_ADC_OPENCIRC, 0);
  adc_get_value(CH_1, TM_142_ADC_FEEDBACK, 0);
  adc_get_value(CH_2, TM_142_ADC_OPENCIRC, 0);
  adc_get_value(CH_2, TM_142_ADC_FEEDBACK, 0);

  /* calibration procedure {{{ */
  /*
  relay_set(TM_142_RELAY_U0, CH_1, STATE_ON);
  relay_set(TM_142_RELAY_U0, CH_2, STATE_ON);

  while(relay_set(TM_142_RELAY_POWER, CH_1, STATE_ON) == 0) { }

  uint16_t tmp = 0;
  adc_get_value(CH_1, TM_142_ADC_OPENCIRC, &tmp);
  adc_get_value(CH_1, TM_142_ADC_OPENCIRC, &tmp);
  adc_get_value(CH_1, TM_142_ADC_OPENCIRC, &tmp);
  adc_get_value(CH_1, TM_142_ADC_OPENCIRC, &tmp);
  adc_get_value(CH_1, TM_142_ADC_OPENCIRC, &tmp);
  adc_get_value(CH_1, TM_142_ADC_OPENCIRC, &tmp);

  adc_get_value(CH_2, TM_142_ADC_OPENCIRC, &tmp);
  adc_get_value(CH_2, TM_142_ADC_OPENCIRC, &tmp);
  adc_get_value(CH_2, TM_142_ADC_OPENCIRC, &tmp);
  adc_get_value(CH_2, TM_142_ADC_OPENCIRC, &tmp);
  adc_get_value(CH_2, TM_142_ADC_OPENCIRC, &tmp);
  adc_get_value(CH_2, TM_142_ADC_OPENCIRC, &tmp);
  
  relay_set(TM_142_RELAY_U0, CH_1, TM_142_U0_DISABLE);
  relay_set(TM_142_RELAY_U0, CH_2, TM_142_U0_DISABLE);

  dac_set(CH_1, 0x0000);
  adc_get_value(CH_1, TM_142_ADC_FEEDBACK, &tmp);
  dac_set(CH_1, 0x0400);
  adc_get_value(CH_1, TM_142_ADC_FEEDBACK, &tmp);
  dac_set(CH_1, 0x0800);
  adc_get_value(CH_1, TM_142_ADC_FEEDBACK, &tmp);
  dac_set(CH_1, 0x0C00);
  adc_get_value(CH_1, TM_142_ADC_FEEDBACK, &tmp);
  dac_set(CH_1, 0x0000);

  dac_set(CH_2, 0x0000);
  adc_get_value(CH_2, TM_142_ADC_FEEDBACK, &tmp);
  dac_set(CH_2, 0x0400);
  adc_get_value(CH_2, TM_142_ADC_FEEDBACK, &tmp);
  dac_set(CH_2, 0x0800);
  adc_get_value(CH_2, TM_142_ADC_FEEDBACK, &tmp);
  dac_set(CH_2, 0x0C00);
  adc_get_value(CH_2, TM_142_ADC_FEEDBACK, &tmp);
  dac_set(CH_2, 0x0000);

while(relay_set(TM_142_RELAY_POWER, CH_1, STATE_OFF) == 0) { }
*/
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
  __asm("nop");
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 72;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
