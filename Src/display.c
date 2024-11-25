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

static uint8_t disp_usb_com_open;
static uint8_t disp_lpa_power;

void display_task(){
    bool render_now = false;

    if(disp_usb_com_open != get_usb_com_open()){
       if(get_usb_com_open())
            {render_image(88, 0, false, &usb_img);}
       else
            {render_box(88, 0, usb_img.w, usb_img.h, true);}//очистить
       render_now = true;
       disp_usb_com_open = get_usb_com_open();
    }

    if(disp_lpa_power != get_lpa_power()){
       if(get_lpa_power())
           {render_image(59, 2, false, &power_img);}
       else
           {render_box(59, 2, power_img.w, power_img.h, true);}//очистить
       render_now = true;
       disp_lpa_power = get_lpa_power(); 
    }

    if(render_now){
        ssd1306_render_now();
        render_now = false;
    }
}