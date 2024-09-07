#include "mbed.h"
#include "FP.hpp"
#include "sengen.hpp"

bool Higher_lock = true;
bool Lowre_lock = true;

bool Left_UP = false;
bool Left_DOWN = false;

bool Right_UP = false;
bool Right_DOWN = false;
// bool test_up = false;
// bool test_down = false;

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
    ints::Higher_count += Flags::Higher_stop ? -1 : 1;
    ints::Higher_count = clamp(ints::Higher_count, 0, 2000);
    if (ints::Higher_count == 600 && !Flags::Higher_flag && !Higher_lock)
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
    ints::Lower_count += Flags::Lower_stop ? -1 : 1;
    ints::Lower_count = clamp(ints::Lower_count, 0, 1000);
    if (ints::Lower_count == 600 && !Flags::Lower_flag && !Lowre_lock)
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
    ints::Higher_Reload += Flags::Higher_Reload ? 1 : -1;
    ints::Higher_Reload = clamp(ints::Higher_Reload, 0, 500);
    if (ints::Higher_Reload == 200 && !Flags::Higher_Reload)
    {
        Flags::Higher_Reload = true;
        motor_move(0);
        Higher_lock = true;
    }
    if (ints::Higher_Reload == 0)
    {
        Flags::Higher_Reload = false;
    }
}

void lower_reload()
{
    ints::Lower_Reload += Flags::Lower_Reload ? 1 : -1;
    ints::Lower_Reload = clamp(ints::Lower_Reload, 0, 100);
    if (ints::Lower_Reload == 50 && !Flags::Lower_Reload)
    {
        Flags::Lower_Reload = true;
        fire_under(0);
        Lowre_lock = true;
    }
    if (ints::Lower_Reload == 0)
    {
        Flags::Lower_Reload = false;
    }
}

void higher_fire()
{
    ints::Higher_fire += Flags::Higher_fire ? 1 : -1;
    ints::Higher_fire = clamp(ints::Higher_fire, 0, 100);
    if (ints::Higher_fire == 50 && !Flags::Higher_fire)
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
    ints::Lower_fire = clamp(ints::Lower_fire, 0, 150);
    if (ints::Lower_fire == 100 && !Flags::Lower_fire)
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
    Flags::Higher_fire = Buttons::Higher_fire.read();
    Flags::Lower_fire = Buttons::Lower_fire.read();
    Flags::Higher_Reload = Buttons::Higher_Reload.read();
    Flags::Lower_Reload = Buttons::Lower_Reload.read();

    Flags::Left_higher = Buttons::Left_higher.read();
    Flags::Left_lower = Buttons::Left_lower.read();
    Flags::Right_higher = Buttons::Right_higher.read();
    Flags::Right_lower = Buttons::Right_lower.read();
}

void Setup()
{
    Higher_lock = true;
    Lowre_lock = true;
    Buttons::Higher_fire.mode(PullUp);
    Buttons::Lower_fire.mode(PullUp);
    Buttons::Higher_stop.mode(PullUp);
    Buttons::Lower_stop.mode(PullUp);
    Buttons::Higher_Reload.mode(PullUp);
    Buttons::Lower_Reload.mode(PullUp);
    Buttons::Left_higher.mode(PullUp);
    Buttons::Left_lower.mode(PullUp);
    Buttons::Right_higher.mode(PullUp);
    Buttons::Right_lower.mode(PullUp);
}

void controler()
{
    // if (can.read(msg) && msg.id == 0x7ff)
    // {
    //     PS2Con controller;
    //     memcpy(&controller, msg.data, sizeof(PS2Con));
    // if (controller.L1 == 0 && controller.L2 == 1 && !Left_UP && !Flags::Left_higher)
    // {
    //     pwm[0] = -16000;
    //     printf("REMOVE\n");
    //     Left_DOWN = true;
    // }
    // if (!Flags::Left_higher && Left_UP)
    // {
    //     printf("Left_higher STOP\n");
    //     Left_UP = false;
    //     pwm[0] = 0;
    // }
    // if (controller.L1 == 1 && controller.L2 == 0)
    // {
    //     pwm[0] = -16000;
    //     printf("testUP\n");
    //     Left_UP = true;
    // }
    // if (controller.L1 == 0 && controller.L2 == 0)
    // {
    //     pwm[0] = 0;
    // }
    // if (controller.L1 == 0 && controller.L2 == 1)
    // {
    //     pwm[0] = 16000;
    //     printf("testdown\n");
    //     Left_DOWN = true;
    // }
    // if (!Flags::Left_lower && Left_DOWN)
    // {
    //     Left_DOWN = false;
    //     pwm[0] = 0;
    // }
    // if (controller.L1 == 1 && controller.L2 == 0 && !Left_DOWN && !Flags::Left_lower)
    // {
    //     pwm[0] = -16000;

    //     Left_UP = true;
    // }
    // ///////////////////////////////////////////////////////
    // if (controller.R1 == 0 && controller.R2 == 1 && !Right_UP && !Flags::Right_higher)
    // {
    //     pwm[0] = 16000;
    //     printf("REMOVE\n");
    //     Right_DOWN = true;
    // }
    // if (!Flags::Right_higher && Right_UP)
    // {
    //     printf("Right_higher STOP\n");
    //     Right_UP = false;
    //     pwm[0] = 0;
    // }
    // if (controller.R1 == 1 && controller.R2 == 0)
    // {
    //     pwm[0] = 16000;
    //     printf("testUP\n");
    //     Right_UP = true;
    // }
    // if (controller.R1 == 0 && controller.R2 == 0)
    // {
    //     pwm[0] = 0;
    // }
    // if (controller.R1 == 0 && controller.R2 == 1)
    // {
    //     pwm[0] = -16000;
    //     printf("testdown\n");
    //     Right_DOWN = true;
    // }
    // if (!Flags::Right_lower && Right_DOWN)
    // {
    //     Right_DOWN = false;
    //     pwm[0] = 0;
    // }
    // if (controller.R1 == 1 && controller.R2 == 0 && !Right_DOWN && !Flags::Right_lower)
    // {
    //     pwm[0] = 16000;

    //     Right_UP = true;
    // }

    // auto now = HighResClock::now();
    // if (now - pre > 1000ms && controller.Circle == 1 && Higher_lock)
    // {
    //     Higher_lock = false;
    //     motor_move(ints::Higher_speed);
    //     pre = now;
    // }
    // if (now - pre > 500ms && controller.Square == 1 && Lowre_lock)
    // {
    //     Lowre_lock = false;
    //     fire_under(ints::Lower_speed);
    //     pre = now;
    // }
    // printf("Circle: %d, Cross: %d, Square: %d, Triangle: %d\n", controller.Circle, controller.Cross, controller.Square, controller.Triangle);
    // printf("Up: %d, Right: %d, Down: %d, Left: %d\n", controller.Up, controller.Right, controller.Down, controller.Left);
    // printf("L2: %d, R2: %d, L1: %d, R1: %d\n", controller.L2, controller.R2, controller.L1, controller.R1);
    // printf("L3: %d, R3: %d, Start: %d, Select: %d\n", controller.L3, controller.R3, controller.Start, controller.Select);

    // }
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
        printf("Circle: %d, Cross: %d, Square: %d, Triangle: %d\n", controller.Circle, controller.Cross, controller.Square, controller.Triangle);
        printf("Up: %d, Right: %d, Down: %d, Left: %d\n", controller.Up, controller.Right, controller.Down, controller.Left);
        printf("L2: %d, R2: %d, L1: %d, R1: %d\n", controller.L2, controller.R2, controller.L1, controller.R1);
        printf("L3: %d, R3: %d, Start: %d, Select: %d\n", controller.L3, controller.R3, controller.Start, controller.Select);
        if (controller.L1 == 0 && controller.L2 == 1 && !Left_UP && !Flags::Left_higher)
        {
            pwm[0] = -16000;
            printf("REMOVE\n");
            Left_DOWN = true;
        }
        if (!Flags::Left_higher && Left_UP)
        {
            printf("Left_higher STOP\n");
            Left_UP = false;
            pwm[0] = 0;
        }
        if (controller.L1 == 1 && controller.L2 == 0)
        {
            pwm[0] = -16000;
            printf("testUP\n");
            Left_UP = true;
        }
        if (controller.L1 == 0 && controller.L2 == 0)
        {
            pwm[0] = 0;
        }
        if (controller.L1 == 0 && controller.L2 == 1)
        {
            pwm[0] = 16000;
            printf("testdown\n");
            Left_DOWN = true;
        }
        if (!Flags::Left_lower && Left_DOWN)
        {
            Left_DOWN = false;
            pwm[0] = 0;
        }
        if (controller.L1 == 1 && controller.L2 == 0 && !Left_DOWN && !Flags::Left_lower)
        {
            pwm[0] = -16000;

            Left_UP = true;
        }
        ///////////////////////////////////////////////////////
        if (controller.R1 == 0 && controller.R2 == 1 && !Right_UP && !Flags::Right_higher)
        {
            pwm[0] = 16000;
            printf("REMOVE\n");
            Right_DOWN = true;
        }
        if (!Flags::Right_higher && Right_UP)
        {
            printf("Right_higher STOP\n");
            Right_UP = false;
            pwm[0] = 0;
        }
        if (controller.R1 == 1 && controller.R2 == 0)
        {
            pwm[0] = 16000;
            printf("testUP\n");
            Right_UP = true;
        }
        if (controller.R1 == 0 && controller.R2 == 0)
        {
            pwm[0] = 0;
        }
        if (controller.R1 == 0 && controller.R2 == 1)
        {
            pwm[0] = -16000;
            printf("testdown\n");
            Right_DOWN = true;
        }
        if (!Flags::Right_lower && Right_DOWN)
        {
            Right_DOWN = false;
            pwm[0] = 0;
        }
        if (controller.R1 == 1 && controller.R2 == 0 && !Right_DOWN && !Flags::Right_lower)
        {
            pwm[0] = 16000;

            Right_UP = true;
        }

        auto now = HighResClock::now();
        if (now - pre > 1000ms && controller.Circle == 1 && Higher_lock)
        {
            Higher_lock = false;
            motor_move(ints::Higher_speed);
            pre = now;
        }
        if (now - pre > 500ms && controller.Square == 1 && Lowre_lock)
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
        CANMessage msg1(1, (const uint8_t *)pwm, 8);
        can.write(msg1);
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

void read_fire()
{

    // if (now - pre > 1000ms && !Flags::Higher_fire && Higher_lock)
    // {
    //     Higher_lock = false;
    //     motor_move(ints::Higher_speed);
    //     pre = now;
    // }
    // if (now - pre > 500ms && !Flags::Lower_fire && Lowre_lock)
    // {
    //     Lowre_lock = false;
    //     fire_under(ints::Lower_speed);
    //     pre = now;
    // }
}

int main()
{
    Setup();
    while (1)
    {
        read_swich();
        read_stop();
        controler();
        read_fire();
        can_send();
    }
}
