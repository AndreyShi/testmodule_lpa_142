//--------------------------------------------------
// Рисовалка для дисплея. Умеет отображать 1-битовые
// упакованные картинки на буфер кадра и кое-что ещё
//--------------------------------------------------
#include <stdio.h>
#include <string.h>

#include "render.h"
#include "ssd1306.h"

#include "resources.h"

//--------------------------------------------------
static uint8_t get_chunk(const image_t *img, uint8_t x, uint8_t y, uint16_t page_x, uint16_t page_y);
//--------------------------------------------------
void render_cls(void)
{ memset(ssd_frame_buff, 0, 128*8); }
//--------------------------------------------------
void render_dot(uint8_t x, uint8_t y, bool inverted)/*{{{*/
{
uint8_t dot;
uint8_t page;

dot  = 1 << y%8;
page = y/8;

if(inverted)
    { ssd_frame_buff[128*page + x] &= ~dot; }
else
    { ssd_frame_buff[128*page + x] |= dot; }
}/*}}}*/
//--------------------------------------------------
void render_box(uint8_t x, uint8_t y, uint8_t w, uint8_t h, bool inverted)/*{{{*/
{
uint16_t mask;
uint8_t page_s;
uint8_t page_e;

page_s =  y/8;
page_e = (y + h)/8;

for(uint16_t j = page_s; j <= page_e; j++)
    {
    if(page_s == page_e)
        { mask = ~(((1 << h) - 1) << (y % 8)); }
    else if(j == page_s)
        { mask = (1 << y % 8) - 1; }
    else if(j == page_e)
        {
        if((y + h) - 8*j == 0) // non-masked line
            { break; }

        mask = ~( (1 << ((y + h) % 8) ) - 1);
        }
    else
        { mask = 0x00; } // all-blank

    // update frame buffer
    for(uint16_t i = x; i < x + w; i++)
        {
        if(inverted)
            { ssd_frame_buff[128*j + i] = ssd_frame_buff[128*j + i] & mask; }
        else
            { ssd_frame_buff[128*j + i] = ssd_frame_buff[128*j + i] | ~mask; }
        }
    }
}/*}}}*/
//--------------------------------------------------
void render_image(uint8_t x, uint8_t y, bool inverted, const image_t *img)/*{{{*/
{
uint8_t chunk;
uint8_t page_s;
uint8_t page_e;
uint16_t mask;
uint16_t pos;

page_s = y/8;
page_e = (y + img->h)/8;

for(uint16_t j = page_s; j <= page_e; j++)
    {
    if(page_s == page_e)
        { mask = ~(((1 << img->h) - 1) << (y % 8)); }
    else if(j == page_s)
        { mask = (1 << y % 8) - 1; }
    else if(j == page_e)
        { mask = ~( (1 << ((y + img->h) % 8) ) - 1); }
    else
        { mask = 0x00; } // all image bits unmasked

    // update frame buffer
    for(uint16_t i = x; i < x + img->w; i++)
        {
        chunk = get_chunk(img, x, y, i, j);

	pos = 128*j + i;
        ssd_frame_buff[pos] = ssd_frame_buff[pos] & mask;

        // use inverted mask here
        if(inverted)
            { chunk = ~chunk & ~mask; }
        else
            { chunk =  chunk & ~mask; }

	ssd_frame_buff[pos] = ssd_frame_buff[pos] | chunk;
        }
    }
}/*}}}*/
//--------------------------------------------------
void render_text(uint8_t x, uint8_t y, bool inverted, const char *fmt, ...)/*{{{*/
{
char sym;
char tmp[100];

va_list format;
va_start(format, fmt);
vsprintf(tmp, fmt, format);
va_end(format);

const char *cur;
uint8_t cx;

cur = tmp;
cx = x;

while((sym = *cur) != 0 && cur - tmp < 4)
    {
    cur++;

    if(sym >= '0' && sym <= '9')
	{ sym = sym - '0'; }
    else if(sym >= 'A' && sym <= 'F')
	{ sym = sym - 'A' + 10; }
    else if(sym >= 'a' && sym <= 'a')
	{ sym = sym - 'a' + 10; }
    else if(sym == ',' || sym == '.')
	{
	render_image(cx, y+hex_dig_img.h-2, inverted, &comma_img);
	cx += comma_img.w + 2;
	continue;
	}
    else
	{ continue; }

    hex_dig_img.data = hex_dig_data[sym];
    render_image(cx, y, inverted, &hex_dig_img);
    cx += hex_dig_img.w + 1;
    };
}/*}}}*/
//--------------------------------------------------
static uint8_t get_chunk(const image_t *img, uint8_t x, uint8_t y, uint16_t page_x, uint16_t page_y)/*{{{*/
{
uint8_t out;
uint8_t tmp;

out = 0x00;
if(img->vertical)
    {/*{{{*/
    if(y >= 8*page_y) // upper border
        { out = img->data[page_x - x] << (y % 8); }
    else
        {
        out = img->data[img->w*(page_y - y/8 - 1) + (page_x - x)] >> (8 - (y % 8)) |
              img->data[img->w*(page_y - y/8    ) + (page_x - x)] <<      (y % 8);
        }
    }/*}}}*/
else/*{{{*/
    {
    out = 0x00;
    tmp = 0;
    for(uint8_t i = 8*page_y < y ? 0 : 8*page_y - y; i < 8*(page_y+1) - y; i++)
        {
        out >>= 1;

        if(img->w%8 != 0)
	    { tmp = img->data[(img->w/8 + 1)*i + (page_x - x)/8]; }
	else
	    { tmp = img->data[(img->w/8    )*i + (page_x - x)/8]; }

	tmp = tmp << (7 - ((page_x - x) % 8));
	out |= tmp & 0x80;
        }
    }/*}}}*/
return out;
}/*}}}*/
//--------------------------------------------------
