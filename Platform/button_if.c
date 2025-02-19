//--------------------------------------------------
// Автомат подавления дребезга кнопки
//--------------------------------------------------
#include "stm32f4xx_hal.h"
#include "encoder.h"
#include "gpio_if.h"
#include "button_if.h"

#include "app_export.h"
#include "tests.h"
#include "display.h"
#include <stdio.h>
#include "relay_if.h"
#include "dac_if.h"
//--------------------------------------------------
const uint32_t debounce_to = 100;
const uint32_t hold_to = 1000;
volatile int btn_break_is_pending;
//--------------------------------------------------
volatile enum _button_state state_bt;
//----переменная "защелка" для фильтрации переходного состояния кнопки
static enum _button_state old_state_bt;
//--------------------------------------------------------------------
//------переменные только для чтения из других модулей------------
extern const int     ch_gl; 
//----------------------------------------------------------------
/*}}}*/
//--------------------------------------------------
static btn_flags flags[3];
//----текущий контекст кнопки-----------------------
int btn_context;
//--------------------------------------------------
void button_init()/*{{{*/
{
button_state(TM_142_BUTTON_LEFT);
button_state(TM_142_BUTTON_GO);
button_state(TM_142_BUTTON_RIGHT);

if(IS_NOT_PUSHED)
    { state_bt = BTN_RELEASED; }
else
    {
    flags[TM_142_BUTTON_GO - BUTTON_BASE].pushed  = 1;
    flags[TM_142_BUTTON_GO - BUTTON_BASE].clicked = 1;
    flags[TM_142_BUTTON_GO - BUTTON_BASE].hold_1  = 1;
    state_bt = BTN_PUSHED;
    }
}/*}}}*/
//--------------------------------------------------
/*
this function takes 0.000000708 S
*/
void btn_processing()/*{{{*/
{
uint8_t idx;
static uint32_t timer = 0;

// conventional button
idx = TM_142_BUTTON_GO - BUTTON_BASE;
switch(state_bt)/*{{{*/
    {
    case BTN_RELEASED:
	if(IS_NOT_PUSHED)
	    { break; }

	flags[idx].pushed   = 1;
	flags[idx].released = 0;
	timer = HAL_GetTick();
	state_bt = BTN_PUSH_DEBOUNCE;
	break;

    case BTN_PUSH_DEBOUNCE:
	if(HAL_GetTick() < timer + debounce_to)
	    { break; }
	timer = HAL_GetTick();

	if(IS_PUSHED)
	    {
	    flags[idx].clicked = 1;
	    state_bt = BTN_HOLD_1;
	    }
	else
	    {
	    flags[idx].pushed   = 0;
	    flags[idx].released = 1;
	    state_bt = BTN_RELEASE_DEBOUNCE;
	    }
	break;

    case BTN_HOLD_1:
	if(HAL_GetTick() < timer + hold_to)
	    {
	    if(IS_NOT_PUSHED)
		{
		flags[idx].pushed   = 0;
		flags[idx].released = 1;
		timer = HAL_GetTick();
		state_bt = BTN_RELEASE_DEBOUNCE;
		//update_button(STATE_OFF);
		}

	    break;
	    }

	flags[idx].hold_1 = 1;
	timer = HAL_GetTick();
	state_bt = BTN_PUSHED;
	//update_button(STATE_ON);
	break;

    case BTN_PUSHED:
	if(IS_PUSHED)
	    { break; }

	timer = HAL_GetTick();
	state_bt = BTN_RELEASE_DEBOUNCE;
	break;

    case BTN_RELEASE_DEBOUNCE:
	if(HAL_GetTick() < timer + debounce_to)
	    { break; }

	if(IS_NOT_PUSHED)
	    {
	    flags[idx].pushed   = 0;
	    flags[idx].released = 1;
	    state_bt = BTN_RELEASED;
	    //update_button(STATE_OFF);
	    }
	else
	    { state_bt = BTN_PUSHED; }
	break;
    };/*}}}*/

	if(btn_context == c_Testing){
		if(state_bt == BTN_HOLD_1){
			btn_break_is_pending = 1;
		}
	}
}/*}}}*/
#define elif else if
int btn_task(){
	if(old_state_bt != state_bt){
        //printf("button_state: %d\n",state_bt);
        if(state_bt == BTN_HOLD_1){
          //--совершаем дальнейшие инструкции по отпусканию кнопки--
		  while(state_bt != BTN_RELEASED) {;} //в такой конструкции state_bt должен быть с volatile
          //--------------------------------------------------------
          if(btn_context == c_ChooseCh){
			  btn_context = c_Testing;
			  btn_break_is_pending = 0;
              int res = all_test_with_display(ch_gl, break_on);//blocking stream
              //-----все реле в исходное состояние и 24 отключаем от барьера---
			  dac_set(CH_ALL, 0);
			  relay_init();
              //---------------------
			  if(res == lets_dont_change_show)
                  {btn_context = c_Finish;}
			  elif(res == lets_show_vibor_kanalov){
				btn_context = c_ChooseCh;
                show_vibor_kanalov();
			  }
          }elif(btn_context == c_Testing){ //we'll get here while test

          }elif(btn_context == c_Finish){ // finish c_Finish
              btn_context = c_ChooseCh;
              show_vibor_kanalov();
          }
        }
        old_state_bt = state_bt;
    }
	return 0;
}

int btn_is_hold(void){
	int res = 0;
	if(state_bt == BTN_HOLD_1)
	    {res = 1;}
	return res;
}
//--------------------------------------------------
btn_flags button_state(button_id_t btn)/*{{{*/
{
uint8_t idx;

idx = btn - BUTTON_BASE;
btn_flags copy = flags[idx];

if(flags[idx].released == 1)
    {
    flags[idx].pushed = 0;
    flags[idx].clicked = 0;
    flags[idx].hold_1 = 0;
    flags[idx].released = 0;
    }

return copy;
}/*}}}*/
//--------------------------------------------------
void enc_callback(uint8_t dir, uint16_t value)/*{{{*/
{
uint8_t idx;

if(dir == 0x00)
    { idx = TM_142_BUTTON_LEFT - BUTTON_BASE; }
else
    { idx = TM_142_BUTTON_RIGHT - BUTTON_BASE; }

flags[idx].clicked = 1;
flags[idx].released = 1;
}/*}}}*/
//--------------------------------------------------
