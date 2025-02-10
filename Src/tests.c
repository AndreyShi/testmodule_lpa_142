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
#include "button_if.h"

//--задержки перед измерениями АЦП
#define DELAY_BOOT  1000 //1000ms
#define DELAY_1     500  //500ms
#define DELAY_2     400  //400ms
#define DAC_TIMEOUT 1000 //1s

///старые пороги
//---уровни используемые в предыдущем стенде    
#define LEVEL_1 0.1F  //0x00, 0x00, 0x00, 0x00  //0.0F
#define LEVEL_2 0.4F  //0x3F, 0x00, 0x00, 0x00  //0.5F
#define LEVEL_3 2.0F  //0x40, 0x00, 0x00, 0x00  //2.0F
#define LEVEL_4 6.80F //0x40, 0xCB, 0x33, 0x33  //6.35F + 0.6F
#define LEVEL_5 7.50F //0x40, 0xE0, 0x00, 0x00  //7.00F + 0.5F
#define LEVEL_6 5.00F //0x40, 0xA0, 0x00, 0x00  //5.00F
#define LEVEL_7 1.3F  //0x3F, 0xA6, 0x66, 0x66  //1.3F
#define LEVEL_8 0.2F  //0x3E, 0x4C, 0xCC, 0xCD  //0.2F
//==
//---------------------------------
//таблица соответствий ошибок русского алфавита  и английского
//---------------------------------
#define _a 0  //А
#define _b 1  //Б
#define _c 2  //В
#define _d 3  //Г
#define _e 4  //Д
#define _f 5  //Е
#define _g 6  //Ж
#define _h 7  //З
#define _i 8  //И
#define _k 9  //К
#define _l 10 //Л
#define _m 11 //М
#define _n 12 //Н
#define _o 13 //О
#define _p 14 //П

float tmp_f;
uint16_t tmp;
int ms[3][2] = {{CH_1,CH_2},{12,76},{20,80}}; // {{канал}{x координата состояний}{x координата ошибки}}
//                  обрыв     выкл    вкл     КЗ      КЗ      вкл     выкл    обрыв
float levels[8] = { LEVEL_1, LEVEL_2,LEVEL_3,LEVEL_4,LEVEL_5,LEVEL_6,LEVEL_7,LEVEL_8};
int se3a[8][2] =    {{0,1},  {0,0},  {1,0},  {1,1},  {1,1},  {1,0},  {0,0},  {0,1}}; //{in_input,in_error}
int se3b[8][2] =    {{1,0},  {1,1},  {0,1},  {0,0},  {0,0},  {0,1},  {1,1},  {1,0}}; //{in_input,in_error}
int se3c[8][2] =    {{1,0},  {1,1},  {0,1},  {0,0},  {0,0},  {0,1},  {1,1},  {1,0}}; //{in_input,in_error}
int se3d[8][2] =    {{0,1},  {0,0},  {1,0},  {1,1},  {1,1},  {1,0},  {0,0},  {0,1}}; //{in_input,in_error}
char* sr  [8][1] = {{"Д\n"},{"Е\n"},{"Е\n"},{"Ж\n"},{"Ж\n"},{"Е\n"},{"Е\n"},{"Д\n"}};
uint8_t sr_a [8] = {  _e   ,  _f   ,  _f   ,  _g   ,  _g   ,  _f   ,  _f   ,  _e};
//char* sr_a[8][1] = {{"e\n"},{"f\n"},{"f\n"},{"g\n"},{"g\n"},{"f\n"},{"f\n"},{"e\n"}};

//--------------------
//текущий вывод на экран
//--------------------
uint8_t stages;

void set_error(error_lpa* er_p, uint8_t ch, uint8_t type_er){

    if(ch >= CH)
        {
            printf("\nerror: ch >= CH \n\n");
            return;
        }

    er_p->flag    [ch] = 1;
    er_p->type_er [ch] = type_er;
    return;
}


/*
Тест 1 проверка U0
*/
error_lpa test_1(const int cm, char break_if_error){      

       error_lpa r = {.stage = 0,.flag = {0},.type_er = {0}};

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

            if(tmp_f > 12.075F){//вывести ошибку в случае если U0 > 12.075 ошибка А, если U0 < 10 Б
                    printf("ошибка: А\n");
                    set_error(&r,c,_a);
                    if(break_if_error == 1)
                       {break;}
                }else if(tmp_f < 10.000F){ 
                    printf("ошибка: Б\n");
                    set_error(&r,c,_b);
                    if(break_if_error == 1)
                       {break;}   
                }else { 
                    printf("ок\n");
                }
        }
    return r;
}

/*
Тест 2 проверка I0
*/
error_lpa test_2(const int cm, char break_if_error){

    error_lpa r = {.stage = 1,.flag = {0},.type_er = {0}};

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
      dac_set(ms[0][c],0); // ЦАП 0, (если цап на 4095, то ср кв напряжения в тест 1 0.009, если ЦАП 0 то ср кв 0.002)

      if(tmp_f < 7.000F){ 
            printf("ошибка: Г\n");
            set_error(&r,c,_d);
            if(break_if_error == 1)
               {break;}
        }else if(tmp_f > 10.000F){ 
            printf("ошибка: В\n");
            set_error(&r,c,_c);
            if(break_if_error == 1)
               {break;}
        }else{ 
            printf("ок\n");
        }
    }
    return r;
}

/*
автокалибровка ЦАПА
*/
error_lpa calibration_dacs(const int cm, char break_if_error){

    error_lpa r = {.stage = 2,.flag = {0},.type_er = {0}};

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

    return r;
}

/*
Тест 3.1
нижний ключ
*/
error_lpa test_3a(const int cm, char break_if_error){

    error_lpa r = {.stage = 3,.flag = {0},.type_er = {0}};
    float tmpf = 0.0F;
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

            adc_get_value_f(ms[0][c], TM_142_ADC_FEEDBACK, &tmpf);

            printf("3.1 канал %d ток %d %2.3f real: %2.3f выход \"работа\": %d, \"ошибка\": %d ",ms[0][c],l,levels[l],tmpf,in_input,in_error);

            if(in_input == se3a[l][0] && in_error == se3a[l][1])
                { printf("ок\n");}
            else
                { 
                    printf(sr[l][0]);
                    set_error(&r, c, sr_a[l]);
                    if(break_if_error == 1)
                        {break;}
                }
        }
    }
    return r;
}

/*
Тест 3.2
нижний ключ инверсия
*/
error_lpa test_3b(const int cm, char break_if_error){

    error_lpa r = {.stage = 4,.flag = {0},.type_er = {0}};
    float tmpf = 0.0F;
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

            adc_get_value_f(ms[0][c], TM_142_ADC_FEEDBACK, &tmpf);

            printf("3.2 канал %d ток %d %2.3f real: %2.3f выход \"работа\": %d, \"ошибка\": %d ",ms[0][c],l,levels[l],tmpf,in_input,in_error);

            if(in_input == se3b[l][0] && in_error == se3b[l][1])
                { printf("ок\n");}
            else
                { 
                    printf(sr[l][0]);
                    set_error(&r, c, sr_a[l]);
                    if(break_if_error == 1)
                        {break;}
                }
        }
    }
    return r;
}
/*
тест 3.3 
верхний ключ
*/
error_lpa test_3c(const int cm, char break_if_error){

    error_lpa r = {.stage = 5,.flag = {0},.type_er = {0}};
    float tmpf = 0.0F;
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

            adc_get_value_f(ms[0][c], TM_142_ADC_FEEDBACK, &tmpf);

            printf("3.3 канал %d ток %d %2.3f real: %2.3f выход \"работа\": %d, \"ошибка\": %d ",ms[0][c],l,levels[l],tmpf,in_input,in_error);

            if(in_input == se3c[l][0] && in_error == se3c[l][1])
                { printf("ок\n");}
            else
                { 
                    printf(sr[l][0]);
                    set_error(&r, c, sr_a[l]);
                    if(break_if_error == 1)
                        {break;}
                }
        }
    }
    return r;
}
/*
тест 3.4
верхний ключ инверсия
*/
error_lpa test_3d(const int cm, char break_if_error){

    error_lpa r = {.stage = 6,.flag = {0},.type_er = {0}};
    float tmpf = 0.0F;
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

            adc_get_value_f(ms[0][c], TM_142_ADC_FEEDBACK, &tmpf);

            printf("3.4 канал %d ток %d %2.3f real: %2.3f выход \"работа\": %d, \"ошибка\": %d ",ms[0][c],l,levels[l],tmpf,in_input,in_error);

            if(in_input == se3d[l][0] && in_error == se3d[l][1])
                { printf("ок\n");}
            else
                { 
                    printf(sr[l][0]);
                    set_error(&r, c, sr_a[l]);
                    if(break_if_error == 1)
                        {break;}
                }
        }
    }
    return r;
}

error_lpa test_4a(const int cm, char break_if_error){

    error_lpa r = {.stage = 7,.flag = {0},.type_er = {0}};
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
    //tim_get_delay(ms[0][0], &data[0]); //sometimes we get error 1912 ticks on 1 channel, this test before main tests, avoid problem
    //30.01.25 куда делись ошибка 1912?
    for(int c = 0; c < cm; c++)
        { tim_get_delay(ms[0][c], &data[c]);}

    for(int c = 0; c < cm; c++)
    {
        printf("4.1 канал %d, задержка: %d ",ms[0][c], data[c]);
        if(data[c] > 360)
            { 
                printf("Н\n"); //иногда вылезает ошибка с задержкой 1912, надо разбираться
                set_error(&r, c, _n);
                if(break_if_error == 1)
                    {break;}
            }
        else
            { printf("ok\n");}
    }
    return r;
}

error_lpa test_4b(const int cm, char break_if_error){

    error_lpa r = {.stage = 8,.flag = {0},.type_er = {0}};
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
    //tim_get_delay(ms[0][0], &data[0]); //sometimes we get error 1912 ticks on 1 channel, this test before main tests, avoid problem
    //30.01.25 куда делись ошибка 1912?
    for(int c = 0; c < cm; c++)
        { tim_get_delay(ms[0][c], &data[c]);}
        
    for(int c = 0; c < cm; c++)
    {
        printf("4.2 канал %d, задержка: %d ",ms[0][c], data[c]);
        if(data[c] > 360)
            { 
                printf("П\n"); //иногда вылезает ошибка с задержкой 1912, надо разбираться
                set_error(&r, c, _p);
                if(break_if_error == 1)
                    {break;}
            }
        else
            { printf("ok\n");}
    }
    return r;
}
/*
info: прошивка + все тесты в цикле, вывод только в консоль
const int cm - кол-во каналов 1 - 1 канал, 2- 2 канала
char break_if_error - 0 - не останавливаемся по ошибке, 1 - остановка по ошибке включена
*/
void all_test(const int cm, char break_if_error){

    error_lpa (*cur_test[])(const int,char) = {test_1, test_2, calibration_dacs, test_3a, test_3b, test_3c, test_3d, test_4a, test_4b};
    const int l = sizeof(cur_test)/sizeof(cur_test[0]);
    error_lpa er[9] = {0};

    __asm("nop");
    //-----fw update----
    if(boot_update() == 0){
        printf("\nboot_update() вернул FALSE!\n\n");
        return;
    }
    //------------------
    if(break_if_error == 1)
        { printf("\nрежим \"break_if_error\" включен!\n\n");}

    for(int op = 0; op < l; op++){
        er[op] = cur_test[op](cm,break_if_error);
        if(break_if_error == 1){
            if(er[op].flag[ch0] == 1 || er[op].flag[ch1] == 1){
                    printf("\nошибка в тесте, стоп!\n\n");
                    break;
            }
        }
        printf("\n");
    }

    printf("\nтест завершен!\n");

    //----relay init---
    relay_init();
    //-----------------
    return;
}

int all_test_with_display(const int cm, char break_if_error){

    error_lpa (*cur_test[])(const int,char) = {test_1, test_2, calibration_dacs, test_3a, test_3b, test_3c, test_3d, test_4a, test_4b};
    const int l = sizeof(cur_test)/sizeof(cur_test[0]);
    error_lpa er[9] = {0};
    char flag = 0;

    stages = 1;
    display_task(0);
    if(btn_break_is_pending == 1)
        { return 1;}
    //-----fw update----
    boot_update(); 
    //------------------

    for(int op = 0; op < l; op++){
        stages = 2 + op;
        display_task(0);
        if(btn_break_is_pending == 1)
            { return 1;}
        er[op] = cur_test[op](cm,break_if_error);

        if(break_if_error == 1){ 
            if(er[op].flag[ch0] == 1 || er[op].flag[ch1] == 1)
                { break;}        //break
        }
    }
    //вывод первого по счету теста с ошибки
    for(int op = 0; op < l; op++){
        if(er[op].flag[ch0] == 1 || er[op].flag[ch1] == 1){
            stages = 11;
            display_task((void*)&er[op]);
            flag = 1;
            break;
        }
    }

    //если ошибок нет, вывод "галочки"
    if(flag == 0){
        stages = 12;
        display_task(0);
    }
    return 0;
}

void diagnostics(const int cm){

        float tmp_f = 0.0F;
        char* p_prev = "\0";
        char buf_prv[150] = {0};
        char* p_M[4] = {"обрыв","выкл ","вкл  ","КЗ   ","\nстоп\n"};
        int perexod = 0;
        //начальная инициализация "обрыв"
        state_t prev_in_input = 0;
        state_t prev_in_error = 1;

        for(int i = 0; i < (int)(7.100F/0.1F); i++){

            //от 3 мА до 5 мА пропускаем для ускорения процесса
            if(i >= 25 && i <= 55){continue;}

            dac_set_i(cm, i * 0.1F);
            //-----------------
            HAL_Delay(400); // для старта нужна задержка побольше
            //-----------------
            state_t in_input,in_error;
            input_read(TM_142_INPUT_INPUT, cm, &in_input);
            input_read(TM_142_INPUT_ERROR, cm, &in_error);

            char* p;
            if(in_input == 0 && in_error == 1)
                {p = "обрыв";}
            else if(in_input == 0 && in_error == 0)
                {p = "выкл";}
            else if(in_input == 1 && in_error == 0)
                {p = "вкл";}
            else if(in_input == 1 && in_error == 1)
                {p = "КЗ";}
            else
                {p = "несоотвествие выходов";}

            adc_get_value_f(cm, TM_142_ADC_FEEDBACK, &tmp_f);

            if(0) // выводим каждый шаг
                { printf("set↑: %2.1f, real: %2.3f, вых: %d, ош %d \n",i * 0.1F, tmp_f, in_input, in_error);}//выводим текущее
            else if((prev_in_input == in_input && prev_in_error == in_error) || i == 0){
               snprintf(buf_prv,150,"set↑: %2.1f, real: %2.3f, вых: %d, ош %d \n",i * 0.1F, tmp_f, in_input, in_error);
               p_prev = p;
               prev_in_input = in_input;
               prev_in_error = in_error;
               if(i == 0)
                  { printf("%s %s",p_M[perexod],buf_prv);}
               //printf("*");
            }else{ //обнаружен переход
               printf("%s %s",p_M[perexod++],buf_prv); //выводим предыдущие
               printf("%s set↑: %2.1f, real: %2.3f, вых: %d, ош %d \n",p_M[perexod], i * 0.1F, tmp_f, in_input, in_error);//выводим текущее
               p_prev = p;
               prev_in_input = in_input;
               prev_in_error = in_error;
            }
        }
        
        printf("\n");
        perexod = 2;// "вкл"
        //начальная инициализация "вкл"
        prev_in_input = 1;
        prev_in_error = 0;
        for(int i = 20; i > 9; i--){
            //от 3 мА до 5 мА пропускаем для ускорения процесса
            if(i >= 25 && i <= 55){continue;}

            dac_set_i(cm, i * 0.1F);
            //-----------------
            HAL_Delay(400);
            //-----------------
            state_t in_input,in_error;
            input_read(TM_142_INPUT_INPUT, cm, &in_input);
            input_read(TM_142_INPUT_ERROR, cm, &in_error);
            char* p = "\0";
            if(in_input == 0 && in_error == 1)
                {p = "обрыв";}
            else if(in_input == 0 && in_error == 0)
                {p = "выкл";}
            else if(in_input == 1 && in_error == 0)
                {p = "вкл";}
            else if(in_input == 1 && in_error == 1)
                {p = "КЗ";}
            else
                {p = "несоотвествие выходов";}

            adc_get_value_f(cm, TM_142_ADC_FEEDBACK, &tmp_f);
            if(0) // выводим каждый шаг
                { printf("set↓: %2.1f, real: %2.3f, вых: %d, ош %d \n",i * 0.1F, tmp_f, in_input, in_error);}//выводим текущее
            else if((prev_in_input == in_input && prev_in_error == in_error)){
               snprintf(buf_prv,150,"set↓: %2.1f, real: %2.3f, вых: %d, ош %d \n",i * 0.1F, tmp_f, in_input, in_error);
               p_prev = p;
               prev_in_input = in_input;
               prev_in_error = in_error;
               if(i == 0)
                  { printf("%s %s",p_M[perexod],buf_prv);}
            }else{ //обнаружен переход
               //if(strcmp(p, "выкл") == 0){
                   printf("%s %s",p_M[perexod--],buf_prv); //выводим предыдущие
                   printf("%s set↓: %2.1f, real: %2.3f, вых: %d, ош %d \n",p_M[perexod], i * 0.1F, tmp_f, in_input, in_error);//выводим текущее
              // }
               p_prev = p;
               prev_in_input = in_input;
               prev_in_error = in_error;
            }
        }

}