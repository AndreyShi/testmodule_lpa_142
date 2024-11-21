#include "display.h"
#include "usbd_cdc_if.h"
#include "render.h"
#include "resources.h"
#include "ssd1306.h"

/* replace the zero with appropriate data pointer */
image_t channels_img =     {  46,  7,  true, 0 };
image_t bg_img =           { 128, 64, false, 0 };
image_t script_stage_img = { 128, 64, false, 0 };
image_t barr_control_img = { 107, 17, false, 0 };
image_t status_img       = {  26, 10,  true, 0 };
image_t hex_dig_img      = {   7, 10,  true, 0 };

static uint8_t disp_usb_com_open;

void display_task(){

    if(disp_usb_com_open != usb_com_open){
       if(usb_com_open)
            {render_image(88, 0, false, &usb_img);}
       else
            {render_box(88, 0, 23, 11, true);}//очистить
       ssd1306_render_now();
       disp_usb_com_open = usb_com_open;
    }
}