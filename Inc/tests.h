//---------------------------
//Модуль тестов барьеров ЛПА142
//---------------------------
#include <stdint.h>
#include <stddef.h>
//---------------------------
//кол-во каналов в стенде
//---------------------------
#define CH 2
//---------------------------
//канал 1
//---------------------------
#define ch0 0
//---------------------------
//канал 2
//---------------------------
#define ch1 1
//---------------------------
//стоп по ошибке включен
//---------------------------
#define break_on  1
//---------------------------
//стоп по ошибке выключен
//---------------------------
#define break_off 0

typedef struct error_lpa142stend{
    uint8_t flag    [CH];  // yes or no 0 or 1
    char    type_er [CH];  // 'a','b','c','d','e'
}error_lpa;

void set_error(error_lpa* er_p, uint8_t ch, char type_er);

error_lpa test_1(const int cm, char break_if_error);
error_lpa test_2(const int cm, char break_if_error);
error_lpa calibration_dacs(const int cm, char break_if_error);
error_lpa test_3_1(const int cm, char break_if_error);
error_lpa test_3_2(const int cm, char break_if_error);
error_lpa test_3_3(const int cm, char break_if_error);
error_lpa test_3_4(const int cm, char break_if_error);
error_lpa test_4_1(const int cm, char break_if_error);
error_lpa test_4_2(const int cm, char break_if_error);
void all_test(const int cm, char break_if_error);
void all_test_with_display(const int cm, char break_if_error);