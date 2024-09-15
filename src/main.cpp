#include "mbed.h"
#include "FP.hpp"
#include "sengen.hpp"

bool Higher_lock = false;
bool Lowre_lock = false;

int output_int16_zero = 0;
int signed_speed = 0;
int speed = 10000;
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
    // printf("Higher_count==%d\n", ints::Higher_count);
    // printf("Lower_count==%d\n", ints::Lower_count);
    // printf("Higher_Reload==%d\n", ints::Higher_Reload);
    // printf("Lower_Reload==%d\n", ints::Lower_Reload);
    ints::Higher_count += !Flags::Higher_stop ? 1 : -1;
    ints::Higher_count = clamp(ints::Higher_count, 0, 15);
    if (ints::Higher_count == 10 && !Flags::Higher_flag && !Higher_lock)
    {
        Flags::Higher_flag = true;
        motor_move(3000);
        printf("handle_higher\n");
    }
    if (ints::Higher_count == 0)
    {
        Flags::Higher_flag = false;
    }
}

void handle_lower()
{
    ints::Lower_count += !Flags::Lower_stop ? 1 : -1;
    ints::Lower_count = clamp(ints::Lower_count, 0, 15);
    if (ints::Lower_count == 10 && !Flags::Lower_flag && !Lowre_lock)
    {
        Flags::Lower_flag = true;
        fire_under(3000);
        printf("handle_lower\n");
    }
    if (ints::Lower_count == 0)
    {
        Flags::Lower_flag = false;
    }
}

void higher_reload()
{
    ints::Higher_Reload += !Flags::Higher_Reload ? -1 : 1;
    ints::Higher_Reload = clamp(ints::Higher_Reload, 0, 15);
    if (ints::Higher_Reload == 10 && !Flags::Higher_Reload)
    {
        Higher_lock = true;
        Flags::Higher_Reload = true;
        motor_move(0);
        printf("higher_reload\n");
    }
    if (ints::Higher_Reload == 0)
    {
        Flags::Higher_Reload = false;
    }
}

void lower_reload()
{
    ints::Lower_Reload += !Flags::Lower_Reload ? -1 : 1;
    ints::Lower_Reload = clamp(ints::Lower_Reload, 0, 15);
    if (ints::Lower_Reload == 10 && !Flags::Lower_Reload)
    {
        Lowre_lock = true;
        Flags::Lower_Reload = true;
        fire_under(0);
        printf("lower_reload\n");
    }
    if (ints::Lower_Reload == 0)
    {
        Flags::Lower_Reload = false;
    }
}

void higher_fire()
{
    ints::Higher_fire += Flags::Higher_fire ? 1 : -1;
    ints::Higher_fire = clamp(ints::Higher_fire, 0, 50);
    if (ints::Higher_fire == 20 && !Flags::Higher_fire)
    {
        Flags::Higher_fire = true;
        motor_move(ints::Higher_speed);
    }
    if (ints::Higher_fire == 0)
    {
        Flags::Higher_fire = false;
    }
}

void lower_fire()
{
    ints::Lower_fire += Flags::Lower_fire ? 1 : -1;
    ints::Lower_fire = clamp(ints::Lower_fire, 0, 50);
    if (ints::Lower_fire == 30 && !Flags::Lower_fire)
    {
        Flags::Lower_fire = true;
        fire_under(ints::Lower_speed);
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
}

void read_swich()
{
    Flags::Higher_stop = Buttons::Higher_stop.read();
    Flags::Lower_stop = Buttons::Lower_stop.read();
    Flags::Higher_Reload = Buttons::Higher_Reload.read();
    Flags::Lower_Reload = Buttons::Lower_Reload.read();
}

void Setup()
{
    Higher_lock = true;
    Lowre_lock = true;
    Buttons::Higher_stop.mode(PullUp);
    Buttons::Lower_stop.mode(PullUp);
    Buttons::Higher_Reload.mode(PullUp);
    Buttons::Lower_Reload.mode(PullUp);
}

void controler()
{
    if (can.read(msg) && msg.id == 0x7ff)
    {
        PS2Con controller;

        controller.Circle = (msg.data[4] >> 0) & 0x01;
        controller.Cross = (msg.data[4] >> 1) & 0x01;
        controller.Square = (msg.data[4] >> 2) & 0x01;
        controller.Triangle = (msg.data[4] >> 3) & 0x01;
        controller.Up = (msg.data[4] >> 4) & 0x01;
        controller.Right = (msg.data[4] >> 5) & 0x01;
        controller.Down = (msg.data[4] >> 6) & 0x01;
        controller.Left = (msg.data[4] >> 7) & 0x01;

        controller.L2 = (msg.data[5] >> 0) & 0x01;
        controller.R2 = (msg.data[5] >> 1) & 0x01;
        controller.L1 = (msg.data[5] >> 2) & 0x01;
        controller.R1 = (msg.data[5] >> 3) & 0x01;
        controller.L3 = (msg.data[5] >> 4) & 0x01;
        controller.R3 = (msg.data[5] >> 5) & 0x01;
        controller.Start = (msg.data[5] >> 6) & 0x01;
        controller.Select = (msg.data[5] >> 7) & 0x01;

        // printf("Circle: %d, Cross: %d, Square: %d, Triangle: %d\n", controller.Circle, controller.Cross, controller.Square, controller.Triangle);
        // printf(" Right: %d, DOWN: %d\n", controller.Right, controller.Down);
        // printf(" R1: %d\n", controller.R1);
        // printf("L3: %d, R3: %d, Start: %d, Select: %d\n", controller.L3, controller.R3, controller.Start, controller.Select);

        ///////////////////////////////////////////////////////
        if (controller.L1 == 1)
        {
            printf("L1\n");
            signed_speed = static_cast<int16_t>(10000);
            C610[4] = signed_speed & 0xFF;         // 下位バイト
            C610[5] = -(signed_speed >> 8) & 0xFF; // 上位バイト
        }
        if (controller.L1 == 0 && controller.Down == 0)
        {
            C610[4] = output_int16_zero & 0xFF;        // 下位バイト
            C610[5] = (output_int16_zero >> 8) & 0xFF; // 上位バイト
        }

        if (controller.Down == 1)
        {
            printf("Down\n");
            signed_speed = static_cast<int16_t>(10000);
            C610[4] = -signed_speed & 0xFF;       // 下位バイト
            C610[5] = (signed_speed >> 8) & 0xFF; // 上位バイト
        }
        ///////////////////////////////////////////////////////
        if (controller.R1 == 1)
        {
            signed_speed = static_cast<int16_t>(10000);
            C610[6] = -signed_speed & 0xFF;        // 下位バイト
            C610[7] = (signed_speed >> 8) & 0xFF; // 上位バイト
            printf("signed_speed%d\n,", signed_speed);
        }
        if (controller.R1 == 0 && controller.Right == 0)
        {
            C610[6] = output_int16_zero & 0xFF;        // 下位バイト
            C610[7] = (output_int16_zero >> 8) & 0xFF; // 上位バイト
        }

        if (controller.Right == 1)
        {
            printf("Right\n");
            signed_speed = static_cast<int16_t>(10000);
            C610[6] = signed_speed & 0xFF;         // 下位バイト
            C610[7] = -(signed_speed >> 8) & 0xFF; // 上位バイト
        }
        ///////////////////////////////////////////////////////
        auto now = HighResClock::now();
        if (now - pre > 1000ms && controller.Circle == 1 && Higher_lock)
        {
            Higher_lock = false;
            motor_move(ints::Higher_speed);
            pre = now;
        }
        if (now - pre > 1000ms && controller.Square == 1 && Lowre_lock)
        {
            Lowre_lock = false;
            fire_under(ints::Lower_speed);
            pre = now;
        }
    }
}

void can_send()
{
    auto now_1 = HighResClock::now();
    if (now_1 - pre_1 > 30ms)
    {
        // printf(
        //     "Higher_Reload = %1d  "
        //     "Lower_Reload = %2d  "
        //     "Higher_stop = %3d  "
        //     "Lower_stop = %4d\n",
        //     Flags::Higher_Reload,
        //     Flags::Lower_Reload,
        //     Flags::Higher_stop,
        //     Flags::Lower_stop);
        CANMessage msg1(1, (const uint8_t *)pwm, 8);
        can.write(msg1);
        CANMessage msgC610(0x1FF, C610, 8);
        can2.write(msgC610);

        if (penguin_1.send() && penguin_2.send())
        {
            // printf("can OK \n");
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
        controler();
        can_send();
    }
}
