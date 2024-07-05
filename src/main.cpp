#include "mbed.h"
#include "FP.hpp"
#include "counter.hpp"

BufferedSerial pc(USBTX, USBRX, 115200);
CAN can(PA_11, PA_12, (int)1e6);

constexpr uint32_t FP_1 = 35; // CAN通信のID
constexpr uint32_t FP_2 = 40; // CAN通信のID
FirstPenguin penguin_1(FP_1, can);
FirstPenguin penguin_2(FP_2, can);

DigitalIn encoderA(PB_13); // A相のピン A0からA5
DigitalIn encoderB(PB_14); // B相のピン A0からA5

bool stop_motor(int speed)
{
    printf("STOP\n");

    penguin_1.pwm[0] = speed;
    penguin_1.pwm[1] = speed;
    penguin_1.pwm[2] = speed;

    penguin_1.pwm[3] = -speed;
    penguin_2.pwm[0] = -speed;
    penguin_2.pwm[1] = -speed;
}

void slowmove(int speed)
{
    printf("slowmove Ready\n");

    penguin_1.pwm[0] = speed;
    penguin_1.pwm[1] = speed;
    penguin_1.pwm[2] = speed;

    penguin_1.pwm[3] = -speed;
    penguin_2.pwm[0] = -speed;
    penguin_2.pwm[1] = -speed;
}

void button_stop_pressed()
{
    printf("button Read");
    stop_motor(0);
}

int main()
{
    InterruptIn button_reset(PC_9);
    InterruptIn button_stop(PC_8);
    button_reset.mode(PullUp);
    button_stop.mode(PullUp);

    button_stop.fall(&button_stop_pressed); // button_stopが押された時にstop_motorを実行

    auto pre = HighResClock::now();
    auto pre_1 = pre;
    auto slow_move_start_time = HighResClock::now(); // slowmoveを開始する時間を追跡
    bool slow_move_started = false;                  // slowmoveが開始されたかどうかを追跡
    bool A_pre = 0;
    bool B_pre = 0;
    int counter = 0;
    int speed = 0;
    int Count_Limit = -60150;
    bool flag_reset = false;

    while (1)
    {
        auto now = HighResClock::now();
        auto now_1 = HighResClock::now();

        bool A_now = encoderA.read();
        bool B_now = encoderB.read();

        counter = launcher_counter(A_pre, B_pre, A_now, B_now, counter);

        A_pre = A_now;
        B_pre = B_now;

        if (now - pre > 500ms && !button_stop.read())
        {
            stop_motor(0);
            flag_reset = true;
            counter = 0;
            // slowmoveを開始する時間を設定
            slow_move_start_time = HighResClock::now();
            slow_move_started = true;

            pre = now;
        }

        // slowmoveが開始されてから100ms経過した場合にslowmoveを実行
        if (slow_move_started && now - slow_move_start_time > 1000ms)
        {
            slowmove(2000);

            slow_move_started = false; // slowmoveの実行後はフラグをリセット
        }
        if (counter < Count_Limit && flag_reset)
        {
            printf("count\n");
            stop_motor(0);
            counter = 0;
            flag_reset = false;
        }
        if (now - pre > 1000ms && !button_reset.read())
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

        if (now_1 - pre_1 > 10ms)
        {
            if (penguin_1.send() && penguin_2.send())
            {
                printf("counter=%d: can OK\n", counter);
            }
            else
            {
                printf("counter=%d: can't Message\n", counter);
            }
            pre_1 = now_1;
        }
    }
}
