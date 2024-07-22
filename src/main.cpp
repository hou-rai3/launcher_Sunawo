#include "mbed.h"
#include "FP.hpp"
#include "sengen.hpp"

void motor_move(int speed)
{
    penguin_1.pwm[1] = -speed;
    penguin_1.pwm[2] = -speed;
    penguin_1.pwm[3] = speed;
    penguin_2.pwm[0] = speed;
}

void fire_under(int speed)
{
    penguin_1.pwm[0] = -speed;
    penguin_2.pwm[1] = -speed;
}

void read_stop()
{
    if (!Flags::sw_1)
    {
        ints::Higher_count++;
    }
    else
    {
        ints::Higher_count--;
    }

    ints::Higher_count = clamp(ints::Higher_count, 0, 1000);
    if (ints::Higher_count == 300 && !Flags::Higher_flag)
    {
        Flags::Higher_flag = true;
        Flags::stop_move_lock = false;
        motor_move(0);
    }
    if (ints::Higher_count == 0)
    {
        Flags::Higher_flag = false;
    }

    if (!Flags::sw_3)
    {
        ints::Higher_count++;
    }
    else
    {
        ints::Lower_count--;
    }

    ints::Lower_count = clamp(ints::Lower_count, 0, 100);
    if (ints::Lower_count == 50 && !Flags::Lower_flag)
    {
        Flags::Lower_flag = true;
        Flags::stop_move_lock = false;
        fire_under(0);
    }
    if (ints::Lower_count == 0)
    {
        Flags::Lower_flag = false;
    }
}
void read_swich()
{
    Flags::sw = Buttons::Higher_fire.read();
    Flags::sw_1 = Buttons::Higher_stop.read();
    Flags::sw_2 = Buttons::Lower_fire.read();
    Flags::sw_3 = Buttons::Lower_stop.read();
}

void Setup()
{
    Buttons::Higher_fire.mode(PullUp);
    Buttons::Higher_stop.mode(PullUp);
    Buttons::Lower_fire.mode(PullUp);
    Buttons::Lower_stop.mode(PullUp);
}

void Can_send()
{
    auto now_1 = HighResClock::now();
    if (now_1 - pre_1 > 50ms)
    {

        if (penguin_1.send() && penguin_2.send())
        {
            printf("can OK\n");
        }
        else
        {
            printf("can't Message\n");
        }
        pre_1 = now_1;
    }
}

int main()
{

    Setup();
    while (1)
    {

        read_swich();
        read_stop();

        auto now = HighResClock::now();
        if (now - pre > 1000ms && !Flags::sw)
        {
            motor_move(ints::Higher_speed);
        }
        if (now - pre > 500ms && !Flags::sw_2)
        {
            fire_under(ints::Lower_speed);
        }
    }
}
