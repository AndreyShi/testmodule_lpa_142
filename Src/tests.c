#include "tests.h"
#include "main.h"
#include "relay_if.h"
#include "adc_if.h"
#include "dac_if.h"
#include "app_export.h"
#include "gpio_if.h"
#include "tim_if.h"
#include <stdio.h>

//--задержки перед измерениями АЦП
#define DELAY_BOOT 1000 //1000ms
#define DELAY_1    500 //500ms
#define DELAY_2    400 //400ms


//---уровни используемые в предыдущем стенде
#define LEVEL_1 0x00, 0x00, 0x00, 0x00  //0.0F
#define LEVEL_2 0x3F, 0x00, 0x00, 0x00  //0.5F
#define LEVEL_3 0x40, 0x00, 0x00, 0x00  //2.0F
#define LEVEL_4 0x40, 0xCB, 0x33, 0x33  //6.35F
#define LEVEL_5 0x40, 0xE0, 0x00, 0x00  //7.00F
#define LEVEL_6 0x40, 0xA0, 0x00, 0x00  //5.00F
#define LEVEL_7 0x3F, 0xA6, 0x66, 0x66  //1.3F
#define LEVEL_8 0x3E, 0x4C, 0xCC, 0xCD  //0.2F

float tmp_f;

/*
Тест 1 проверка U0
*/
void test_1(void){      

       relay_set(TM_142_RELAY_U0, CH_1, STATE_ON);//включить K7 первого канала
       HAL_Delay(DELAY_1);//проверить паузу включения реле
       adc_get_value_f(CH_1, TM_142_ADC_OPENCIRC, &tmp_f);//измерить напряжение
       printf("канал %d %2.3fV\n",CH_1,tmp_f);
       if(tmp_f > 12.075F)//вывести ошибку в случае если U0 > 12.075 ошибка А, если U0 < 10 Б
          { printf("канал %d ошибка А\n",CH_1);}
       else if(tmp_f < 10.000F)
          { printf("канал %d ошибка Б\n",CH_1);}
       else 
          { printf("тест 1 канал %d пройден\n",CH_1);}

       relay_set(TM_142_RELAY_U0, CH_2, STATE_ON);//включить K7 второго канала
       HAL_Delay(DELAY_1);//проверить паузу включения реле
       adc_get_value_f(CH_2, TM_142_ADC_OPENCIRC, &tmp_f);//измерить напряжение
       printf("канал %d %2.3fV\n",CH_2,tmp_f);
       if(tmp_f > 12.075F)//вывести ошибку в случае если U0 > 12.075 ошибка А, если U0 < 10 Б
          { printf("канал %d ошибка А\n",CH_2);}
       else if(tmp_f < 10.000F)
          { printf("канал %d ошибка Б>\n",CH_2);}  
       else 
          { printf("тест 1 канал %d пройден\n",CH_2);}

    return;
}

/*
Тест 2 проверка I0
*/
void test_2(void){
    //подключить аналоговый имитатор датчика (отключить К7 и К6) первый канал
      relay_set(TM_142_RELAY_U0, CH_1, TM_142_U0_DISABLE);// K7
      relay_set(TM_142_RELAY_SENSOR, CH_1, TM_142_SENSOR_ANA);//K6
      dac_set(CH_1,4095);//на ЦАП выставить 4095
      HAL_Delay(DELAY_1);//проверить паузу включения реле
      adc_get_value_f(CH_1, TM_142_ADC_FEEDBACK, &tmp_f);//измерить АЦП ,если I0 < 7,5 mA, то ошибка Г, если I0 > 10 mA то ошибка B
      printf("канал %d %2.3fmA\n",CH_1,tmp_f);
      if(tmp_f < 7.5F)
          { printf("канал %d ошибка Г\n",CH_1);}
      else if(tmp_f > 10.0F)
          { printf("канал %d ошибка В\n",CH_1);}
      else
          { printf("тест 2 канал %d пройден\n",CH_1);}
    //подключить аналоговый имитатор датчика (отключить К7 и К6) второго канала
      relay_set(TM_142_RELAY_U0, CH_2, TM_142_U0_DISABLE);// K7
      relay_set(TM_142_RELAY_SENSOR, CH_2, TM_142_SENSOR_ANA);//K6
      dac_set(CH_2,4095);//на ЦАП выставить 4095
      HAL_Delay(DELAY_1);//проверить паузу включения реле
      adc_get_value_f(CH_2, TM_142_ADC_FEEDBACK, &tmp_f);//измерить АЦП ,если I0 < 7,5 mA, то ошибка Г, если I0 > 10 mA то ошибка B
      printf("канал %d %2.3fmA\n",CH_2,tmp_f);
      if(tmp_f < 7.5F)
          { printf("канал %d ошибка Г\n",CH_2);}
      else if(tmp_f > 10.0F)
          { printf("канал %d ошибка В\n",CH_2);}
      else
          { printf("тест 2 канал %d пройден\n",CH_2);}

    return;
}

/*
Тест 3.1
*/
void test_3_1(void){
    //подключить аналоговый имитатор датчика. (отключить К7 и К6) 
    relay_set(TM_142_RELAY_U0, CH_1, TM_142_U0_DISABLE);// K7
    relay_set(TM_142_RELAY_SENSOR, CH_1, TM_142_SENSOR_ANA);//K6
    //подключить входы барьера для режима «нижний ключ» (реле K2..K5).
    relay_set(TM_142_RELAY_INPUT, CH_1, TM_142_BOT_SW);//K2K4
    relay_set(TM_142_RELAY_ERROR, CH_1, TM_142_BOT_SW);//K3K5
    //Установить режим работа барьера «нижний ключ», без инверсии.
    //увеличиваем ток имитатора датчика от нуля до 0,4 мА 
    dac_set_i(CH_1,0.1F);
    HAL_Delay(DELAY_2);
    state_t cur_state;
    input_read(TM_142_INPUT_INPUT, CH_1, &cur_state);
    printf("канал %d ток 0.4mA вход \"работа\": %d",CH_1,cur_state);
    input_read(TM_142_INPUT_ERROR, CH_1, &cur_state);
    printf("канал %d ток 0.4mA вход \"работа\": %d",CH_1,cur_state);
    return;
}

void test_4_1(void){
    uint16_t data = 0;
    tim_get_delay(CH_1, &data);
    __asm("nop");
    return;
}