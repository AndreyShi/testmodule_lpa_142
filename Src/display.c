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
//----------------------------------------------------------------
//------переменные только для чтения из других модулей------------
extern const uint8_t usb_com_open;
extern const uint8_t lpa_power;
extern const int     ch_gl; 
//----------------------------------------------------------------

void display_task(){
    bool render_now = false;

    if(disp_ch_gl != ch_gl){

        if(ch_gl == 1)
            { bg_img.data = bg_data[BG_ONE]; }
        else if (ch_gl == 2)
            { bg_img.data = bg_data[BG_TWO]; }

        render_image(0, 0, false,0, &bg_img);
        render_now = true;
        disp_ch_gl = ch_gl;//защелка
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