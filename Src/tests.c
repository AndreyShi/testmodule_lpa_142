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
#include "modes.h"
#include "boot_uart_if.h"

//--задержки перед измерениями АЦП
#define DELAY_BOOT  1000 //1000ms
#define DELAY_1     500  //500ms
#define DELAY_2     400  //400ms
#define DAC_TIMEOUT 1000 //1s


//---уровни используемые в предыдущем стенде
#define LEVEL_1 0.1F  //0x00, 0x00, 0x00, 0x00  //0.0F
#define LEVEL_2 0.4F  //0x3F, 0x00, 0x00, 0x00  //0.5F
#define LEVEL_3 2.0F  //0x40, 0x00, 0x00, 0x00  //2.0F
#define LEVEL_4 6.35F //0x40, 0xCB, 0x33, 0x33  //6.35F
#define LEVEL_5 7.00F //0x40, 0xE0, 0x00, 0x00  //7.00F
#define LEVEL_6 5.00F //0x40, 0xA0, 0x00, 0x00  //5.00F
#define LEVEL_7 1.3F  //0x3F, 0xA6, 0x66, 0x66  //1.3F
#define LEVEL_8 0.2F  //0x3E, 0x4C, 0xCC, 0xCD  //0.2F

float tmp_f;
uint16_t tmp;
int ms[3][2] = {{CH_1,CH_2},{12,76},{20,80}}; // {{канал}{x координата состояний}{x координата ошибки}}
float levels[8] = {LEVEL_1,LEVEL_2,LEVEL_3,LEVEL_4,LEVEL_5,LEVEL_6,LEVEL_7,LEVEL_8};
int se31[8][2] = {{0,1},{0,0},{1,0},{1,1},{1,1},{1,0},{0,0},{0,1}}; //{in_input,in_error}
int se32[8][2] = {{1,0},{1,1},{0,1},{0,0},{0,0},{0,1},{1,1},{1,0}}; //{in_input,in_error}
int se33[8][2] = {{1,0},{1,1},{0,1},{0,0},{0,0},{0,1},{1,1},{1,0}}; //{in_input,in_error}
int se34[8][2] = {{0,1},{0,0},{1,0},{1,1},{1,1},{1,0},{0,0},{0,1}}; //{in_input,in_error}
char* sr[8][1] = {{"Д\n"},{"Е\n"},{"Е\n"},{"Ж\n"},{"Ж\n"},{"Е\n"},{"Е\n"},{"Д\n"}};
/*
Тест 1 проверка U0
*/
void test_1(int cm){      

    render_box(0, 15, DISPLAY_X_MAX, DISPLAY_Y_MAX - 15, 1 ); //очистка
    render_text(0,0,0,0, "%d", 1);
    ssd1306_render_now();

       for(int c = 0; c < cm; c++){
            relay_set(TM_142_RELAY_U0,     ms[0][c], TM_142_U0_ENABLE); //K7 
            //relay_set(TM_142_RELAY_SENSOR, ms[0][c], TM_142_SENSOR_ANA);//K6 по схеме пропускается
       }
       //-----------------
       HAL_Delay(DELAY_1);//пауза включения реле
       //-----------------
       for(int c = 0; c < cm; c++){
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
void test_2(int cm){

    render_box(0, 15, DISPLAY_X_MAX, DISPLAY_Y_MAX - 15, 1 ); //очистка
    render_text(0,0,0,0, "%d", 2);
    ssd1306_render_now();

    //------подключить аналоговый имитатор датчика (отключить К7 и К6)
    for(int c = 0; c < cm; c++){
      relay_set(TM_142_RELAY_U0,     ms[0][c], TM_142_U0_DISABLE);//K7
      relay_set(TM_142_RELAY_SENSOR, ms[0][c], TM_142_SENSOR_ANA);//K6
    }
    //----------------------------------------------------------------------------
    HAL_Delay(DELAY_1);//пауза включения реле
    //-----------------
    for(int c = 0; c < cm; c++){
      dac_set(ms[0][c],4095);//на ЦАП выставить 4095
      //----------------
      HAL_Delay(DAC_TIMEOUT);
      //----------------
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
автокалибровка ЦАПА
*/
void calibration_dacs(int cm){

    uint16_t dac_p[2][2] = {{0x200,0x600},{0x200,0x600}};  // [канал][{min,max}]
    float adc_p[2][2]    = {{0.0F ,0.0F },{0.0F ,0.0F }};  // [канал][{min,max}]
    int ms[3][2]         = {{CH_1,CH_2},{12,76},{20,80}};  // {{канал}{x координата напряжения}{x координата ошибки}}
    float k, b;

    //---подготовка реле и ЦАПА --
    for(int c = 0; c < cm; c++){
      dac_set(ms[0][c],0);//на ЦАП выставить 0
      relay_set(TM_142_RELAY_U0,     ms[0][c], TM_142_U0_DISABLE);// K7
      relay_set(TM_142_RELAY_SENSOR, ms[0][c], TM_142_SENSOR_ANA);//K6
    }
    //--------------------
    HAL_Delay(DELAY_1);
    //--------------------
    for(int c = 0; c < cm; c++){    // 0 - 1 канал, 1 - 2 канал
      for(int m = 0; m < 2; m++){  // min-0 max-1
        dac_set(ms[0][c],dac_p[c][m]);
        //-----------------
        HAL_Delay(DAC_TIMEOUT);
        //-----------------
        adc_get_value_f(ms[0][c], TM_142_ADC_FEEDBACK, &adc_p[c][m]);
      }
    }

    for(int c = 0; c < cm; c++){    // 0 - 1 канал, 1 - 2 канал
        k = (dac_p[c][1] - dac_p[c][0]) / (adc_p[c][1] - adc_p[c][0]);
        b = dac_p[c][0] - k*adc_p[c][0];
        calibrations_dac[c][0] = k;
        calibrations_dac[c][1] = b;
    }

    for(int c = 0; c < cm; c++){
        printf("цап %d ацп min: %fmA, ацп max: %fmA, k: %f b: %f\n",ms[0][c],adc_p[c][0],adc_p[c][1],calibrations_dac[c][0],calibrations_dac[c][1]);
    }
}

/*
Тест 3.1
нижний ключ
*/
void test_3_1(int cm){
    //----подготовка реле -------------------
    for(int c = 0; c < cm; c++){
        //подключить аналоговый имитатор датчика. (отключить К7 и К6) 
        relay_set(TM_142_RELAY_U0,     ms[0][c], TM_142_U0_DISABLE);// K7
        relay_set(TM_142_RELAY_SENSOR, ms[0][c], TM_142_SENSOR_ANA);//K6
        //подключить входы барьера для режима «нижний ключ» (реле K2..K5).
        relay_set(TM_142_RELAY_INPUT,  ms[0][c], TM_142_BOT_SW);//K2K4
        relay_set(TM_142_RELAY_ERROR,  ms[0][c], TM_142_BOT_SW);//K3K5
    }
    //---------------------------------------
    set_lpa_mode(SENSOR_TYPE_NAMUR | OUTPUT_TYPE_BOT | DIRECT_OUT | DIRECT_ERR);//---здесь установить конфигурацию барьера по UART
    //---------------------------------------
    for(int l = 0; l < sizeof(levels)/sizeof(float); l++){
        for(int c = 0; c < cm; c++){ // c - канал
            dac_set_i(ms[0][c],levels[l]);
            //-----------------
            HAL_Delay(DELAY_2);
            //-----------------
            state_t in_input,in_error;
            input_read(TM_142_INPUT_INPUT, ms[0][c], &in_input);
            input_read(TM_142_INPUT_ERROR, ms[0][c], &in_error);

            printf("3.1 канал %d ток %d %2.3f выход \"работа\": %d, \"ошибка\": %d ",ms[0][c],l,levels[l],in_input,in_error);

            if(in_input == se31[l][0] && in_error == se31[l][1])
                { printf("ок\n");}
            else
                { printf(sr[l][0]);}
        }
    }
    return;
}

/*
Тест 3.2
нижний ключ инверсия
*/
void test_3_2(int cm){
    //----подготовка реле -------------------
    for(int c = 0; c < cm; c++){
        //подключить аналоговый имитатор датчика. (отключить К7 и К6) 
        relay_set(TM_142_RELAY_U0,     ms[0][c], TM_142_U0_DISABLE);// K7
        relay_set(TM_142_RELAY_SENSOR, ms[0][c], TM_142_SENSOR_ANA);//K6
        //подключить входы барьера для режима «нижний ключ» (реле K2..K5).
        relay_set(TM_142_RELAY_INPUT,  ms[0][c], TM_142_BOT_SW);//K2K4
        relay_set(TM_142_RELAY_ERROR,  ms[0][c], TM_142_BOT_SW);//K3K5
    }
    //---------------------------------------
    set_lpa_mode(SENSOR_TYPE_NAMUR | OUTPUT_TYPE_BOT | INVERTED_OUT | INVERTED_ERR);//---здесь установить конфигурацию барьера по UART
    //---------------------------------------
    for(int l = 0; l < sizeof(levels)/sizeof(float); l++){
        for(int c = 0; c < cm; c++){ // c - канал
            dac_set_i(ms[0][c],levels[l]);
            //-----------------
            HAL_Delay(DELAY_2);
            //-----------------
            state_t in_input,in_error;
            input_read(TM_142_INPUT_INPUT, ms[0][c], &in_input);
            input_read(TM_142_INPUT_ERROR, ms[0][c], &in_error);

            printf("3.2 канал %d ток %d %2.3f выход \"работа\": %d, \"ошибка\": %d ",ms[0][c],l,levels[l],in_input,in_error);

            if(in_input == se32[l][0] && in_error == se32[l][1])
                { printf("ок\n");}
            else
                { printf(sr[l][0]);}
        }
    }
    return;
}
/*
тест 3.3 
верхний ключ
*/
void test_3_3(int cm){
    //----подготовка реле -------------------
    for(int c = 0; c < cm; c++){
        //подключить аналоговый имитатор датчика. (отключить К7 и К6) 
        relay_set(TM_142_RELAY_U0,     ms[0][c], TM_142_U0_DISABLE);// K7
        relay_set(TM_142_RELAY_SENSOR, ms[0][c], TM_142_SENSOR_ANA);//K6
        //подключить входы барьера для режима «нижний ключ» (реле K2..K5).
        relay_set(TM_142_RELAY_INPUT,  ms[0][c], TM_142_TOP_SW);//K2K4
        relay_set(TM_142_RELAY_ERROR,  ms[0][c], TM_142_TOP_SW);//K3K5
    }
    //---------------------------------------
    set_lpa_mode(SENSOR_TYPE_NAMUR | OUTPUT_TYPE_TOP | DIRECT_OUT | DIRECT_ERR);//---здесь установить конфигурацию барьера по UART
    //---------------------------------------
    for(int l = 0; l < sizeof(levels)/sizeof(float); l++){
        for(int c = 0; c < cm; c++){ // c - канал
            dac_set_i(ms[0][c],levels[l]);
            //-----------------
            HAL_Delay(DELAY_2);
            //-----------------
            state_t in_input,in_error;
            input_read(TM_142_INPUT_INPUT, ms[0][c], &in_input);
            input_read(TM_142_INPUT_ERROR, ms[0][c], &in_error);

            printf("3.3 канал %d ток %d %2.3f выход \"работа\": %d, \"ошибка\": %d ",ms[0][c],l,levels[l],in_input,in_error);

            if(in_input == se33[l][0] && in_error == se33[l][1])
                { printf("ок\n");}
            else
                { printf(sr[l][0]);}
        }
    }
    return;
}
/*
тест 3.4
верхний ключ инверсия
*/
void test_3_4(int cm){
    //----подготовка реле -------------------
    for(int c = 0; c < cm; c++){
        //подключить аналоговый имитатор датчика. (отключить К7 и К6) 
        relay_set(TM_142_RELAY_U0,     ms[0][c], TM_142_U0_DISABLE);// K7
        relay_set(TM_142_RELAY_SENSOR, ms[0][c], TM_142_SENSOR_ANA);//K6
        //подключить входы барьера для режима «нижний ключ» (реле K2..K5).
        relay_set(TM_142_RELAY_INPUT,  ms[0][c], TM_142_TOP_SW);//K2K4
        relay_set(TM_142_RELAY_ERROR,  ms[0][c], TM_142_TOP_SW);//K3K5
    }
    //---------------------------------------
    set_lpa_mode(SENSOR_TYPE_NAMUR | OUTPUT_TYPE_TOP | INVERTED_OUT | INVERTED_ERR);//---здесь установить конфигурацию барьера по UART
    //---------------------------------------
    for(int l = 0; l < sizeof(levels)/sizeof(float); l++){
        for(int c = 0; c < cm; c++){ // c - канал
            dac_set_i(ms[0][c],levels[l]);
            //-----------------
            HAL_Delay(DELAY_2);
            //-----------------
            state_t in_input,in_error;
            input_read(TM_142_INPUT_INPUT, ms[0][c], &in_input);
            input_read(TM_142_INPUT_ERROR, ms[0][c], &in_error);

            printf("3.4 канал %d ток %d %2.3f выход \"работа\": %d, \"ошибка\": %d ",ms[0][c],l,levels[l],in_input,in_error);

            if(in_input == se34[l][0] && in_error == se34[l][1])
                { printf("ок\n");}
            else
                { printf(sr[l][0]);}
        }
    }
    return;
}

void test_4_1(int cm){
    uint16_t data[2] = {0};
    //----подготовка реле --------------
    for(int c = 0; c < cm; c++){
        //подключить аналоговый имитатор датчика. (отключить К7 и К6) 
        relay_set(TM_142_RELAY_U0,     ms[0][c], TM_142_U0_DISABLE);// K7
        relay_set(TM_142_RELAY_SENSOR, ms[0][c], TM_142_SENSOR_DIG);//K6
        //подключить входы барьера для режима «нижний ключ» (реле K2..K5).
        relay_set(TM_142_RELAY_INPUT,  ms[0][c], TM_142_BOT_SW);//K2K4
        relay_set(TM_142_RELAY_ERROR,  ms[0][c], TM_142_BOT_SW);//K3K5
    }
    //-----------------
    set_lpa_mode(SENSOR_TYPE_NAMUR | OUTPUT_TYPE_BOT | DIRECT_OUT | DIRECT_ERR);
    //-----------------
    HAL_Delay(DELAY_2);
    //-----------------
    for(int c = 0; c < cm; c++)
        { tim_get_delay(ms[0][c], &data[c]);}

    for(int c = 0; c < cm; c++)
    {
        printf("4.1 канал %d, задержка: %d ",ms[0][c], data[c]);
        if(data[c] > 360)
            { printf("Н\n");}
        else
            { printf("ok\n");}
    }
    return;
}

void test_4_2(int cm){
    uint16_t data[2] = {0};
    //----подготовка реле ----------------
    for(int c = 0; c < cm; c++){
        //подключить аналоговый имитатор датчика. (отключить К7 и К6) 
        relay_set(TM_142_RELAY_U0,     ms[0][c], TM_142_U0_DISABLE);// K7
        relay_set(TM_142_RELAY_SENSOR, ms[0][c], TM_142_SENSOR_DIG);//K6
        //подключить входы барьера для режима «нижний ключ» (реле K2..K5).
        relay_set(TM_142_RELAY_INPUT,  ms[0][c], TM_142_TOP_SW);//K2K4
        relay_set(TM_142_RELAY_ERROR,  ms[0][c], TM_142_TOP_SW);//K3K5
    }
    //-----------------
    set_lpa_mode(SENSOR_TYPE_NAMUR | OUTPUT_TYPE_TOP | DIRECT_OUT | DIRECT_ERR);
    //-----------------
    HAL_Delay(DELAY_2);
    //-----------------
    for(int c = 0; c < cm; c++)
        { tim_get_delay(ms[0][c], &data[c]);}
        
    for(int c = 0; c < cm; c++)
    {
        printf("4.2 канал %d, задержка: %d ",ms[0][c], data[c]);
        if(data[c] > 360)
            { printf("Н\n");}
        else
            { printf("ok\n");}
    }
    return;
}