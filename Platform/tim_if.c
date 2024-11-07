//--------------------------------------------------
// Интерфейс к Измерителю задержек прохождения
// сигнала через барьер
//--------------------------------------------------
#include "tim.h"
#include "tim_if.h"

//--------------------------------------------------
#define TIM_PERIODS 20 // no less than 5

//--------------------------------------------------
__attribute__(( section(".dmaSection") ))
uint16_t tim_ref[2][TIM_PERIODS];
uint16_t tim[2][TIM_PERIODS];
uint16_t cursor[2];

//--------------------------------------------------
static uint16_t tim_max(uint8_t buf_num);

//--------------------------------------------------
/*
one tick is 1/72000000 = 0.000000014 Sec
*/
enum _tim_error tim_get_delay(enum _channels channel, uint16_t *val)/*{{{*/
{
uint8_t ch;
TIM_HandleTypeDef *gen_tim;
TIM_HandleTypeDef *mes_tim;

if(channel == CH_1)/*{{{*/
    {
    ch = 0;
    gen_tim = &htim9;
    mes_tim = &htim1;
    }
else if(channel == CH_2)
    {
    ch = 1;
    gen_tim = &htim12;
    mes_tim = &htim8;
    }
else
    { return TIM_CHAN; }
/*}}}*/
cursor[ch] = 0; // compensate for interrupt being too slow

HAL_TIM_IC_Start_DMA(mes_tim, TIM_CHANNEL_1, (uint32_t *)tim_ref[ch], TIM_PERIODS);
HAL_TIM_IC_Start_IT(mes_tim, TIM_CHANNEL_2);

TIM_CCxChannelCmd(gen_tim->Instance, TIM_CHANNEL_1, TIM_CCx_ENABLE);
HAL_TIM_OC_Start (gen_tim, TIM_CHANNEL_2);

HAL_Delay(5);

*val = tim_max(ch);
return TIM_OK;
}/*}}}*/
//--------------------------------------------------
static uint16_t tim_max(uint8_t buf_num)/*{{{*/
{
uint32_t max[2];
uint32_t tmp;

max[0] = 0;
max[1] = 0;
for(uint8_t i=2; i<TIM_PERIODS - 2; i++)
    {
    if(tim[buf_num][i] >= tim_ref[buf_num][i])
        {tmp = tim[buf_num][i] - tim_ref[buf_num][i];}
    else
        {tmp = tim_ref[buf_num][i] - tim[buf_num][i];}
        
    if(max[0] < tmp)
	{ max[0] = tmp; }

    if(tim[buf_num][i] >= tim_ref[buf_num][i+1])
        {tmp = tim[buf_num][i] - tim_ref[buf_num][i+1];}
    else
        {tmp = tim_ref[buf_num][i+1] - tim[buf_num][i];}

    if(max[1] < tmp)
	{ max[1] = tmp; }
    }

if(max[0] < max[1])
    { return max[0]; }
else
    { return max[1]; }
}/*}}}*/
//--------------------------------------------------
void delay_tim_isr(TIM_HandleTypeDef *htim)/*{{{*/
{
uint8_t ch;
uint32_t dma_mask;
TIM_HandleTypeDef *gen_tim;

if(htim->Instance == TIM1)
    {
    ch = 0;
    gen_tim = &htim9;
    dma_mask = 0x00000800;
    }
if(htim->Instance == TIM8)
    {
    ch = 1;
    gen_tim = &htim12;
    dma_mask = 0x00800000;
    }

if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)/* DMA done */
    {
    HAL_TIM_OC_Stop(gen_tim, TIM_CHANNEL_1);
    HAL_TIM_OC_Stop(gen_tim, TIM_CHANNEL_2);

    gen_tim->Instance->CCR1 = 0;
    gen_tim->Instance->CCR2 = 0;
    gen_tim->Instance->CNT  = 0;
    gen_tim->Instance->SR   = 0;

    HAL_TIM_IC_Stop_DMA(htim, TIM_CHANNEL_1);
    HAL_TIM_IC_Stop_IT(htim, TIM_CHANNEL_2);

    htim->Instance->CNT  = 0;
    htim->Instance->SR   = 0;

    htim->hdma[TIM_DMA_ID_CC1]->Instance->CR &= 0xFFFFFFFE;

    DMA2->LIFCR |= dma_mask;
    }

if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2)/* go capture */
    {
    if(cursor[ch] < TIM_PERIODS)
	{
	tim[ch][cursor[ch]] = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);
	cursor[ch]++;
	}
    }
}/*}}}*/
//--------------------------------------------------
