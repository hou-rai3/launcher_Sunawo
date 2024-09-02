#include "mbed.h"
#include "FP.hpp"
#include "sengen.hpp"

bool Higher_lock = true;
bool Lowre_lock = true;

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
    ints::Lower_count += Flags::Lower_stop ? -1 : 1;
    ints::Lower_count = clamp(ints::Lower_count, 0, 1000);
    if (ints::Lower_count == 600 && !Flags::Lower_flag)
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
        Lowre_lock = false;
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
}

void Setup()
{
    Higher_lock = true;
    Lowre_lock = true;
    InterruptIn *buttons[] = {
        &Buttons::Higher_fire, &Buttons::Lower_fire, &Buttons::Higher_stop, &Buttons::Lower_stop,
        &Buttons::Higher_Reload, &Buttons::Lower_Reload};
    for (auto button : buttons)
    {
        button->mode(PullUp);
    }
    // std::fill(std::begin(pwm), std::end(pwm), 0);
}

void controler()
{
    if (can.read(msg) && msg.id == 0x7ff)
    {
        Data[0] = (msg.data[0] >> 7) & 0x01;
        Data[1] = (msg.data[1] >> 7) & 0x01;
        Data[2] = (msg.data[2] >> 7) & 0x01;
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
        auto now = HighResClock::now();
        auto now_1 = HighResClock::now();

        if (now - pre > 1000ms && Data[0] == 1 && Higher_lock)
        {
            Higher_lock = false;
            motor_move(ints::Higher_speed);
            pre = now;
        }
        if (now - pre > 500ms && Data[2] == 1 && Lowre_lock)
        {
            Lowre_lock = false;
            fire_under(ints::Lower_speed);
            pre = now;
        }
        if (now - pre > 1000ms && !Flags::Higher_fire && Higher_lock)
        {
            Higher_lock = false;
            motor_move(ints::Higher_speed);
            pre = now;
        }
        if (now - pre > 500ms && !Flags::Lower_fire && Lowre_lock)
        {
            Lowre_lock = false;
            fire_under(ints::Lower_speed);
            pre = now;
        }

        if (now_1 - pre_1 > 30ms)
        {
            CANMessage msg1(1, (const uint8_t *)pwm, 8);
            can.write(msg1);
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
            }
            pre_1 = now_1;
        }
    }
}
