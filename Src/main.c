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
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app_export.h"
#include "gpio_if.h"
#include "relay_if.h"
#include "adc_if.h"
#include "dac_if.h"
#include "ssd1306.h"
#include "usbd_cdc_if.h"
#include "tim_if.h"
#include "app_config.h"
#include "encoder.h"
#include "calibration.h"
int _write(int file, char *ptr, int len);
int id_stend142;
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
  setvbuf(stdin, NULL, _IONBF, 0);  
  setvbuf(stdout, NULL, _IONBF, 0);
  setvbuf(stderr, NULL, _IONBF, 0);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  MX_DAC_Init();
  MX_SPI2_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  //MX_TIM6_Init();
  MX_TIM8_Init();
  MX_TIM9_Init();
  MX_TIM12_Init();
  //MX_USART2_UART_Init();
  //MX_USART3_UART_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  dac_start();
  ssd1306_init();


    /* initialize module state */
  relay_set(TM_142_RELAY_U0, CH_1, TM_142_U0_DISABLE);// K7
  relay_set(TM_142_RELAY_U0, CH_2, TM_142_U0_DISABLE);// K7
  relay_set(TM_142_RELAY_SENSOR, CH_1, TM_142_SENSOR_ANA);//K6
  relay_set(TM_142_RELAY_SENSOR, CH_2, TM_142_SENSOR_ANA);//K6
  relay_set(TM_142_RELAY_INPUT, CH_1, TM_142_BOT_SW);//K2K4
  relay_set(TM_142_RELAY_INPUT, CH_2, TM_142_BOT_SW);//K2K4
  relay_set(TM_142_RELAY_ERROR, CH_1, TM_142_BOT_SW);//K3K5
  relay_set(TM_142_RELAY_ERROR, CH_2, TM_142_BOT_SW);//K3K5
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
  uint16_t tmp = 0;
 
  relay_set(TM_142_RELAY_U0, CH_1, STATE_ON);
  relay_set(TM_142_RELAY_U0, CH_2, STATE_ON);

  while(relay_set(TM_142_RELAY_POWER, CH_1, STATE_ON) == 0) { }
/*
while(1){
  adc_get_value(CH_1, TM_142_ADC_OPENCIRC, &tmp);//max 14.015 3916  min 1.004 282
  printf("ch1 adc: %d %2.3fV\n",tmp,calibrate_u0(tmp, CH_1));
  HAL_Delay(5000);
}
*/
/*
while(1){
  adc_get_value(CH_2, TM_142_ADC_OPENCIRC, &tmp);//max 14.011 3933 min 1.004 283 
  printf("ch2 adc: %d %2.3fV\n",tmp,calibrate_u0(tmp, CH_2));
  HAL_Delay(5000);
}
*/

  relay_set(TM_142_RELAY_U0, CH_1, TM_142_U0_DISABLE);
  relay_set(TM_142_RELAY_U0, CH_2, TM_142_U0_DISABLE);
  while(relay_set(TM_142_RELAY_POWER, CH_1, STATE_ON) == 0) { }

/*
while(1){
  dac_set_i(CH_1,1.001);//dac_set(CH_1, 221); // 1.001 mA agilent
  HAL_Delay(5000);
  adc_get_value(CH_1, TM_142_ADC_FEEDBACK, &tmp);//224
  printf("ch1 dac 221  adc: %d %2.3fmA\n",tmp,calibrate_i0(tmp, CH_1));
  HAL_Delay(5000);
  dac_set_i(CH_1,17.003);//dac_set(CH_1, 3759); // 17.003 mA agilent
  HAL_Delay(5000);
  adc_get_value(CH_1, TM_142_ADC_FEEDBACK, &tmp);//3755
  printf("ch1 dac 3759  adc: %d %2.3fmA\n",tmp,calibrate_i0(tmp, CH_1));
  HAL_Delay(5000);
}

while(1){
  dac_set(CH_2, 222); // 1.000 mA agilent
  HAL_Delay(5000);
  adc_get_value(CH_2, TM_142_ADC_FEEDBACK, &tmp);//225
  printf("ch2 dac 222  adc: %d %2.3fmA\n",tmp,calibrate_i0(tmp, CH_2));
  HAL_Delay(5000);
  dac_set(CH_2, 3779); // 17.003 mA agilent
  HAL_Delay(5000);
  adc_get_value(CH_2, TM_142_ADC_FEEDBACK, &tmp);//3776
  printf("ch2 dac 3779  adc: %d %2.3fmA\n",tmp,calibrate_i0(tmp, CH_2));
  HAL_Delay(5000);
}
*/
//while(relay_set(TM_142_RELAY_POWER, CH_1, STATE_OFF) == 0) { }
/**/

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  uint32_t tmp_tm = 0;
  float tmp_f = 0;
  while (1)
  {
    if(tmp_tm != htim3.Instance->CNT){


        //printf("%d dir:%d\n",htim3.Instance->CNT,htim3.Instance->CR1 & 0x10);
        printf("Hello\n");
        tmp_tm = htim3.Instance->CNT;

    }
    if(id_stend142 == 1){    //Тест 1 проверка Uo:
       relay_set(TM_142_RELAY_U0, CH_1, STATE_ON);//включить K7 первого канала
       HAL_Delay(100);//проверить паузу включения реле
       adc_get_value_f(CH_1, TM_142_ADC_OPENCIRC, &tmp_f);//измерить напряжение
       printf("канал %d %2.3fV\n",CH_1,tmp_f);
       if(tmp_f > 12.075F)//вывести ошибку в случае если U0 > 12.075 ошибка А, если U0 < 10 Б
          { printf("канал %d ошибка А\n",CH_1);}
       else if(tmp_f < 10.000F)
          { printf("канал %d ошибка Б\n",CH_1);}

       relay_set(TM_142_RELAY_U0, CH_2, STATE_ON);//включить K7 второго канала
       HAL_Delay(100);//проверить паузу включения реле
       adc_get_value_f(CH_2, TM_142_ADC_OPENCIRC, &tmp_f);//измерить напряжение
       printf("канал %d %2.3fV\n",CH_2,tmp_f);
       if(tmp_f > 12.075F)//вывести ошибку в случае если U0 > 12.075 ошибка А, если U0 < 10 Б
          { printf("канал %d ошибка А\n",CH_2);}
       else if(tmp_f < 10.000F)
          { printf("канал %d ошибка Б>\n",CH_2);}      
    }else if(id_stend142 == 2){
    //Тест 2 проверка I0:
    //подключить аналоговый имитатор датчика (отключить К7 и К6) первый канал
    //на ЦАП выставить 4095
    //измерить АЦП ,если I0 < 7,5 mA, то ошибка Г, если I0 > 10 mA то ошибка B
    //подключить аналоговый имитатор датчика (отключить К7 и К6) второго канала
    //на ЦАП выставить 4095
    //измерить АЦП ,если I0 < 7,5 mA, то ошибка Г, если I0 > 10 mA то ошибка B
    }else if(id_stend142 == 3){ 
    //Тест 3.1
    //подключен аналоговый имитатор датчика. (отключить К7 и К6) 
    }else if(id_stend142 == 41){
        uint16_t data = 0;
        tim_get_delay(CH_1, &data);
        __asm("nop");
    }
    /* USER CODE END WHILE */

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
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
/*
Символ новый строки в определенных случаях теряется при выводе, к примеру:
printf("Hello\n");   - символ новый строки "\n" потеряется
printf("Hello\n",1); - вот так не потеряется

UPD: проверка выполнена
при printf("Hello\n") вызывается _write с len 5 "Hello", а затем 
сразу _write с len 1 "\n", usb cdc не успевает обработать вызовы _write
(надо подождать пока придет прерывание об отправленном пакете usb cdc) 
поэтому ждем тут отправки предыдущего сообщения
*/
int _write(int file, char *ptr, int len)
{
    (void)file;
  usb_trans_ok = 0;
  //DBG_PIN_SET;
  CDC_Transmit_FS((uint8_t *)ptr, len);
  while(!usb_trans_ok && usb_com_open){;}
  //DBG_PIN_RS;
  return len;
} 

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)/*{{{*/
{
if(htim->Instance == ch1_meas_tim.Instance)
    { delay_tim_isr(htim); }
if(htim->Instance == enc_tim.Instance)
    { enc_tim_isr();}
if(htim->Instance == ch2_meas_tim.Instance)
    { delay_tim_isr(htim); }
}/*}}}*/
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
