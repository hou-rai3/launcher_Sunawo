#include "mbed.h"
#include "FP.hpp"
#include "sengen.hpp"

// can
CAN can(PA_11, PA_12, (int)1e6);
constexpr uint32_t FP_1 = 35;
constexpr uint32_t FP_2 = 40;
FirstPenguin penguin_1(FP_1, can);
FirstPenguin penguin_2(FP_2, can);

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
void handle_higher()
{
    if (!Flags::Higher_stop)
    {
        ints::Higher_count++;
    }
    else
    {
        ints::Higher_count--;
    }

    ints::Higher_count = clamp(ints::Higher_count, 0, 2000);
    if (ints::Higher_count == 600 && !Flags::Higher_flag)
    {
        Flags::Higher_flag = true;

        motor_move(3000);
    }
    if (ints::Higher_count == 0)
    {
        Flags::Higher_flag = false;
    }
}

void handle_lower()
{
    if (!Flags::Lower_stop)
    {
        ints::Lower_count++;
    }
    else
    {
        ints::Lower_count--;
    }

    ints::Lower_count = clamp(ints::Lower_count, 0, 1000);
    if (ints::Lower_count == 500 && !Flags::Lower_flag)
    {
        Flags::Lower_flag = true;

        fire_under(3000);
    }
    if (ints::Lower_count == 0)
    {
        Flags::Lower_flag = false;
    }
}

void higher_reload()
{
    if (!Flags::Higher_Reload)
    {
        ints::Higher_Reload++;
    }
    else
    {
        ints::Higher_Reload--;
    }

    ints::Higher_Reload = clamp(ints::Higher_Reload, 0, 100);
    if (ints::Higher_Reload == 50 && !Flags::Higher_Reload)
    {
        Flags::Higher_Reload = true;
        motor_move(0);
    }
    if (ints::Higher_Reload == 0)
    {
        Flags::Higher_Reload = false;
    }
}
void lower_reload()
{
    if (!Flags::Lower_Reload)
    {
        ints::Lower_Reload++;
    }
    else
    {
        ints::Lower_Reload--;
    }

    ints::Lower_Reload = clamp(ints::Lower_Reload, 0, 100);
    if (ints::Lower_Reload == 50 && !Flags::Lower_Reload)
    {
        Flags::Lower_Reload = true;
        motor_move(0);
    }
    if (ints::Lower_Reload == 0)
    {
        Flags::Lower_Reload = false;
    }
}
void read_stop()
{
    handle_higher();
    handle_lower();
    higher_reload();
    lower_reload();
}
void read_swich()
{
    Flags::Higher_fire = Buttons::Higher_fire.read();
    Flags::Higher_stop = Buttons::Higher_stop.read();
    Flags::Lower_fire = Buttons::Lower_fire.read();
    Flags::Lower_stop = Buttons::Lower_stop.read();
    Flags::Higher_Reload = Buttons::Higher_Reload.read();
    Flags::Lower_Reload = Buttons::Lower_Reload.read();
}

void Setup()
{
    Buttons::Higher_fire.mode(PullUp);
    Buttons::Higher_stop.mode(PullUp);
    Buttons::Lower_fire.mode(PullUp);
    Buttons::Lower_stop.mode(PullUp);
    Buttons::Higher_Reload.mode(PullUp);
    Buttons::Lower_Reload.mode(PullUp);
}

int main()
{

    Setup();
    while (1)
    {

        read_swich();
        read_stop();

        auto now = HighResClock::now();

        // if (!Flags::Higher_Reload)
        // printf("Higher_R\n");
        // else
        // printf("NO Higher_R\n");
        // if (!Flags::Lower_Reload)
        //     printf("lower_R\n");
        // else
        //     printf("NO lower_R\n");
        // if (!Flags::Higher_fire)
        // printf("Higher_fire\n");
        // else
        // printf("NO Higher_fire\n");
        // if (!Buttons::Higher_stop)
        // printf("Higher_stop\n");
        // else
        // printf("NO Higher_stop\n");
        // if (!Buttons::Lower_fire)
        // printf("Lower_fire\n");
        // else
        // printf("NO Lower_fire\n");
        // if (!Buttons::Lower_stop)
        // printf("Lower_stop\n");
        // else
        // printf("NO Lower_stop\n");
        if (now - pre > 1000ms && !Flags::Higher_fire)
        {
            motor_move(ints::Higher_speed);
            pre = now;
        }
        if (now - pre > 500ms && !Flags::Lower_fire)
        {
            fire_under(ints::Lower_speed);
            pre = now;
        }
        auto now_1 = HighResClock::now();
        if (now_1 - pre_1 > 10ms)
        {

            if (penguin_1.send() && penguin_2.send())
            {
                // printf("can OK %d\n", ints::Lower_count);
            }
            else
            {
                // printf("can't Message\n");
            }
            pre_1 = now_1;
        }
    }
}
