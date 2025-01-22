#include "display.h"
#include "usbd_cdc_if.h"
#include "render.h"
#include "resources.h"
#include "ssd1306.h"
#include "relay_if.h"
#include "tests.h"

typedef struct {
    bool has_digit;
    bool has_letter;

    uint8_t digit;
    uint8_t letter;

    uint8_t dig_x;
    uint8_t dig_y;

    uint8_t ltr_x;
    uint8_t ltr_y;
} failed_stage_t;

/* replace the zero with appropriate data pointer */
image_t channels_img =     {  46,  7,  true, 0 };
image_t bg_img =           { 128, 64, false, 0 };
image_t script_stage_img = { 128, 64, false, 0 };
image_t barr_control_img = { 107, 17, false, 0 };
image_t status_img       = {  26, 10,  true, 0 };
image_t hex_dig_img      = {   7, 10,  true, 0 };

/* special indication on 'failed test' screen */
failed_stage_t failed_stages[] = {/*{{{*/
	{ true, false, 0, 0, 111, 17,   0,  0 },//test1
	{ true, false, 1, 0, 110, 17,   0,  0 },//test2

    { true, false, 1, 0, 110, 17,   0,  0 },//calibration_dac - картинки осутствуют, эти параметры как заглушка

	{ true,  true, 2, 0, 103, 17, 116, 17 },//test3a
	{ true,  true, 2, 1, 103, 17, 116, 17 },//test3b
	{ true,  true, 2, 2, 103, 17, 116, 17 },//test3c
	{ true,  true, 2, 3, 103, 17, 116, 17 },//test3d
	{ true,  true, 3, 0, 102, 17, 116, 17 },//test4a
	{ true,  true, 3, 1, 102, 17, 116, 17 },//test4b
};/*}}}*/


//------переменные "защелки" для одиночного вывода на дисплей-----
static uint8_t disp_usb_com_open = 255;
static uint8_t disp_lpa_power    = 255;
static int     disp_ch_gl        = 255;
static uint8_t disp_stages       =   0; //не обновлять при вкл питания
//----------------------------------------------------------------
//------переменные только для чтения из других модулей------------
extern const uint8_t usb_com_open;
extern const uint8_t lpa_power;
extern const int     ch_gl; 
extern const uint8_t stages;
//----------------------------------------------------------------
/*
TO DO лист
25.12.2024 9:42 - рендер bg_img, а потом рендер usb_img + power_img 
создает на картинке артифакты, надо разобраться с render image
09.01.2025 15:48 - снова протестировал рендер usb_img + power_img 
поверх bg_img, артефактов нету, куда исчезли?
22.01.2025 14:11 - при прикосновении пальцем руки к энкодеру ударило статикой 
и появились артефакты
*/
static void display_channels_count(void);
//---------------------------------
//весь вывод на дисплей здесь
//---------------------------------
void display_task(void* some_data){
    bool render_now = false;

    if(disp_stages != stages){
      if(stages == 1){

          script_stage_img.data = script_stage_data[1];
          render_image(0, 0, false,0, &script_stage_img);

      }else if (stages == 2 || stages == 3 || stages == 4 || stages == 5 || stages == 6 || stages == 7 || stages == 8 || stages == 9 || stages == 10){

          script_stage_img.data = script_stage_data[stages];
          render_image(0, 0, false,0, &script_stage_img);
          display_channels_count();/* display channels count */

      }else if(stages == 11 && some_data != 0){ //show errors

            const image_t*   err_img;
            const error_lpa* err_p = (error_lpa*)some_data;
            const uint8_t    tmp = err_p->stage;
            uint8_t          err_ch = 0;
            //---------------------------------
            //выбираем первый по счету канал с ошибкой
            for(int c = 0; c < CH;c++){
                if(err_p->flag[c] == 1){
                        err_ch = c;
                        break;
                    }
                }

            bg_img.data = bg_data[BG_FAILED];
            render_image(0, 0, false,0, &bg_img);
            display_channels_count();/* display channels count */
            /* display error code */
            err_img = &(errors_img[err_p->type_er[err_ch]]);
            render_image(32 - (err_img->w/2), 30, false,0, err_img);
            /* display stage number */
            if(failed_stages[tmp].has_digit)
                { render_image(failed_stages[tmp].dig_x, failed_stages[tmp].dig_y, false,0, &(err_numbers_img[failed_stages[tmp].digit])); }
            if(failed_stages[tmp].has_letter)
                { render_image(failed_stages[tmp].ltr_x, failed_stages[tmp].ltr_y, false,0, &(err_letters_img[failed_stages[tmp].letter])); }
            /* display failed channel number */
            if(err_ch == 0)
                { render_image(111, 44, false,0, &(err_numbers_img[0])); }
            else if(err_ch == 1)
                { render_image(110, 44, false,0, &(err_numbers_img[1])); }

      }else if(stages == 12){ //show success
            render_box(0, 17, 128, success_img.h, true);//очистить
            render_image(45, 17, false,0, &success_img);        
            display_channels_count();/* display channels count */
      }
      
      disp_stages = stages;   //защелка
      disp_usb_com_open = 255;//добавляем поверх usb_img
      disp_lpa_power = 255;   //добавляем поверх power_img
      render_now = true;
    }


    if(disp_ch_gl != ch_gl){

        if(ch_gl == 1)
            { bg_img.data = bg_data[BG_ONE]; }
        else if (ch_gl == 2)
            { bg_img.data = bg_data[BG_TWO]; }

        render_image(0, 0, false,0, &bg_img);
        render_now = true;
        disp_ch_gl = ch_gl;     //защелка
        disp_usb_com_open = 255;//добавляем поверх usb_img
        disp_lpa_power = 255;   //добавляем поверх power_img
    }

    if(disp_usb_com_open != usb_com_open){
       if(usb_com_open)
            {render_image(88, 0, false,0, &usb_img);}
       else
            {render_box(88, 0, usb_img.w, usb_img.h, true);}//очистить
       render_now = true;
       disp_usb_com_open = usb_com_open;//защелка
    }

    if(disp_lpa_power != lpa_power){
       if(lpa_power)
           {render_image(59, 2, false,0, &power_img);}
       else
           {render_box(59, 2, power_img.w, power_img.h, true);}//очистить
       render_now = true;
       disp_lpa_power = lpa_power; //защелка
    }

    if(render_now){
        ssd1306_render_now();
        render_now = false;
    }
}

void display_channels_count(void){
    if(ch_gl == 1)
        { channels_img.data = channels_data[0]; }
    else if(ch_gl == 2)
        { channels_img.data = channels_data[1]; }
    render_image(1, 2, false,0, &channels_img); 
}
/*
обновляет переменную-защелку которая в следующем вызове функции display_task() обновит экран
и покажет выбор каналов
*/
void show_vibor_kanalov(void){
    disp_ch_gl = 255;
}