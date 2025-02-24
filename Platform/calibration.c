//--------------------------------------------------
// Калибровочные значения для стенда
//--------------------------------------------------
#include "app_config.h"
#include "calibration.h"
#include "gpio_if.h"

//--------------------------------------------------
__attribute__(( section(".calibrationSection") ))
#define U_MAX 15.0F  //V
#define I_MAX 18.75F  //mA
#define ADC_I_def   I_MAX/4095.F
#define ADC_U_def   U_MAX/4095.F

// channel, K*x + b
static const struct {
    float  u0[2][2];
    float  i0[2][2];
}
//сюда заносятся результаты вычислений калибровок
//вычисления по формуле для АЦП:
// K = (int_max - int_min) / (hex_max - hex_min)
// b = int_min - k * hex_min
calibrations[] = {
	{ // dummy unconfigured values for numberless modules     //    0ch    1ch
	    { {1.000e+5,  0.000e+0}, {1.000e+5,  0.000e+0} },    // U{ {K, b}, {K, b} },
	    { {1.000e+5,  0.000e+0}, {1.000e+5,  0.000e+0} } },  // I{ {K, b}, {K, b} },
	{ // 1
	    { {3.776e-3, -5.471e-1}, {3.601e-3,  4.999e-2} },
	    { {4.506e-3, -3.851e-3}, {4.493e-3, -8.664e-3} } },
	{ // 2                                                            agilent  ацп   agilent ацп      agilent  ацп   agilent ацп
		{ {0.00358035,-0.00565934}, {0.00356356,-0.00448799} },  //max 14.015 3916 min 1.004 282 //max 14.011 3933 min 1.004 283 
	    { {0.00453186,-0.01413679}, {0.00450662,-0.01398897} } },//max 17.003 3755 min 1.001 224 //max 17.003 3776 min 1.000 225
	{ // 3 откалибровано 24.02.2025
	    { {0.00358687,-0.00491118}, {0.00358533,-0.01164901} },  //max 14.009 3907 min 1.003 281 //max 14.007 3910 min 1.003 283
	    { {0.00449621,-0.00964653}, {0.00449480,-0.00833035} } },//max 17.004 3784 min 1.002 225 //max 17.000 3784 min 1.003 225
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
// вычисляется по формуле для ЦАП:
// K = (hex_max - hex_min) / (int_max - int_min)
// b = hex_min - k * int_min
//--------------------------------------------------                                            //0ch    1ch
float calibrations_dac[2][2] = { {220.97308350, 1.79470825}, {222.765976, -0.899414} };//{ {K, b}, {K, b} }

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
