#include "mbed.h"
#include "FP.hpp"
BufferedSerial pc(USBTX, USBRX, 115200);
CAN can(PA_11, PA_12, (int)1e6);

constexpr uint32_t FP_1 = 35;
constexpr uint32_t FP_2 = 40;

FirstPenguin penguin_1(FP_1, can);
FirstPenguin penguin_2(FP_2, can);

DigitalIn encoderA(PB_13);
DigitalIn encoderB(PB_14);

void stop_motor(int speed)
{
    printf("STOP\n");

    penguin_1.pwm[0] = speed;
    penguin_1.pwm[1] = speed;
    penguin_1.pwm[2] = speed;

    penguin_1.pwm[3] = -speed;
    penguin_2.pwm[0] = -speed;
    penguin_2.pwm[1] = -speed;
}

void button_stop_pressed()
{
    stop_motor(0);
}

int main()
{
    InterruptIn button_fire(PC_9);
    InterruptIn button_stop(PC_8);
    button_fire.mode(PullUp);
    button_stop.mode(PullUp);

    auto pre = HighResClock::now();
    auto pre_1 = pre;
    int speed = 0;
    button_stop.fall(button_stop_pressed);

    while (1)
    {
        auto now = HighResClock::now();
        auto now_1 = HighResClock::now();

        bool sw = button_fire.read();

        if (now - pre > 1000ms && sw == 0)
        {
            printf("FIRE\n");
            speed = 30000;

            penguin_1.pwm[0] = speed;
            penguin_1.pwm[1] = speed;
            penguin_1.pwm[2] = speed;

            penguin_1.pwm[3] = -speed;
            penguin_2.pwm[0] = -speed;
            penguin_2.pwm[1] = -speed;

            pre = now;
        }

        if (now_1 - pre_1 > 20ms)
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
}