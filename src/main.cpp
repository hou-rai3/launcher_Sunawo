#include "mbed.h"
#include "PID.hpp"

InterruptIn button_reset(PC_9); // 起動スイッチ

CAN can(PA_11, PA_12, (int)1e6);
CANMessage msg;
uint8_t DATA[8] = {0};
int DJI_ID = 0x200;
int16_t speed = 0;

bool flag = false;

void stop_motor(int zero)
{
    printf("STOP\n");
    for (int i = 0; i < 8; i += 2)
    {
        DATA[i] = (zero >> 8) & 0xFF; // 上位バイト
        DATA[i + 1] = zero & 0xFF;    // 下位バイト
    }
}

int main()
{
    button_reset.mode(PullUp);
    BufferedSerial pc(USBTX, USBRX, 115200);
    auto pre = HighResClock::now();
    auto pre_1 = pre;

    while (1)
    {
        auto now = HighResClock::now();
        auto now_1 = HighResClock::now();
        bool sw = button_reset.read();

        if (now - pre > 1000ms && sw == 0)
        {
            printf("FIRE\n");
            speed = 6000; // 速度MAX

            int16_t signed_speed = static_cast<int16_t>(-speed);
            DATA[0] = (signed_speed >> 8) & 0xFF; // 上位バイト
            DATA[1] = signed_speed & 0xFF;        // 下位バイト
            signed_speed = static_cast<int16_t>(speed);
            DATA[2] = (signed_speed >> 8) & 0xFF; // 上位バイト
            DATA[3] = signed_speed & 0xFF;        // 下位バイト
            signed_speed = static_cast<int16_t>(-speed);
            DATA[4] = (signed_speed >> 8) & 0xFF; // 上位バイト
            DATA[5] = signed_speed & 0xFF;        // 下位バイト
            signed_speed = static_cast<int16_t>(speed);
            DATA[6] = (signed_speed >> 8) & 0xFF; // 上位バイト
            DATA[7] = signed_speed & 0xFF;        // 下位バイト

            pre = now;
            can.reset(); // CANコントローラをリセット
            flag = true;
        }

        if (flag == true && now - pre > 500ms)
        {
            stop_motor(0);
            flag = false;
        }

        if (now_1 - pre_1 > 10ms)
        {
            CANMessage msg(DJI_ID, DATA, 8);
            if (can.write(msg))
            {
                can.reset();
                // CANコントローラをリセット
                //  printf("OK\n");
            }
            else
            {
                printf("Can't send Message\n");
                printf("CAN Bus Error Status: %d\n", can.rderror());
                printf("CAN Bus Write Error Count: %d\n", can.tderror());
                if (can.rderror() == 255 || can.tderror() == 249)
                {
                    printf("Resetting CAN controller\n");
                    can.reset();
                }
            }
            pre_1 = now_1;
        }
    }
}
