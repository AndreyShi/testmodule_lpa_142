//--------------------------------------------------
// Калибровочные значения для стенда
//--------------------------------------------------
#include "app_config.h"
#include "calibration.h"
#include "gpio_if.h"

//--------------------------------------------------
__attribute__(( section(".calibrationSection") ))
// channel, K*x + B
static const struct {
    float  u0[2][2];
    float  i0[2][2];
}
calibrations[] = {
	{ // dummy unconfigured values for numberless modules     //    0ch    1ch
	    { {1.000e+5,  0.000e+0}, {1.000e+5,  0.000e+0} },    // U{ {K, B}, {K, B} },
	    { {1.000e+5,  0.000e+0}, {1.000e+5,  0.000e+0} } },  // I{ {K, B}, {K, B} },
	{ // 1
	    { {3.776e-3, -5.471e-1}, {3.601e-3,  4.999e-2} },
	    { {4.506e-3, -3.851e-3}, {4.493e-3, -8.664e-3} } },
	{ // 2
	    { {3.597e-3, -8.575e-3}, {3.598e-3, -7.506e-3} },
	    { {4.517e-3, -9.077e-3}, {4.527e-3, -8.709e-2} } },
	{ // 3
	    { {0.00358035,-0.00565934}, {0.00356356,-0.00448799} },
	    { {0.00453186,-0.01413679}, {0.00450662,-0.01398897} } },
	{ // 4
	    { {1.000e+5,  0.000e+0}, {1.000e+5,  0.000e+0} },
	    { {1.000e+5,  0.000e+0}, {1.000e+5,  0.000e+0} } },
	{ // 5
	    { {1.000e+5,  0.000e+0}, {1.000e+5,  0.000e+0} },
	    { {1.000e+5,  0.000e+0}, {1.000e+5,  0.000e+0} } },
};

// калибровка управления ЦАП подстраивается по АЦП
// для каждого барьера отдельно. Поэтому тут только
// весьма потолочные значения
//--------------------------------------------------                                            //0ch    1ch
float calibrations_dac[2][2] = { {220.97308350, 1.79470825}, {222.32638550, -0.32638550} };//{ {K, B}, {K, B} }

//--------------------------------------------------
float calibrate_u0(uint16_t x, channels_t ch)/*{{{*/
{
if(ch == CH_1)
    { return calibrations[id_read()].u0[0][0] * x + calibrations[id_read()].u0[0][1]; }
else if(ch == CH_2)
    { return calibrations[id_read()].u0[1][0] * x + calibrations[id_read()].u0[1][1]; }
else
    { return -1.f; }
}/*}}}*/
//--------------------------------------------------
float calibrate_i0(uint16_t x, channels_t ch)/*{{{*/
{
if(ch == CH_1)
    { return calibrations[id_read()].i0[0][0] * x + calibrations[id_read()].i0[0][1]; }
else if(ch == CH_2)
    { return calibrations[id_read()].i0[1][0] * x + calibrations[id_read()].i0[1][1]; }
else
    { return -1.f; }
}/*}}}*/
//--------------------------------------------------
uint16_t calibrate_dac(float i, channels_t ch)/*{{{*/
{
if(ch == CH_1)
    { return (uint16_t)(calibrations_dac[0][0] * i + calibrations_dac[0][1]); }
else if(ch == CH_2)
    { return (uint16_t)(calibrations_dac[1][0] * i + calibrations_dac[1][1]); }
else
    { return (uint16_t)(-1); }
}/*}}}*/
//--------------------------------------------------
