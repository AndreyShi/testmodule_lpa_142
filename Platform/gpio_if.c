//--------------------------------------------------
// Модуль управления дискретными сигналами
//--------------------------------------------------
#include "stm32f4xx_hal.h"

#include "app_export.h"
#include "gpio.h"
#include "gpio_if.h"

#include "KModuleState142.h"

// светодиоды
//--------------------------------------------------
#define RED_LED_SET (GPIOC->BSRR = GPIO_BSRR_BS_0)
#define RED_LED_RST (GPIOC->BSRR = GPIO_BSRR_BR_0)
#define RED_LED_GET HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0)

#define BLU_LED_SET (GPIOC->BSRR = GPIO_BSRR_BS_1)
#define BLU_LED_RST (GPIOC->BSRR = GPIO_BSRR_BR_1)
#define BLU_LED_GET HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_1)

#define GRN_LED_SET (GPIOC->BSRR = GPIO_BSRR_BS_2)
#define GRN_LED_RST (GPIOC->BSRR = GPIO_BSRR_BR_2)
#define GRN_LED_GET HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2)

#define YLW_LED_SET (GPIOC->BSRR = GPIO_BSRR_BS_13)
#define YLW_LED_RST (GPIOC->BSRR = GPIO_BSRR_BR_13)
#define YLW_LED_GET HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)

// входы
//--------------------------------------------------
#define IN_CH1_GET HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_11)
#define IN_CH2_GET HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7)

#define ERR_CH1_GET HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_10)
#define ERR_CH2_GET HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8)

// Идентификация модуля, исходно - для калибровок
//--------------------------------------------------
#define ID_1 HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_8)
#define ID_2 HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9)
#define ID_3 HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)
#define ID_4 HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_4)

//--------------------------------------------------
static uint8_t my_id = 0xFF;

//--------------------------------------------------
uint8_t led_set(const led_id_t led, const state_t state)/*{{{*/
{
switch(led)
    {
    case TM_142_LED_RED:
	state == STATE_ON ? RED_LED_SET : RED_LED_RST;
	break;
    case TM_142_LED_BLUE:
	state == STATE_ON ? BLU_LED_SET : BLU_LED_RST;
	break;
    case TM_142_LED_GREEN:
	state == STATE_ON ? GRN_LED_SET : GRN_LED_RST;
	break;
    case TM_142_LED_YELLOW:
	state == STATE_ON ? YLW_LED_SET : YLW_LED_RST;
	break;

    default:
	return 0x01;
    };

return 0x00;
}/*}}}*/
//--------------------------------------------------
uint8_t led_tgl(const led_id_t led)/*{{{*/
{
switch(led)
    {
    case TM_142_LED_RED:
	RED_LED_GET == GPIO_PIN_RESET ? RED_LED_SET : RED_LED_RST;
	break;
    case TM_142_LED_BLUE:
	BLU_LED_GET == GPIO_PIN_RESET ? BLU_LED_SET : BLU_LED_RST;
	break;
    case TM_142_LED_GREEN:
	GRN_LED_GET == GPIO_PIN_RESET ? GRN_LED_SET : GRN_LED_RST;
	break;
    case TM_142_LED_YELLOW:
	YLW_LED_GET == GPIO_PIN_RESET ? YLW_LED_SET : YLW_LED_RST;
	break;

    default:
	return 0x01;
    };

return 0x00;
}/*}}}*/
//--------------------------------------------------
uint8_t input_read(const input_id_t input, const channels_t ch, state_t *state)/*{{{*/
{
if(state == 0)
    { return 0x01; }

switch(input)
    {
    case TM_142_INPUT_INPUT:
	if(ch == CH_1)
	    {
	    if(state != 0)
		{
		if(IN_CH1_GET == GPIO_PIN_SET)
		    { *state = STATE_ON; }
		else
		    { *state = STATE_OFF; }
		}

	    update_input(CH_1, *state);
	    }
	else if(ch == CH_2)
	    {
	    if(state != 0)
		{
		if(IN_CH2_GET == GPIO_PIN_SET)
		    { *state = STATE_ON; }
		else
		    { *state = STATE_OFF; }
		}

	    update_input(CH_2, *state);
	    }
	else
	    { return 0x02; }
	break;

    case TM_142_INPUT_ERROR:
	if(ch == CH_1)
	    {
	    if(state != 0)
		{
		if(ERR_CH1_GET == GPIO_PIN_SET)
		    { *state = STATE_ON; }
		else
		    { *state = STATE_OFF; }
		}

	    update_error(CH_1, *state);
	    }
	else if(ch == CH_2)
	    {
	    if(state != 0)
		{
		if(ERR_CH2_GET == GPIO_PIN_SET)
		    { *state = STATE_ON; }
		else
		    { *state = STATE_OFF; }
		}

	    update_error(CH_2, *state);
	    }
	else
	    { return 0x02; }
	break;

    default:
	return 0x03;
    };

return 0x00;
}/*}}}*/
//--------------------------------------------------
uint8_t id_read(void)/*{{{*/
{
if(my_id != 0xFF)
    { return my_id; }
    
my_id = 0;
if(ID_1 == GPIO_PIN_RESET) // 0-Ohm pulls down
    { my_id |= 1<<0; }
if(ID_2 == GPIO_PIN_RESET)
    { my_id |= 1<<1; }
if(ID_3 == GPIO_PIN_RESET)
    { my_id |= 1<<2; }
if(ID_4 == GPIO_PIN_RESET)
    { my_id |= 1<<3; }

my_id = my_id & 0x0F;
return my_id;
}/*}}}*/
//--------------------------------------------------
