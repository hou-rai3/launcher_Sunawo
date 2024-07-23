#ifndef SENGEN_HPP
#define SENGEN_HPP
#include <mbed.h>
class Flags
{
public:
    static bool stop_move_lock;
    static bool Higher_flag;
    static bool Lower_flag;
    static bool Higher_fire;
    static bool Higher_stop;
    static bool Lower_fire;
    static bool Lower_stop;
    static bool Reload;
};
class Buttons
{
public:
    // button
    static InterruptIn Reload;
    static InterruptIn Higher_fire;
    static InterruptIn Higher_stop;
    static InterruptIn Lower_fire;
    static InterruptIn Lower_stop;
};
class ints
{
public:
    static int Reload;
    static int counter;
    static int Higher_count;
    static int Lower_count;
    static int Higher_speed;
    static int Lower_speed;
    static int Count_Limit;
};
// Flags
bool Flags::stop_move_lock = true;
bool Flags::Higher_flag = false;
bool Flags::Lower_flag = false;
bool Flags::Higher_fire = false;
bool Flags::Higher_stop = false;
bool Flags::Lower_fire = false;
bool Flags::Lower_stop = false;
bool Flags::Reload = false;

InterruptIn Buttons::Reload(PC_1);
InterruptIn Buttons::Higher_fire(PC_8);
InterruptIn Buttons::Higher_stop(PC_9);
InterruptIn Buttons::Lower_fire(PC_0);
InterruptIn Buttons::Lower_stop(PC_11);
// ints

int ints::Reload = 0;
int ints::Higher_count = 0;
int ints::Lower_count = 0;
int ints::Higher_speed = 30000;
int ints::Lower_speed = 30000;
int ints::Count_Limit = -195;

// 通信
BufferedSerial pc(USBTX, USBRX, 115200);
// can
CAN can(PA_11, PA_12, (int)1e6);
constexpr uint32_t FP_1 = 35;
constexpr uint32_t FP_2 = 40;
FirstPenguin penguin_1(FP_1, can);
FirstPenguin penguin_2(FP_2, can);

// timer
auto pre = HighResClock::now();
auto slow_move_start_time = HighResClock::now();
auto last_debounce_time = HighResClock::now();
auto pre_1 = pre;
constexpr auto debounce_delay = 50ms;

#endif
