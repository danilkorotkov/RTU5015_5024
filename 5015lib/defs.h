#ifndef defs_h
#define defs_h

#define PowerLedPin   14
#define RelayLedPin   11
#define RedPin        12
#define GreenPin      13
#define Relay1Pin     1
#define Relay2Pin     0
#define IGT           6
#define PD            7
#define Input1        25
#define Input2        24
#define AT24C32       0x50
#define Capacity      680
#define NO            1
#define NC            0

#define PhoneLen 13
#define SMStxtLen 70

#define CELL          F("Сеть ")
#define tRELAY        F("Реле, сек ")
#define RELAY1ON      F("Реле1 ВКЛ\n")
#define RELAY1OFF     F("Реле1 ВЫКЛ\n")
#define RELAY2ON      F("Реле2 ВКЛ")
#define RELAY2OFF     F("Реле2 ВЫКЛ")
#define sPASS         F("Пароль: ")
#define AA            F("Доступен всем")
#define AU            F("Доступ по списку")
#define ADDED         F("Добавлен:")
#define RESETED       F("Сброс")
#define DELETED       F("Удален: ")
#define MEMCELL       F(" Ячейка")
#define NOTFOUND      F(" Не найден")
#define AlARM1        F("Тревога 1.\n")
#define AlARM2        F("Тревога 2. ")
#define sINPUT1       F("ВХОД1 ")
#define sINPUT2       F("ВХОД2 ")
#define sNC           F("NC\n")
#define sNO           F("NO\n")

struct SetsStruct {                   // Создаем пользовательскую структуру
  char Pass[6];                       // Пароль модуля /итого 4 байта
  int RelayOnTime;                    // Время работы реле в полусекундах /итого 6 байта/
  int DigitalInput;                  // Тип 00h/01h/10h/11h Первая 1вкл/0выкл  7 байт input1/input2
  int DigitalInputType;              // 0NC/1NO /итого 8 байт 00 10 11 01
  int AccessAlarm;                   // Первая 1все/0только в списке Вторая сигналка 1вкл/0выкл /итого 9 байт
//  int Notification;                  // Уведомления о реле 00 вкл, 01 первому 11 001 и 002 абоненту / Итого 90 байт
//  char AlarmTextInput1[20];
//  char AlarmTextInput2[20];           // Уведомления о срабатывании входов / 
};

#endif
