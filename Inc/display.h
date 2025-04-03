//----------------------------------
//Модуль вывода информации на дисплей
//разрешение дисплея 128x64
//-----------------------------------
#define DISPLAY_X_MAX 128U
#define DISPLAY_Y_MAX 64U

#define need_update_on_display   255
#define no_update_on_display     0

#define pic_PROSHIVKA   1
#define pic_TEST1       2
#define pic_TEST2       3
#define pic_SHESTERENKI 4
#define pic_TEST3a      5
#define pic_TEST3b      6
#define pic_TEST3c      7
#define pic_TEST3d      8
#define pic_TEST4a      9
#define pic_TEST4b      10
#define pic_OSHIBKA     11
#define pic_VSE_OK      12


#include <stdint.h>

void display_task(void* some_data);
void show_vibor_kanalov(void);