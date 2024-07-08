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
InterruptIn button_reset(PC_9);
InterruptIn button_stop(PC_8);

int counter = 0;
bool flag_loop = true;
bool flag_stop = false;
bool slow_move = false;
bool stop_move_lock = true;
auto pre = HighResClock::now();
auto pre_1 = pre;
auto slow_move_start_time = HighResClock::now();
int speed = 0;
int Count_Limit = -2000;
bool A_pre = 0;
bool B_pre = 0;

void stop_motor(int speed)
{
    printf("停止\n");
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

void slowmove(int speed)
{
    printf("低速動作\n");
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

bool button_stop_pressed()
{
    printf("停止スイッチ確認\n");
    counter = 0;
    stop_motor(0);
    flag_stop = true;
    return flag_stop;
}

void Setup()
{
    button_reset.mode(PullUp);
    button_stop.mode(PullUp);
    button_stop.fall(button_stop_pressed); // button_stopが押された時にstop_motorを実行
}

void Encoder()
{
    bool A_now = encoderA.read();
    bool B_now = encoderB.read();
    counter = launcher_counter(A_pre, B_pre, A_now, B_now, counter);
    A_pre = A_now;
    B_pre = B_now;
}

void Reload()
{

    auto now = HighResClock::now();
    if (now - slow_move_start_time > 1000ms)
    {
        counter = 0;
        slowmove(2000);
        slow_move_start_time = now; // slow_move_start_timeを更新する
    }
}

void Fire()
{
    flag_loop = false;
    auto now = HighResClock::now();
    if (now - pre > 1000ms && !button_reset.read())
    {
        printf("発射\n");
        speed = 30000;
        counter = 0;
        for (int i = 0; i < 3; i++)
        {
            penguin_1.pwm[i] = speed;
        }
        penguin_1.pwm[3] = -speed;
        for (int i = 0; i < 2; i++)
        {
            penguin_2.pwm[i] = -speed;
        }
        pre = now;
    }
}

void Can_send()
{

    auto now_1 = HighResClock::now();
    if (now_1 - pre_1 > 10ms)
    {
        if (penguin_1.send() && penguin_2.send())
        {
            printf("counter = %d: can OK\n", counter);
        }
        else
        {
            printf("counter = %d: can't Message\n", counter);
        }
        pre_1 = now_1;
    }
}

void Reload_limit()
{
    stop_move_lock = true;
    flag_loop = true;
    stop_motor(0);
    counter = 0;
}

int main()
{
    Setup();
    while (1)
    {
        Encoder();
        if (flag_stop == true)
        {
            Reload();
            flag_stop = false;
            stop_move_lock = false;
        }
        Fire();
        Can_send();
        if (counter < Count_Limit && !stop_move_lock)
        {
            Reload_limit();
        }
    }
}
