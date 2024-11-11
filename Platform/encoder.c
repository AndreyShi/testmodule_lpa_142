//--------------------------------------------------
// Модуль управления энкодером
//--------------------------------------------------
#include "encoder.h"

#include "app_config.h"
#include "tim.h"

//--------------------------------------------------
uint16_t old_value;

volatile uint8_t  enc_idx;
volatile uint16_t enc_values[128];
//--------------------------------------------------
void encoder_init(void)
{
HAL_TIM_Encoder_Start_IT(&enc_tim, TIM_CHANNEL_ALL);
old_value = enc_tim.Instance->CNT;

enc_idx = 0;
}
//--------------------------------------------------
void encoder_reset_position(uint16_t pos)
{
enc_tim.Instance->CNT = pos;
old_value = pos;
}
//--------------------------------------------------
uint16_t encoder_position(void)
{ return enc_tim.Instance->CNT; }
//--------------------------------------------------
void enc_tim_isr()
{
uint16_t value;
uint8_t dir;

value = enc_tim.Instance->CNT;
enc_values[enc_idx] = value;
enc_idx++;
if(enc_idx > 127)
    { enc_idx = 0; }

if(value > old_value)
    {
    if(value == encoder_max && old_value == 0)
	{ dir = 0; }
    else
	{ dir = 1; }
    }
else if(value < old_value)
    {
    if(value == 0 && old_value == encoder_max)
	{ dir = 1; }
    else
	{ dir = 0; }
    }
else
    { return; } // just ignore this

old_value = enc_tim.Instance->CNT;
enc_callback(dir, value);
}
//--------------------------------------------------
__weak void enc_callback(uint8_t dir, uint16_t value)
{ }
//--------------------------------------------------
