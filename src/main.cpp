#include "mbed.h"
#include "FP.hpp"
#include "counter.hpp"

BufferedSerial pc(USBTX, USBRX, 115200);
CAN can(PA_11, PA_12, (int)1e6);

constexpr uint32_t FP_1 = 35;
constexpr uint32_t FP_2 = 40;
FirstPenguin penguin_1(FP_1, can);
FirstPenguin penguin_2(FP_2, can);

DigitalIn encoderA(PB_13);
DigitalIn encoderB(PB_14);
InterruptIn button_reset(PC_8);
InterruptIn button_stop(PC_9);

class Flags
{
public:
    static bool flag_loop;
    static bool flag_stop;
    static bool slow_move;
    static bool stop_move_lock;
    static bool flag_count;
};

class ints
{
public:
    static int counter;
    static int count_1;
    static int speed;
    static int Count_Limit;
};

bool Flags::flag_loop = true;
bool Flags::flag_stop = false;
bool Flags::slow_move = false;
bool Flags::stop_move_lock = true;
bool Flags::flag_count = false;
int ints::counter = 0;
int ints::count_1 = 0;
int ints::speed = 0;
int ints::Count_Limit = -195;
auto pre = HighResClock::now();
auto slow_move_start_time = HighResClock::now();
auto last_debounce_time = HighResClock::now();
auto pre_1 = pre;
constexpr auto debounce_delay = 50ms;
bool A_pre = 0;
bool B_pre = 0;

void motor_move(int speed)
{
    printf("motor_move\n");
    for (int i = 0; i < 3; i++)
    {
        penguin_1.pwm[i] = speed;
    }
    penguin_1.pwm[3] = -speed;
    for (int i = 0; i < 2; i++)
    {
        penguin_2.pwm[i] = -speed;
    }
}

void Setup()
{
    ints::counter = 0;
    Flags::flag_loop = false;
    button_reset.mode(PullUp);
    button_stop.mode(PullUp);
}

void Encoder()
{
    bool A_now = encoderA.read();
    bool B_now = encoderB.read();
    ints::counter = launcher_counter(A_pre, B_pre, A_now, B_now, ints::counter);
    A_pre = A_now;
    B_pre = B_now;
}

void Can_send()
{
    auto now_1 = HighResClock::now();
    if (now_1 - pre_1 > 10ms)
    {
        if (penguin_1.send() && penguin_2.send())
        {
            printf("counter = %d: can OK\n", ints::counter);
        }
        else
        {
            printf("counter = %d: can't Message\n", ints::counter);
        }
        pre_1 = now_1;
    }
}

void Reload_limit()
{
    Flags::stop_move_lock = true;
    Flags::flag_loop = true;
    motor_move(0);
    ints::counter = 0;
}

int main()
{

    Setup();
    while (1)
    {
        bool sw = button_reset.read();
        bool sw_1 = button_stop.read();

        if (!sw_1)
        {
            ints::count_1++;
        }
        else
        {
            ints::count_1--;
        }
        ints::count_1 = clamp(ints::count_1, 0, 1000);
        if (ints::count_1 == 650 && !Flags::flag_count)
        {
            Flags::flag_count = true;
            Flags::stop_move_lock = false;
            motor_move(0);

            // 1秒間の遅延を追加
            auto delay_start = HighResClock::now();
            while (HighResClock::now() - delay_start < 1000ms)
            {
                // 何もしないで待つ
            }

            motor_move(-3000);
        }
        if (ints::count_1 == 0)
        {
            Flags::flag_count = false;
        }

        Encoder();

        auto now = HighResClock::now();
        if (now - pre > 1000ms && !sw)
        {
            printf("Fire\n");
            ints::speed = 30000;
            ints::counter = 0;
            for (int i = 0; i < 3; i++)
            {
                penguin_1.pwm[i] = ints::speed;
            }
            penguin_1.pwm[3] = -ints::speed;
            for (int i = 0; i < 2; i++)
            {
                penguin_2.pwm[i] = -ints::speed;
            }
            pre = now;
        }

        Can_send();

        if (ints::counter < ints::Count_Limit && !Flags::stop_move_lock)
        {
            Reload_limit();
        }
    }
}
