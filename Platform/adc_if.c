//--------------------------------------------------
// Интерфейс к АЦП
//--------------------------------------------------
#include "adc_if.h"
#include "app_export.h"
#include "calibration.h"

//#include "KModuleState142.h"

//--------------------------------------------------
// размер буфера для данных АЦП. Учитываются оба
// измерительных канала(open circ and feedback)
#define ADC_BUFF_SIZE 10

//--------------------------------------------------
__attribute__(( section(".dmaSection") ))
uint16_t adc_buff1[ADC_BUFF_SIZE]; // 0-4 index opencirc u0  5-9 index adc_feedback i0
__attribute__(( section(".dmaSection") ))
uint16_t adc_buff2[ADC_BUFF_SIZE]; // 0-4 index opencirc u0  5-9 index adc_feedback i0

//--------------------------------------------------
static uint16_t adc_avg(uint16_t *buff, enum _adc_input in);

//--------------------------------------------------
enum _adc_error adc_get_value(channels_t channel, adc_input_t in, uint16_t *val)/*{{{*/
{
uint16_t value;

if(channel == CH_ALL) { return ADC_CHAN; }

if(channel == CH_1)
    { HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buff1, ADC_BUFF_SIZE); }
else
    { HAL_ADC_Start_DMA(&hadc2, (uint32_t *)adc_buff2, ADC_BUFF_SIZE); }

HAL_Delay(5);
if(channel == CH_1)
    {
    HAL_ADC_Stop_DMA(&hadc1);
    value = adc_avg(adc_buff1, in);
    }
else               
    {
    HAL_ADC_Stop_DMA(&hadc2);
    value = adc_avg(adc_buff2, in);
    }

if(val != 0)
    { *val = value; }

return ADC_OK;
}/*}}}*/

enum _adc_error adc_get_value_f(channels_t channel, adc_input_t in, float *val)
{
uint16_t value;
float value_f;

if(channel == CH_ALL) { return ADC_CHAN; }

if(channel == CH_1)
    { HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_buff1, ADC_BUFF_SIZE); }
else
    { HAL_ADC_Start_DMA(&hadc2, (uint32_t *)adc_buff2, ADC_BUFF_SIZE); }

HAL_Delay(5);
if(channel == CH_1)
    {
    HAL_ADC_Stop_DMA(&hadc1);
    value = adc_avg(adc_buff1, in);

        if(in == TM_142_ADC_FEEDBACK)
           { value_f = calibrate_i0(value, CH_1); }
        else
           { value_f = calibrate_u0(value, CH_1); }
    }
else               
    {
    HAL_ADC_Stop_DMA(&hadc2);
    value = adc_avg(adc_buff2, in);

        if(in == TM_142_ADC_FEEDBACK)
            { value_f = calibrate_i0(value, CH_2); }
        else
            { value_f = calibrate_u0(value, CH_2); }
    }

if(val != 0)
    { *val = value_f; }

return ADC_OK;
}/*}}}*/
//--------------------------------------------------
static uint16_t adc_avg(uint16_t *buff, adc_input_t in)/*{{{*/
{
uint8_t off = 0;
uint32_t avg;

if(in == TM_142_ADC_OPENCIRC)
    { off = ADC_BUFF_SIZE/2; }

avg = 0;
for(uint8_t i=0; i<ADC_BUFF_SIZE/2; i++)
    { avg += buff[off+i]; }

avg <<= 1;
avg = avg/ADC_BUFF_SIZE;
return avg;
}/*}}}*/
//--------------------------------------------------
