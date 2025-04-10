/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_cdc_if.c
  * @version        : v1.0_Cube
  * @brief          : Usb device for Virtual Com Port.
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
#include "usbd_cdc_if.h"

/* USER CODE BEGIN INCLUDE */
#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

#include "gpio_if.h"
#include "tests.h"
#include "adc_if.h"
#include "app_export.h"
#include "relay_if.h"
#include "boot_uart_if.h"
#include "render.h"
#include "modes.h"
#include "button_if.h"
#include "resources.h"
#include "dac_if.h"
static uint8_t usb_trans_ok;
uint8_t usb_com_open;
static uint8_t usb_recieve_ok;
//------переменные только для чтения из других модулей------------
extern const int     ch_gl; 
//----------------------------------------------------------------
//------переменные только для чтения/записи из других модулей-----
extern int       btn_context;
extern uint32_t enc_cnt_diag;
//----------------------------------------------------------------
/*
при выводе в консоль используется кодировка UTF8 из набора символов UNICODE
если символ "А" в UNICODE это 0x0410 в UTF8 это 0xD090 
т. е. прибавить к UNICODE коду 0xC2 ,если больше 7f (от 0x00 по 0x7f UNICODE и UTF8 совпадают) 
и получим код UTF8
символ UTF8 ,если код > 7F, хранится как два байта char
для работы с терминалом Putty
в Terminal -> Local Echo        -> Froce ON
  Terminal -> Local Line Ending -> Froce ON
  Implicit CR in evere LF -> ON
  Implicit LF in evere CR -> ON
затем нажать на этойже вкладке Open
*/
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device library.
  * @{
  */

/** @addtogroup USBD_CDC_IF
  * @{
  */

/** @defgroup USBD_CDC_IF_Private_TypesDefinitions USBD_CDC_IF_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Defines USBD_CDC_IF_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */
/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Macros USBD_CDC_IF_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_Variables USBD_CDC_IF_Private_Variables
  * @brief Private variables.
  * @{
  */
/* Create buffer for reception and transmission           */
/* It's up to user to redefine and/or remove those define */
/** Received data over USB are stored in this buffer      */
uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];

/** Data to send over USB CDC are stored in this buffer   */
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

/* USER CODE BEGIN PRIVATE_VARIABLES */

/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Exported_Variables USBD_CDC_IF_Exported_Variables
  * @brief Public variables.
  * @{
  */

extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CDC_IF_Private_FunctionPrototypes USBD_CDC_IF_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t* pbuf, uint32_t *Len);
static int8_t CDC_TransmitCplt_FS(uint8_t *pbuf, uint32_t *Len, uint8_t epnum);

/* USER CODE BEGIN PRIVATE_FUNCTIONS_DECLARATION */

/* USER CODE END PRIVATE_FUNCTIONS_DECLARATION */

/**
  * @}
  */

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS =
{
  CDC_Init_FS,
  CDC_DeInit_FS,
  CDC_Control_FS,
  CDC_Receive_FS,
  CDC_TransmitCplt_FS
};

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Initializes the CDC media low layer over the FS USB IP
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Init_FS(void)
{
  /* USER CODE BEGIN 3 */
  /* Set Application Buffers */
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
  return (USBD_OK);
  /* USER CODE END 3 */
}

/**
  * @brief  DeInitializes the CDC media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_DeInit_FS(void)
{
  /* USER CODE BEGIN 4 */
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  Manage the CDC class requests
  * @param  cmd: Command code
  * @param  pbuf: Buffer containing command data (request parameters)
  * @param  length: Number of data to be sent (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t* pbuf, uint16_t length)
{
  /* USER CODE BEGIN 5 */
  USBD_SetupReqTypedef* req = (USBD_SetupReqTypedef*)pbuf;
  switch(cmd)
  {
    case CDC_SEND_ENCAPSULATED_COMMAND:

    break;

    case CDC_GET_ENCAPSULATED_RESPONSE:

    break;

    case CDC_SET_COMM_FEATURE:

    break;

    case CDC_GET_COMM_FEATURE:

    break;

    case CDC_CLEAR_COMM_FEATURE:

    break;

  /*******************************************************************************/
  /* Line Coding Structure                                                       */
  /*-----------------------------------------------------------------------------*/
  /* Offset | Field       | Size | Value  | Description                          */
  /* 0      | dwDTERate   |   4  | Number |Data terminal rate, in bits per second*/
  /* 4      | bCharFormat |   1  | Number | Stop bits                            */
  /*                                        0 - 1 Stop bit                       */
  /*                                        1 - 1.5 Stop bits                    */
  /*                                        2 - 2 Stop bits                      */
  /* 5      | bParityType |  1   | Number | Parity                               */
  /*                                        0 - None                             */
  /*                                        1 - Odd                              */
  /*                                        2 - Even                             */
  /*                                        3 - Mark                             */
  /*                                        4 - Space                            */
  /* 6      | bDataBits  |   1   | Number Data bits (5, 6, 7, 8 or 16).          */
  /*******************************************************************************/
    case CDC_SET_LINE_CODING:

    break;

    case CDC_GET_LINE_CODING:

    break;

    case CDC_SET_CONTROL_LINE_STATE:
      if(req->wValue == 3)
          {usb_com_open = 1;}
      else
          {usb_com_open = 0;}
    break;

    case CDC_SEND_BREAK:
    
    break;

  default:
    break;
  }

  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Data received over USB OUT endpoint are sent over CDC interface
  *         through this function.
  *
  *         @note
  *         This function will issue a NAK packet on any OUT packet received on
  *         USB endpoint until exiting this function. If you exit this function
  *         before transfer is complete on CDC interface (ie. using DMA controller)
  *         it will result in receiving more data while previous ones are still
  *         not sent.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_Receive_FS(uint8_t* Buf, uint32_t *Len)
{
  /* USER CODE BEGIN 6 */
  USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
  //USBD_CDC_ReceivePacket(&hUsbDeviceFS);
  usb_recieve_ok = 1;
  return (USBD_OK);
  /* USER CODE END 6 */
}

/**
  * @brief  CDC_Transmit_FS
  *         Data to send over USB IN endpoint are sent over CDC interface
  *         through this function.
  *         @note
  *
  *
  * @param  Buf: Buffer of data to be sent
  * @param  Len: Number of data to be sent (in bytes)
  * @retval USBD_OK if all operations are OK else USBD_FAIL or USBD_BUSY
  */
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 7 */
  usb_trans_ok = 0;
  USBD_CDC_HandleTypeDef *hcdc = (USBD_CDC_HandleTypeDef*)hUsbDeviceFS.pClassData;
  if (hcdc->TxState != 0){
    return USBD_BUSY;
  }
  USBD_CDC_SetTxBuffer(&hUsbDeviceFS, Buf, Len);
  result = USBD_CDC_TransmitPacket(&hUsbDeviceFS);
  /*
  Символ новый строки в определенных случаях теряется при выводе, к примеру:
  printf("Hello\n");   - символ новый строки "\n" потеряется
  printf("Hello\n",1); - вот так не потеряется

  UPD: проверка выполнена
  при printf("Hello\n") вызывается _write с len 5 "Hello", а затем 
  сразу _write с len 1 "\n", usb cdc не успевает обработать вызовы _write
  (надо подождать пока придет прерывание об отправленном пакете usb cdc) 
  поэтому ждем тут отправки сообщения in while
  */
  uint32_t ticks = HAL_GetTick();
  while(usb_com_open && !usb_trans_ok){
    if(HAL_GetTick() - ticks > 5)
        {break;}
  }
  /* USER CODE END 7 */
  return result;
}

/**
  * @brief  CDC_TransmitCplt_FS
  *         Data transmitted callback
  *
  *         @note
  *         This function is IN transfer complete callback used to inform user that
  *         the submitted Data is successfully sent over USB.
  *
  * @param  Buf: Buffer of data to be received
  * @param  Len: Number of data received (in bytes)
  * @retval Result of the operation: USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CDC_TransmitCplt_FS(uint8_t *Buf, uint32_t *Len, uint8_t epnum)
{
  uint8_t result = USBD_OK;
  /* USER CODE BEGIN 13 */
  UNUSED(Buf);
  UNUSED(Len);
  UNUSED(epnum);
  usb_trans_ok = 1;
  /* USER CODE END 13 */
  return result;
}

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
/*
при использовании strncmp если сначала сравнивается strncmp("цап", 6), а потом ниже strncmp ("цап ток", 13)
то всегда будет выполняться условие по "цап"
*/
#define compare_str(x,y,cnt) strncmp((const char*)x,y,cnt) == 0
void usb_parse(usb_packet* pk)
{
    if(usb_recieve_ok == 0)
        { return;}

    if(strcmp((const char*)UserRxBufferFS,       "тест 1") == 0)            //[строка:тест 1]
        {pk->cmd =  1;}
    else if(strcmp((const char*)UserRxBufferFS,  "тест 2") == 0)            //[строка:тест 2]
        {pk->cmd =  2;}
    else if(strcmp((const char*)UserRxBufferFS,  "тест 31") == 0)           //[строка:тест 31]
        {pk->cmd =  4;}
    else if(strcmp((const char*)UserRxBufferFS,  "тест 32") == 0)           //[строка:тест 32]
        {pk->cmd =  5;} 
    else if(strcmp((const char*)UserRxBufferFS,  "тест 33") == 0)           //[строка:тест 33]
        {pk->cmd =  6;} 
    else if(strcmp((const char*)UserRxBufferFS,  "тест 34") == 0)           //[строка:тест 34]
        {pk->cmd =  7;} 
    else if(strcmp((const char*)UserRxBufferFS,  "тест 41") == 0)           //[строка:тест 41]
        {pk->cmd =  8;}
    else if(strcmp((const char*)UserRxBufferFS,  "тест 42") == 0)           //[строка:тест 42]
        {pk->cmd =  9;}
    else if(strcmp((const char*)UserRxBufferFS,  "старт")== 0)              //[строка:старт]
        {pk->cmd =  10;}
    else if(strcmp((const char*)UserRxBufferFS,  "выкл 24") == 0)           //[строка:выкл 24]
        {pk->cmd = -1;}
    else if(strcmp((const char*)UserRxBufferFS,  "вкл 24") == 0)            //[строка:вкл 24]
        {pk->cmd = -2;}
    else if(strncmp((const char*)UserRxBufferFS, "цап реал",15) == 0){      //[строка:цап реал][пробел][int:канал][пробел][float:данные]
         pk->ch = atoi((const char*)&UserRxBufferFS[16]);
         pk->data_f = atof((const char*)&UserRxBufferFS[18]);
         if(pk->ch != 1 && pk->ch != 2)
             {printf("ошибка выбора канала! первый: 1, второй: 2\n");}
         else
             {pk->cmd = -3;}
    }else if(strncmp((const char*)UserRxBufferFS,"цап бин",13) == 0){       //[строка:цап бин][пробел][int:канал][пробел][bin:данные]
         pk->ch = atoi((const char*)&UserRxBufferFS[14]);
         pk->dac_bin = atoi((const char*)&UserRxBufferFS[16]);
         if(pk->ch != 1 && pk->ch != 2)
             {printf("ошибка выбора канала! первый: 1, второй: 2\n");}
         else
             {pk->cmd = -4;}
    }else if(strcmp((const char*)UserRxBufferFS, "калибровка цап") == 0){   //[строка:калибровка цап]
         pk->cmd =  3;
    }else if(strncmp((const char*)UserRxBufferFS,"ацп ток реал",22) == 0){        //[строка:ацп ток][пробел][int:канал]   
         pk->ch = atoi((const char*)&UserRxBufferFS[23]);
         if(pk->ch != 1 && pk->ch != 2)
             {printf("ошибка выбора канала! первый: 1, второй: 2\n");}
         else
             {pk->cmd = -5;}
    }else if(strcmp((const char*)UserRxBufferFS, "boot") == 0){             //[строка:boot]
         pk->cmd = -6;
    }else if(strcmp((const char*)UserRxBufferFS, "boot_nb") == 0){          //[строка:boot_nb]
         pk->cmd = -7;
    }else if(strncmp((const char*)UserRxBufferFS,"pic",3) == 0){
         pk->cmd = -8;
         pk->dt = atoi((const char*)&UserRxBufferFS[4]);
    }else if(strcmp((const char*)UserRxBufferFS,"boot pin on") == 0){
      ENABLE_BOOT;
    }else if(strcmp((const char*)UserRxBufferFS,"boot pin off") == 0){
      DISABLE_BOOT;  
    }else if(strncmp((const char*)UserRxBufferFS,"lpa mode",8) == 0){       //[строка:lpa mode][пробел][int:mode]
         pk->cmd = -9;
         pk->dt = atoi((const char*)&UserRxBufferFS[9]);
    }else if(strncmp((const char*)UserRxBufferFS,"K6 on",5) == 0){
       pk->cmd = -10;
       pk->dt = atoi((const char*)&UserRxBufferFS[6]);
    }else if(strncmp((const char*)UserRxBufferFS,"K6 off",6) == 0){
       pk->cmd = -11;
       pk->dt = atoi((const char*)&UserRxBufferFS[7]);
    }else if(strcmp((const char*)UserRxBufferFS, "relay init") == 0){
      pk->cmd = -12;
    }else if(strncmp((const char*)UserRxBufferFS, "диагностика",22) == 0){    //[строка:диагностика][пробел][int:канал]
      pk->ch = atoi((const char*)&UserRxBufferFS[11 * 2 + 1]); // 11 символов в кириллице в utf * 2 + 1 пробел
      if(pk->ch != 1 && pk->ch != 2 && pk->ch != 3)
        { printf("ошибка выбора канала! первый: 1, второй: 2\n");}
      else
        { pk->cmd = -13;}
    }else if(strcmp((const char*)UserRxBufferFS, "ручная диагностика вкл") == 0){
        btn_context = c_Diagnostics;
        enc_cnt_diag = 0;
    }else if(strcmp((const char*)UserRxBufferFS, "ручная диагностика выкл") == 0){
        btn_context = c_ChooseCh;
    }else if(strcmp((const char*)UserRxBufferFS, "адрес платы") == 0){
      //printf("id_read: %d\n",id_read());
      printf("id_read: "BYTE_TO_BINARY_PATTERN, BYTE_TO_BINARY(id_read()));
    }else if(strncmp((const char*)UserRxBufferFS,"ацп напр бин",22) == 0){        //[строка:ацп ток][пробел][int:канал]   
      pk->ch = atoi((const char*)&UserRxBufferFS[10 * 2 + 3]);//10 символов в кириллице в utf * 2 + 3 пробела
      if(pk->ch != 1 && pk->ch != 2)
          {printf("ошибка выбора канала! первый: 1, второй: 2\n");}
      else
          {pk->cmd = -14;}
    }else if(strncmp((const char*)UserRxBufferFS,"ацп ток бин",20) == 0){          //[строка:ацп ток][пробел][int:канал]   
      pk->ch = atoi((const char*)&UserRxBufferFS[9 * 2 + 3]);//9 символов в кириллице в utf * 2 + 3 пробела
      if(pk->ch != 1 && pk->ch != 2)
          {printf("ошибка выбора канала! первый: 1, второй: 2\n");}
      else
          {pk->cmd = -15;}
    }else if(compare_str(UserRxBufferFS,"цап тест бин",22)){
      pk->ch = atoi((const char*)&UserRxBufferFS[23]);
      pk->dac_bin = atoi((const char*)&UserRxBufferFS[25]);
      if(pk->ch != 1 && pk->ch != 2)
          {printf("ошибка выбора канала! первый: 1, второй: 2\n");}
      else
          {pk->cmd = -16;}
    }else
         { printf("неизвестная команда ☺\n");}
        
    usb_recieve_ok = 0;
    memset(UserRxBufferFS,0,APP_RX_DATA_SIZE);//обнуляем буфер (иначе накладываются предыдущие команды)
    USBD_CDC_ReceivePacket(&hUsbDeviceFS);//ждем следующую команду
  return;
}

void usb_task(usb_packet* ub)
{
  float tmp_f = 0;
    if(       ub->cmd ==  0)
        {return;}
    else if(  ub->cmd ==  1)
        {test_1(ch_gl,          break_off);}
    else if(  ub->cmd ==  2)
        {test_2(ch_gl,          break_off);}
    else if(  ub->cmd ==  3)
        {calibration_dacs(ch_gl,break_off);}
    else if(  ub->cmd ==  4)
        {test_3a(ch_gl,        break_off);}
    else if(  ub->cmd ==  5)
        {test_3b(ch_gl,        break_off);}
    else if(  ub->cmd ==  6)
        {test_3c(ch_gl,        break_off);}
    else if(  ub->cmd ==  7)
        {test_3d(ch_gl,        break_off);}
    else if(  ub->cmd ==  8)
        {test_4a(ch_gl,        break_off);}
    else if(  ub->cmd ==  9)
        {test_4b(ch_gl,        break_off);}
    else if(  ub->cmd == 10)
        {all_test(ch_gl,        break_off);}
    else if(  ub->cmd == -2)
        {while(relay_set(TM_142_RELAY_POWER, CH_1, STATE_ON) == 0) { ;}}
    else if(  ub->cmd == -1)
        {while(relay_set(TM_142_RELAY_POWER, CH_1, STATE_OFF) == 0) { ;}}
    else if(  ub->cmd == -3)
        { dac_set_i(ub->ch,ub->data_f);}
    else if(  ub->cmd == -4)
        { dac_set(ub->ch,ub->dac_bin);}
    else if(  ub->cmd == -5){
      adc_get_value_f(ub->ch, TM_142_ADC_FEEDBACK, &tmp_f);
      printf("ацп %d, ток:%fmA\n",ub->ch,tmp_f);
    }else if( ub->cmd == -6){
      printf("boot_update: %d\n",boot_update());
    }else if( ub->cmd == -7){
      while(1){
        uint8_t nb = boot_update_nb();
        if(nb)
            {
              printf("boot_update_nb: %d",nb);
              break;
            }
      }
    }else if( ub->cmd == -8){
      render_image(0,30,0,1, &errors_img[ub->dt]); 
    }else if( ub->cmd == -9){
      char res  = 0;
      if(ub->dt == 0)
          { res = set_lpa_mode(SENSOR_TYPE_NAMUR | OUTPUT_TYPE_BOT | DIRECT_OUT | DIRECT_ERR);}
      else if(ub->dt == 1)
          { res = set_lpa_mode(SENSOR_TYPE_NAMUR | OUTPUT_TYPE_BOT | INVERTED_OUT | INVERTED_ERR);}
      else if(ub->dt == 2)
          { res = set_lpa_mode(SENSOR_TYPE_NAMUR | OUTPUT_TYPE_TOP | DIRECT_OUT | DIRECT_ERR);}
      else if(ub->dt == 3)
          { res = set_lpa_mode(SENSOR_TYPE_NAMUR | OUTPUT_TYPE_TOP | INVERTED_OUT | INVERTED_ERR);}

      printf("set_lpa_mode %d: %d",ub->dt,res);
    }else if( ub->cmd == -10){ //K6 on
       relay_set(TM_142_RELAY_SENSOR, ub->dt, TM_142_SENSOR_DIG);//K6
    }else if( ub->cmd == -11){ //K6 off
       relay_set(TM_142_RELAY_SENSOR, ub->dt, TM_142_SENSOR_ANA);//K6
    }else if( ub->cmd == -12){
      relay_init();
    }else if( ub->cmd == -13){

        while(relay_set(TM_142_RELAY_POWER, CH_1, STATE_ON) == 0) { ;}
        HAL_Delay(400);
        if(ub->ch == 1 || ub->ch == 2)
            { diagnostics(ub->ch);}
        else if(ub->ch == 3){

          for(int i = 1; i < ub->ch;i++){
             printf("канал %d\n",i);
             diagnostics(i);
          }
        }

        printf("exit\n");
        while(relay_set(TM_142_RELAY_POWER, CH_1, STATE_OFF) == 0) { ;}
    }else if( ub->cmd == -14){
      relay_set(TM_142_RELAY_U0,     ub->ch, TM_142_U0_ENABLE); //K7 //это реле надо включить
      uint16_t tmp = 0;
      adc_get_value(ub->ch, TM_142_ADC_OPENCIRC, &tmp);
      printf("ацп %d напр бин %d", ub->ch, tmp);
    }else if( ub->cmd == -15){
      relay_set(TM_142_RELAY_U0,     ub->ch, TM_142_U0_DISABLE);//K7
      relay_set(TM_142_RELAY_SENSOR, ub->ch, TM_142_SENSOR_ANA);//K6
      uint16_t tmp = 0;
      adc_get_value(ub->ch, TM_142_ADC_FEEDBACK, &tmp);
      printf("ацп %d ток бин %d", ub->ch, tmp);
    }else if( ub->cmd == -16){
      dac_set(ub->ch,ub->dac_bin);
      int i = 15;
      float tmp_f = 99999.9;
      while(i--)
      {
        adc_get_value_f(ub->ch, TM_142_ADC_FEEDBACK, &tmp_f);
        printf("%2.1f\n",tmp_f);
        HAL_Delay(400);
      }
    }
}


/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */

/**
  * @}
  */

/**
  * @}
  */
