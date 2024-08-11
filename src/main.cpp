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
        printf("Higher_Reload\n");
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
    if (ints::Lower_count == 600 && !Flags::Lower_flag)
    {
        Flags::Lower_flag = true;
        fire_under(3000);
        printf("Lower_Reload\n");
    }
    if (ints::Lower_count == 0)
    {
        Flags::Lower_flag = false;
    }
}

void higher_reload()
{
    if (Flags::Higher_Reload)
    {
        ints::Higher_Reload++;
    }
    else
    {
        ints::Higher_Reload--;
    }

    ints::Higher_Reload = clamp(ints::Higher_Reload, 0, 500);
    if (ints::Higher_Reload == 200 && !Flags::Higher_Reload)
    {
        Flags::Higher_Reload = true;
        motor_move(0);
        printf("Higher_Reload\n");
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
        fire_under(0);
        printf("Lower_Reload\n");
    }
    if (ints::Lower_Reload == 0)
    {
        Flags::Lower_Reload = false;
    }
}

void higher_fire()
{
    if (Flags::Higher_fire)
    {
        ints::Higher_fire++;
    }
    else
    {
        ints::Higher_fire--;
    }

    ints::Higher_fire = clamp(ints::Higher_fire, 0, 100);
    if (ints::Higher_fire == 50 && !Flags::Higher_fire)
    {
        Flags::Higher_fire = true;
        motor_move(ints::Higher_speed);
        printf("Higher_fire\n");
    }
    if (ints::Higher_fire == 0)
    {
        Flags::Higher_fire = false;
    }
}

void lower_fire()
{
    if (!Flags::Lower_fire)
    {
        ints::Lower_fire++;
    }
    else
    {
        ints::Lower_fire--;
    }

    ints::Lower_fire = clamp(ints::Lower_fire, 0, 150);
    if (ints::Lower_fire == 100 && !Flags::Lower_fire)
    {
        Flags::Lower_fire = true;
        fire_under(ints::Lower_speed);
        printf("Lower_fire\n");
    }
    if (ints::Lower_fire == 0)
    {
        Flags::Lower_fire = false;
    }
}

void read_stop()
{
    handle_higher();
    handle_lower();
    higher_reload();
    lower_reload();
    // higher_fire();
    // lower_fire();
}

void read_swich()
{
    Flags::Higher_stop = Buttons::Higher_stop.read();
    Flags::Lower_stop = Buttons::Lower_stop.read();
    Flags::Higher_fire = Buttons::Higher_fire.read();
    Flags::Lower_fire = Buttons::Lower_fire.read();
    Flags::Higher_Reload = Buttons::Higher_Reload.read();
    Flags::Lower_Reload = Buttons::Lower_Reload.read();
}

void Setup()
{
    Buttons::Higher_fire.mode(PullUp);
    Buttons::Lower_fire.mode(PullUp);
    Buttons::Higher_stop.mode(PullUp);
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

        if (can.read(msg) && msg.id == 0x7ff)
        {
            Data[0] = (msg.data[0] >> 7) & 0x01; // circle
            Data[1] = (msg.data[1] >> 7) & 0x01; // cross
            Data[2] = (msg.data[2] >> 7) & 0x01; // rectangle
        }

        auto now = HighResClock::now();
        auto now_1 = HighResClock::now();
        if (now - pre > 1000ms && Data[0] == 1) // 上発射
        {
            printf("fire\n");
            motor_move(ints::Higher_speed);
            pre = now;
        }
        if (now - pre > 500ms && Data[2] == 1) // 下発射
        {
            fire_under(ints::Lower_speed);
            pre = now;
        }
        if (now - pre > 1000ms && !Flags::Higher_fire) // 上発射
        {
            printf("fire\n");
            motor_move(ints::Higher_speed);
            pre = now;
        }
        if (now - pre > 500ms && !Flags::Lower_fire) // 下発射
        {
            fire_under(ints::Lower_speed);
            pre = now;
        }

        if (now_1 - pre_1 > 30ms)
        {
            if (penguin_1.send() && penguin_2.send())
            {
                printf("can OK \n");
            }
            else
            {
                if (!penguin_1.send())
                {
                    printf("penguin_1 send failed\n");
                }
                if (!penguin_2.send())
                {
                    printf("penguin_2 send failed\n");
                }
                printf("can't Message\n");

                // CAN通信のリセットを試みる
                // can.reset();
            }
            pre_1 = now_1;
        }
    }
}
// if (now - pre > 1000ms && Flags::Higher_fire) // 上発射
// {
//     motor_move(ints::Higher_speed);
//     pre = now;
// }
// if (now - pre > 500ms && !Flags::Lower_fire) // 下発射
// {
//     fire_under(ints::Lower_speed);
//     pre = now;
// }
