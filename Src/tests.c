#include "tests.h"
#include "main.h"
#include "relay_if.h"
#include "adc_if.h"
#include "dac_if.h"
#include "app_export.h"
#include "gpio_if.h"
#include "tim_if.h"
#include "render.h"
#include "resources.h"
#include "ssd1306.h"
#include <stdio.h>
#include "display.h"
#include "calibration.h"

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
uint16_t tmp;
/*
Тест 1 проверка U0
*/
void test_1(void){      

    render_box(0, 15, DISPLAY_X_MAX, DISPLAY_Y_MAX - 15, 1 ); //очистка
    render_text(0,0,0,0, "%d", 1);
    ssd1306_render_now();

    int ms[3][2] = {{CH_1,CH_2},{12,76},{20,80}}; // {{канал}{x координата напряжения}{x координата ошибки}}

       for(int c = 0; c < 2; c++){
            relay_set(TM_142_RELAY_U0, ms[0][c], STATE_ON);//включить K7 первого канала
            HAL_Delay(DELAY_1);//пауза включения реле
            adc_get_value_f(ms[0][c], TM_142_ADC_OPENCIRC, &tmp_f);//измерить напряжение
            printf("тест 1, канал %d: %2.3fV, ",ms[0][c],tmp_f);
            render_text(ms[1][c],15,0,0, "%2.3f", tmp_f);

            if(tmp_f > 12.075F){//вывести ошибку в случае если U0 > 12.075 ошибка А, если U0 < 10 Б
                    printf("ошибка: А\n");
                    render_image(ms[2][c],30,0,0, &errors_img[0]); 
                }else if(tmp_f < 10.000F){ 
                    printf("ошибка: Б\n");
                    render_image(ms[2][c],30,0,0, &errors_img[1]);      
                }else { 
                    printf("ок\n");
                    render_image(ms[2][c],25,0,0, &success_img);
                }
        }

    ssd1306_render_now();
    return;
}

/*
Тест 2 проверка I0
*/
void test_2(void){

    render_box(0, 15, DISPLAY_X_MAX, DISPLAY_Y_MAX - 15, 1 ); //очистка
    render_text(0,0,0,0, "%d", 2);
    ssd1306_render_now();

    int ms[3][2] = {{CH_1,CH_2},{12,76},{20,80}}; // {{канал}{x координата напряжения}{x координата ошибки}}

    //подключить аналоговый имитатор датчика (отключить К7 и К6) первый канал
    for(int c = 0; c < 2; c++){
      relay_set(TM_142_RELAY_U0, ms[0][c], TM_142_U0_DISABLE);// K7
      relay_set(TM_142_RELAY_SENSOR, ms[0][c], TM_142_SENSOR_ANA);//K6
      dac_set(ms[0][c],4095);//на ЦАП выставить 4095
      HAL_Delay(DELAY_1);//пауза включения реле
      adc_get_value_f(ms[0][c], TM_142_ADC_FEEDBACK, &tmp_f);//измерить АЦП ,если I0 < 7,5 mA, то ошибка Г, если I0 > 10 mA то ошибка B
      printf("тест 2, канал %d: %2.3fmA, ",ms[0][c],tmp_f);
      render_text(ms[1][c],15,0,0, "%2.3f", tmp_f);
      dac_set(ms[0][c],0); // ЦАП 0, (если цап на 4095, то ср кв напряжения в тест 1 0.009, если ЦАП 0 то ср кв 0.002)

      if(tmp_f < 7.5F){ 
            printf("ошибка: Г\n");
            render_image(ms[2][c],30,0,0, &errors_img[3]); 
        }else if(tmp_f > 10.0F){ 
            printf("ошибка: В\n");
            render_image(ms[2][c],30,0,0, &errors_img[2]); 
        }else{ 
            printf("ок\n");
            render_image(ms[2][c],25,0,0, &success_img);
        }
    }

    ssd1306_render_now();
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