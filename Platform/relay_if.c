//--------------------------------------------------
// Модуль управления дискретными выходами
//--------------------------------------------------
#include "stm32f4xx_hal.h"

#include "app_config.h"
#include "gpio_if.h"
#include "relay_if.h"

//#include "KModuleState142.h"

//--------------------------------------------------
#define SET_U0_CH1 GPIOC->BSRR = GPIO_BSRR_BS_5;//измерение напряжения
#define RST_U0_CH1 GPIOC->BSRR = GPIO_BSRR_BR_5;//дискретный или аналоговый имитатор

#define SET_U0_CH2 GPIOD->BSRR = GPIO_BSRR_BS_1;//измерение напряжения
#define RST_U0_CH2 GPIOD->BSRR = GPIO_BSRR_BR_1;//дискретный или аналоговый имитатор

#define DIG_CH1 GPIOC->BSRR = GPIO_BSRR_BS_4; //дискретный имитатор датчика
#define ANA_CH1 GPIOC->BSRR = GPIO_BSRR_BR_4; //аналоговый имитатор датчика

#define DIG_CH2 GPIOD->BSRR = GPIO_BSRR_BS_0; //дискретный имитатор датчика
#define ANA_CH2 GPIOD->BSRR = GPIO_BSRR_BR_0; //аналоговый имитатор датчика

#define IN_TOP_SW_CH1 GPIOE->BSRR = GPIO_BSRR_BS_12 | GPIO_BSRR_BS_13; // "верхний ключ"
#define IN_BOT_SW_CH1 GPIOE->BSRR = GPIO_BSRR_BR_12 | GPIO_BSRR_BR_13; // "нижний ключ"

#define IN_TOP_SW_CH2 GPIOD->BSRR = GPIO_BSRR_BS_12 | GPIO_BSRR_BS_13; // "верхний ключ"
#define IN_BOT_SW_CH2 GPIOD->BSRR = GPIO_BSRR_BR_12 | GPIO_BSRR_BR_13; // "нижний ключ"

#define ERR_TOP_SW_CH1 GPIOE->BSRR = GPIO_BSRR_BS_14 | GPIO_BSRR_BS_15;// "верхний ключ"
#define ERR_BOT_SW_CH1 GPIOE->BSRR = GPIO_BSRR_BR_14 | GPIO_BSRR_BR_15;// "нижний ключ"

#define ERR_TOP_SW_CH2 GPIOD->BSRR = GPIO_BSRR_BS_14 | GPIO_BSRR_BS_15;// "верхний ключ"
#define ERR_BOT_SW_CH2 GPIOD->BSRR = GPIO_BSRR_BR_14 | GPIO_BSRR_BR_15;// "нижний ключ"

#define PWR_ENABLE  GPIOD->BSRR = GPIO_BSRR_BR_2;
#define PWR_DISABLE GPIOD->BSRR = GPIO_BSRR_BS_2;

#define GND_ENABLE  GPIOD->BSRR = GPIO_BSRR_BS_8;
#define GND_DISABLE GPIOD->BSRR = GPIO_BSRR_BR_8;

//--------------------------------------------------
static uint32_t power_phase;
static uint8_t lpa_power;
//--------------------------------------------------
void relay_init(void)/*{{{*/
{
RST_U0_CH1;
RST_U0_CH2;

ANA_CH1;
ANA_CH2;

IN_BOT_SW_CH1;
IN_BOT_SW_CH2;

ERR_BOT_SW_CH1;
ERR_BOT_SW_CH2;

PWR_DISABLE;
GND_DISABLE;

power_phase = 0;
}/*}}}*/
//--------------------------------------------------
uint8_t relay_set(const relay_id_t relay, const channels_t ch, const state_t state)/*{{{*/
{
switch(relay)
    {
    case TM_142_RELAY_U0:
	if(ch == CH_1)
	    {
	    if(state == STATE_ON)
		{ SET_U0_CH1; }
	    else
		{ RST_U0_CH1; }

	    //update_relay_u0(CH_1, state);
	    }
	else if(ch == CH_2)
	    {
	    if(state == STATE_ON)
		{ SET_U0_CH2; }
	    else
		{ RST_U0_CH2; }

	    //update_relay_u0(CH_2, state);
	    }
	else
	    { return 0x02; }
	break;

    case TM_142_RELAY_SENSOR:
	if(ch == CH_1)
	    {
	    if(state == TM_142_SENSOR_DIG)
		{ DIG_CH1; }
	    else
		{ ANA_CH1; }
	    
	    //update_sensor_type(CH_1, state);
	    }
	else if(ch == CH_2)
	    {
	    if(state == TM_142_SENSOR_DIG)
		{ DIG_CH2; }
	    else
		{ ANA_CH2; }

	    //update_sensor_type(CH_2, state);
	    }
	else
	    { return 0x02; }
	break;

    case TM_142_RELAY_INPUT:
	if(ch == CH_1)
	    {
	    if(state == TM_142_TOP_SW)
		{ IN_TOP_SW_CH1; }
	    else
		{ IN_BOT_SW_CH1; }

	    //update_input_type(CH_1, state);
	    }
	else if(ch == CH_2)
	    {
	    if(state == TM_142_TOP_SW)
		{ IN_TOP_SW_CH2; }
	    else
		{ IN_BOT_SW_CH2; }

	    //update_input_type(CH_2, state);
	    }
	else
	    { return 0x02; }
	break;

    case TM_142_RELAY_ERROR:
	if(ch == CH_1)
	    {
	    if(state == TM_142_TOP_SW)
		{ ERR_TOP_SW_CH1; }
	    else
		{ ERR_BOT_SW_CH1; }

	    //update_error_type(CH_1, state);
	    }
	else if(ch == CH_2)
	    {
	    if(state == TM_142_TOP_SW)
		{ ERR_TOP_SW_CH2; }
	    else
		{ ERR_BOT_SW_CH2; }

	    //update_error_type(CH_2, state);
	    }
	else
	    { return 0x02; }
	break;

    case TM_142_RELAY_POWER:
	if(state == STATE_ON)
	    {
	    if(power_phase == 0)
		{
		GND_ENABLE;
		power_phase = HAL_GetTick();
		return 0; // not done yet
		}
	    else if(HAL_GetTick() - power_phase >= 100)
		{
		PWR_ENABLE;
		power_phase = 0;
		lpa_power = 1;
		} // done for now
	    else
		{ return 0; }
	    }
	else
	    {
	    PWR_DISABLE;
	    GND_DISABLE;
		lpa_power = 0;
	    }

	//update_lpa_power(state);
	break;

    default:
	return 0x01;
    };

return 255;
}/*}}}*/

uint8_t get_lpa_power(void)
{return lpa_power;}
//--------------------------------------------------
