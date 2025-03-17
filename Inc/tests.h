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
//---------------------------
//вывести на дисплей выбор каналов
//---------------------------
#define lets_show_vibor_kanalov 1
//---------------------------
//оставить текущий вывод
//---------------------------
#define lets_dont_change_show   0
//---------------------------
//stages 
//---------------------------
#define stage_CheckU          0  // проверка напряжения датчика барьера
#define stage_CheckI          1  // проверка тока датчика барьера
#define stage_AutoDacCalibr   2  // автоматическая калибровка ЦАПА
#define stage_Check_bot       3  // проверка с имитацией датчика по всему диапазону по схеме "Нижний ключ" без инверсии
#define stage_Check_bot_Inv   4  // проверка с имитацией датчика по всему диапазону по схеме "Нижний ключ" с инверсией
#define stage_Check_top       5  // проверка с имитацией датчика по всему диапазону по схеме "Верхний ключ" без инверсии
#define stage_Check_top_Inv   6  // проверка с имитацией датчика по всему диапазону по схеме "Верхний ключ" с инверсией
#define stage_Check_Freq_bot  7  // проверка задержки прохождения сигнала по схеме "Нижний ключ"
#define stage_Check_Freq_top  8  // проверка задержки прохождения сигнала по схеме "Верхний ключ"

typedef struct error_lpa142stend{
    uint8_t stage;
    uint8_t flag    [CH];  // yes or no 0 or 1
    uint8_t type_er [CH];  // 0 - А, 1 - Б, 2 - В, 3 - Г, 4 - Д,...
}error_lpa;

void set_error(error_lpa* er_p, uint8_t ch, uint8_t type_er);

error_lpa test_1(const int cm, char break_if_error);
error_lpa test_2(const int cm, char break_if_error);
error_lpa calibration_dacs(const int cm, char break_if_error);
error_lpa test_3a(const int cm, char break_if_error);
error_lpa test_3b(const int cm, char break_if_error);
error_lpa test_3c(const int cm, char break_if_error);
error_lpa test_3d(const int cm, char break_if_error);
error_lpa test_4a(const int cm, char break_if_error);
error_lpa test_4b(const int cm, char break_if_error);
void all_test(const int cm, char break_if_error);
int all_test_with_display(const int cm, char break_if_error);

void diagnostics(const int cm);