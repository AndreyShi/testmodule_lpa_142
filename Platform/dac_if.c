//--------------------------------------------------
// Интерфейс управления ЦАП-ами
//--------------------------------------------------
#include "dac.h"
#include "dac_if.h"
#include "gpio_if.h"

#include "calibration.h"
//#include "KModuleState.h"

//--------------------------------------------------
void dac_start(void)/*{{{*/
{
HAL_DAC_Start(&hdac, DAC_CHANNEL_1);
HAL_DAC_Start(&hdac, DAC_CHANNEL_2);
}/*}}}*/
//--------------------------------------------------
uint8_t dac_set(enum _channels channel, uint16_t value)/*{{{*/
{
if(value > DAC_MAX)
    { return 1; }

uint32_t val1;
uint32_t val2;

val1 = HAL_DACEx_DualGetValue(&hdac); // hi 16 bit 2 channel,  lo 16 bit 1 channel
val2 = val1 >> 16;
val1 = val1 & 0x0000FFFF;

if(channel == CH_1 || channel == CH_ALL) { val1 = value; }
if(channel == CH_2 || channel == CH_ALL) { val2 = value; }

//update_dac(channel, value, 0.f); // no backwards calibration
HAL_DACEx_DualSetValue(&hdac, DAC_ALIGN_12B_R, val1, val2);
return 0;
}/*}}}*/
//--------------------------------------------------
uint8_t dac_set_i(enum _channels channel, float value)/*{{{*/
{
if(calibrate_dac(value, CH_1) > DAC_MAX ||
   calibrate_dac(value, CH_2) > DAC_MAX)
    { return 1; }

uint32_t val1;
uint32_t val2;

val1 = HAL_DACEx_DualGetValue(&hdac);
val2 = val1 >> 16;
val1 = val1 & 0x0000FFFF;

if(channel == CH_1 || channel == CH_ALL)
    {
    val1 = calibrate_dac(value, CH_1);
    //update_dac(CH_1, val1, value);
    }
if(channel == CH_2 || channel == CH_ALL)
    {
    val2 = calibrate_dac(value, CH_2);
    //update_dac(CH_2, val2, value);
    }

HAL_DACEx_DualSetValue(&hdac, DAC_ALIGN_12B_R, val1, val2);
return 0;
}/*}}}*/
//--------------------------------------------------
