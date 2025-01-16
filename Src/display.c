#include "display.h"
#include "usbd_cdc_if.h"
#include "render.h"
#include "resources.h"
#include "ssd1306.h"
#include "relay_if.h"

/* replace the zero with appropriate data pointer */
image_t channels_img =     {  46,  7,  true, 0 };
image_t bg_img =           { 128, 64, false, 0 };
image_t script_stage_img = { 128, 64, false, 0 };
image_t barr_control_img = { 107, 17, false, 0 };
image_t status_img       = {  26, 10,  true, 0 };
image_t hex_dig_img      = {   7, 10,  true, 0 };


//------переменные "защелки" для одиночного вывода на дисплей-----
static uint8_t disp_usb_com_open = 255;
static uint8_t disp_lpa_power    = 255;
static int     disp_ch_gl        = 255;
static uint8_t disp_stages       =   0;
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
*/
void display_task(void* some_data){
    bool render_now = false;

    if(disp_stages != stages){
      if(stages == 1){
          script_stage_img.data = script_stage_data[1];
          render_image(0, 0, false,0, &script_stage_img);
      }else if (stages == 2 || stages == 3 || stages == 4 || stages == 5 || stages == 6 || stages == 7 || stages == 8 || stages == 9 || stages == 10){
          script_stage_img.data = script_stage_data[stages];
          render_image(0, 0, false,0, &script_stage_img);
          /* display channels count */
          if(ch_gl == 1)
            { channels_img.data = channels_data[0]; }
          else if(ch_gl == 2)
            { channels_img.data = channels_data[1]; }
          render_image(1, 2, false,0, &channels_img); 
      }else if(stages == 11){ //show errors
#ifdef IFERROR
            bg_img.data = bg_data[BG_FAILED];
            render_image(0, 0, false, &bg_img);
            /* display channels count */
            if(engine_channels() == 1)
                { channels_img.data = channels_data[0]; }
            else
                { channels_img.data = channels_data[1]; }
            render_image(1, 2, false, &channels_img);
            /* display error code */
            err_img = &(errors_img[engine_error_code()]);
            render_image(32 - (err_img->w/2), 30, false, err_img);
            /* display stage number */
            tmp = -1;
            if(menu->cur < 2)
                { /* a no-go */ }
            else if(menu->cur < 4)
                { tmp = menu->cur - 2; }
            else if(menu->cur > 4)
                { tmp = menu->cur - 3; }
            if(failed_stages[tmp].has_digit)
                { render_image(failed_stages[tmp].dig_x, failed_stages[tmp].dig_y, false, &(err_numbers_img[failed_stages[tmp].digit])); }
            if(failed_stages[tmp].has_letter)
                { render_image(failed_stages[tmp].ltr_x, failed_stages[tmp].ltr_y, false, &(err_letters_img[failed_stages[tmp].letter])); }
            /* display failed channel number */
            if(engine_failed_channel() == 1)
                { render_image(111, 44, false, &(err_numbers_img[0])); }
            else
                { render_image(110, 44, false, &(err_numbers_img[1])); }
            break;
#endif
      }else if(stages == 12){ //show succsess

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