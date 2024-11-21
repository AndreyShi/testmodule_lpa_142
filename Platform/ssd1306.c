//--------------------------------------------------
// Модуль управления диспелеем на чипе ssd1306
//--------------------------------------------------
#include <stdbool.h>

#include "ssd1306.h"

#include "spi.h"

#include "app_config.h"
#include "gpio_if.h"

#include "resources.h"

//--------------------------------------------------
const uint32_t SSD_MSPF = 40; // ms per frame
//--------------------------------------------------
uint8_t ssd_frame_buff[128*8];
static volatile bool ssd_busy;
static uint8_t ssd_buff[7];
//--------------------------------------------------
static void ssd_set_contrast(uint8_t c);
static void ssd_set_blank(bool blank);
static void ssd_set_invertion(bool inverse);
static void ssd_set_disp(bool on);
static void ssd_cont_horiz_scroll_setup(bool right, uint8_t start_page, uint8_t pace, uint8_t end_page);
static void ssd_cont_vert_and_horiz_scroll_setup(bool right, uint8_t start_page, uint8_t pace, uint8_t end_page, uint8_t offset);
static void ssd_deactivate_scroll(void);
static void ssd_activate_scroll(void);
static void ssd_set_vert_scroll_area(uint8_t offset, uint8_t size);
static void ssd_set_low_col_start_addr(uint8_t col);
static void ssd_set_high_col_start_addr(uint8_t col);
static void ssd_set_addr_mode(uint8_t mode);
static void ssd_set_col_start_end_addr(uint8_t start_addr, uint8_t end_addr);
static void ssd_set_page_start_end_addr(uint8_t start_addr, uint8_t end_addr);
static void ssd_set_page_start_addr(uint8_t start_addr);
static void ssd_set_start_line(uint8_t line);
static void ssd_set_segment_remap(bool remap);
static void ssd_set_mux(uint8_t mux);
static void ssd_set_scan(bool normal);
static void ssd_set_offset(uint8_t offset);
static void ssd_set_com_pins_conf(uint8_t conf);
static void ssd_set_osc_freq_div_ratio(uint8_t freq, uint8_t div);
static void ssd_set_precharge(uint8_t phase1, uint8_t phase2);
static void ssd_set_vcom_deselect_level(uint8_t val);
static void ssd_enable_pump(bool enable);
//--------------------------------------------------
void ssd1306_init(void)/*{{{*/
{
ssd_busy = true; // just in case

HAL_Delay(2);
SSD_UNRESET;

HAL_Delay(2);
ssd_set_disp(false);

ssd_set_mux(0x3F);

ssd_set_addr_mode(0x00);
ssd_set_low_col_start_addr(0x00);
ssd_set_high_col_start_addr(0x00);
ssd_set_col_start_end_addr(0x00, 0x7F);
ssd_set_page_start_end_addr(0x00, 0x07);
ssd_set_offset(0x00);
ssd_set_start_line(0x00);
ssd_set_segment_remap(false);
ssd_set_scan(true);
ssd_set_com_pins_conf(0x12);
ssd_set_contrast(0x7F);
ssd_set_osc_freq_div_ratio(0x80, 0x00);
ssd_set_precharge(0x01, 0xF0);
ssd_set_vcom_deselect_level(0x40);
ssd_enable_pump(true);
ssd_set_blank(false);
ssd_set_invertion(false);
ssd_set_disp(true);
// blank display memory
for(uint8_t i=0; i<8; i++)
    {
    for(uint8_t j=0; j<128; j++)
	{ ssd_frame_buff[128*i + j] = 0x00; }
    }
SSD_DATA;
HAL_SPI_Transmit(&ssd_spi, ssd_frame_buff, 128*8, 1); 

ssd_busy = false;
}/*}}}*/
//--------------------------------------------------
void ssd1306_task(void)/*{{{*/
{
static uint32_t clock = 0;

if(HAL_GetTick() - clock < SSD_MSPF)
    { return; }

if(ssd_busy)
    { return; }

clock = HAL_GetTick();
ssd_busy = true;

SSD_DATA;
HAL_SPI_Transmit_DMA(&ssd_spi, ssd_frame_buff, 128*8);
}/*}}}*/
//--------------------------------------------------
bool ssd1306_render_now(void)/*{{{*/
{
if(ssd_busy)
    { return false; }

ssd_busy = true;

SSD_DATA;
HAL_SPI_Transmit(&ssd_spi, ssd_frame_buff, 128*8, 500);
ssd_busy = false;
return true;
}/*}}}*/
//--------------------------------------------------
static void ssd_set_contrast(uint8_t c)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0x81;
ssd_buff[1] = c;
HAL_SPI_Transmit(&ssd_spi, ssd_buff, 2, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_blank(bool blank)/*{{{*/
{
SSD_COMMAND;
if(blank)
    { ssd_buff[0] = 0xA5; }
else
    { ssd_buff[0] = 0xA4; }

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 1, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_invertion(bool inverse)/*{{{*/
{
SSD_COMMAND;
if(inverse)
    { ssd_buff[0] = 0xA7; }
else
    { ssd_buff[0] = 0xA6; }

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 1, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_disp(bool on)/*{{{*/
{
SSD_COMMAND;
if(on)
    { ssd_buff[0] = 0xAF; }
else
    { ssd_buff[0] = 0xAE; }
volatile HAL_StatusTypeDef st;
st = HAL_SPI_Transmit(&ssd_spi, ssd_buff, 1, 10);
__asm("nop");
}/*}}}*/
//--------------------------------------------------
static void ssd_cont_horiz_scroll_setup(bool right, uint8_t start_page, uint8_t pace, uint8_t end_page)/*{{{*/
{
SSD_COMMAND;
if(right)
    { ssd_buff[0] = 0x26; }
else
    { ssd_buff[0] = 0x27; }

ssd_buff[1] = 0x00;
ssd_buff[2] = start_page & 0x07;
ssd_buff[3] = pace & 0x07;
ssd_buff[4] = end_page & 0x07;
ssd_buff[5] = 0x00;
ssd_buff[6] = 0xFF;

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 7, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_cont_vert_and_horiz_scroll_setup(bool right, uint8_t start_page, uint8_t pace, uint8_t end_page, uint8_t offset)/*{{{*/
{
SSD_COMMAND;
if(right)
    { ssd_buff[0] = 0x29; }
else
    { ssd_buff[0] = 0x2A; }

ssd_buff[1] = 0x00;
ssd_buff[2] = start_page & 0x07;
ssd_buff[3] = pace & 0x07;
ssd_buff[4] = end_page & 0x07;
ssd_buff[5] = offset & 0x3F;

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 6, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_deactivate_scroll(void)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0x2E;

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 1, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_activate_scroll(void)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0x2F;

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 1, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_vert_scroll_area(uint8_t offset, uint8_t size)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0xA3;
ssd_buff[1] = offset & 0x3F;
ssd_buff[2] = size   & 0x3F;

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 3, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_low_col_start_addr(uint8_t col)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = col & 0x0F;

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 1, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_high_col_start_addr(uint8_t col)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0x10 | (col & 0x0F);

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 1, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_addr_mode(uint8_t mode)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0x20;
ssd_buff[1] = mode & 0x03;

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 2, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_col_start_end_addr(uint8_t start_addr, uint8_t end_addr)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0x21;
ssd_buff[1] = start_addr & 0x7F;
ssd_buff[2] = end_addr   & 0x7F;

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 3, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_page_start_end_addr(uint8_t start_addr, uint8_t end_addr)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0x22;
ssd_buff[1] = start_addr & 0x07;
ssd_buff[2] = end_addr   & 0x07;

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 3, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_page_start_addr(uint8_t start_addr)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0xB0 | (start_addr & 0x07);

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 1, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_start_line(uint8_t line)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0x40 | (line & 0x3F);

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 1, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_segment_remap(bool remap)/*{{{*/
{
SSD_COMMAND;
if(remap)
    { ssd_buff[0] = 0xA1; }
else
    { ssd_buff[0] = 0xA0; }

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 1, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_mux(uint8_t mux)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0xA8;
ssd_buff[1] = mux & 0x3F;

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 2, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_scan(bool normal)/*{{{*/
{
SSD_COMMAND;
if(normal)
    { ssd_buff[0] = 0xC0; }
else
    { ssd_buff[0] = 0xC8; }

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 1, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_offset(uint8_t offset)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0xD3;
ssd_buff[1] = offset & 0x3F;

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 2, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_com_pins_conf(uint8_t conf)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0xDA;
ssd_buff[1] = (conf & 0x30 ) | 0x02;

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 2, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_osc_freq_div_ratio(uint8_t freq, uint8_t div)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0xD5;
ssd_buff[1] = (freq & 0xF0) | (div & 0x0F);

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 2, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_precharge(uint8_t phase1, uint8_t phase2)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0xD9;
ssd_buff[1] = (phase1 & 0x0F) | (phase2 & 0xF0);

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 2, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_set_vcom_deselect_level(uint8_t val)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0xDB;
ssd_buff[1] = val & 0x70;

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 2, 10);
}/*}}}*/
//--------------------------------------------------
static void ssd_enable_pump(bool enable)/*{{{*/
{
SSD_COMMAND;
ssd_buff[0] = 0x8D;
if(enable)
    { ssd_buff[1] = 0x14; }
else
    { ssd_buff[1] = 0x10; }

HAL_SPI_Transmit(&ssd_spi, ssd_buff, 2, 10);
}/*}}}*/
//--------------------------------------------------
void ssd_spi_isr()
{ ssd_busy = false; }
//--------------------------------------------------
