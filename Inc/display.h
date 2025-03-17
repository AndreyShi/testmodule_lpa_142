//----------------------------------
//Модуль вывода информации на дисплей
//разрешение дисплея 128x64
//-----------------------------------
#define DISPLAY_X_MAX 128U
#define DISPLAY_Y_MAX 64U

#define need_update_on_display   255
#define no_update_on_display     0

#include <stdint.h>

void display_task(void* some_data);
void show_vibor_kanalov(void);