#ifndef SENGEN_HPP
#define SENGEN_HPP
#include <mbed.h>
class Flags
{
public:
    static bool Higher_fire;
    static bool Lower_fire;
    static bool Higher_flag;
    static bool Lower_flag;
    static bool Higher_stop;
    static bool Lower_stop;
    static bool Higher_Reload;
    static bool Lower_Reload;
};
class Buttons
{
public:
    static InterruptIn Higher_fire;
    static InterruptIn Lower_fire;
    static InterruptIn Higher_stop;
    static InterruptIn Lower_stop;
    static InterruptIn Higher_Reload;
    static InterruptIn Lower_Reload;
};
class ints
{
public:
    static int Higher_fire;
    static int Lower_fire;
    static int Higher_Reload;
    static int Lower_Reload;
    static int Higher_speed;
    static int Lower_speed;
    static int Higher_count;
    static int Lower_count;
};
// Flags
bool Flags::Higher_fire = false;
bool Flags::Lower_fire = false;
bool Flags::Higher_flag = false;
bool Flags::Lower_flag = false;
bool Flags::Higher_stop = false;
bool Flags::Lower_stop = false;
bool Flags::Higher_Reload = false;
bool Flags::Lower_Reload = false;

InterruptIn Buttons::Lower_Reload(PC_4);
InterruptIn Buttons::Higher_Reload(PC_5);
InterruptIn Buttons::Higher_stop(PC_8);
InterruptIn Buttons::Lower_stop(PC_9);
InterruptIn Buttons::Higher_fire(PC_12);
InterruptIn Buttons::Lower_fire(PC_13);

// intsint ints::Lower_fire = 0;
int ints::Higher_fire = 0;
int ints::Higher_speed = 22000;
int ints::Lower_speed = 31000;
int ints::Higher_Reload = 0;
int ints::Lower_Reload = 0;
int ints::Higher_count = 0;
int ints::Lower_count = 0;

// timer
auto pre = HighResClock::now();
auto pre_1 = pre;

// 通信
BufferedSerial pc(USBTX, USBRX, 115200);
// can
CAN can(PA_11, PA_12, (int)1e6);
CAN R_can(PB_12, PB_13, (int)1e6);
CANMessage msg;
constexpr uint32_t FP_1 = 35;
constexpr uint32_t FP_2 = 40;
FirstPenguin penguin_1(FP_1, can);
FirstPenguin penguin_2(FP_2, can);
uint16_t Data[4] = {};

typedef struct
{
    unsigned char circle : 1;
    unsigned char cross : 1;
    unsigned char rectangle : 1;
} PS2Con;

#endif
