#include "mbed.h"
uint8_t DATA_1[8] = {};
uint8_t DATA_2[8] = {};

int main()
{

    auto now = HighResClock::now();
    static auto pre = now;

    DigitalIn button(D4); // リミットスイッチ
    BufferedSerial pc(USBTX, USBRX, 115200);
    CAN can(PA_11, PA_12, (int)1e6);
    button.mode(PullUp);

    int16_t motor_1 = 0;
    int16_t motor_2 = 0;
    int16_t speed = 30000; // 16ビットの最大値

    motor_1 = speed;
    motor_2 = speed;

    int16_t output_1 = motor_1;
    DATA_1[0] = output_1 & 0xFF;        // 下位バイト
    DATA_1[1] = (output_1 >> 8) & 0xFF; // 上位バイト

    int16_t output_2 = motor_2;
    DATA_2[0] = output_2 & 0xFF;        // 下位バイト
    DATA_2[1] = (output_2 >> 8) & 0xFF; // 上位バイ
    while (1)
    {
        int sw = button.read(); // スイッチの状態を読み取る
        CANMessage msg0(0x1FF, DATA_1, 8);
        CANMessage msg1(0x200, DATA_2, 8);
        can.write(msg0);
        can.write(msg1);
        if (sw == 0)
        {
            int16_t speed = 0;
            motor_1 = speed;
            motor_2 = speed;

            int16_t output_1 = motor_1;
            DATA_1[0] = output_1 & 0xFF;        // 下位バイト
            DATA_1[1] = (output_1 >> 8) & 0xFF; // 上位バイト

            int16_t output_2 = motor_2;
            DATA_2[0] = output_2 & 0xFF;        // 下位バイト
            DATA_2[1] = (output_2 >> 8) & 0xFF; // 上位バイ
        }
        if (now - pre > 100ms)
        {
            pre = now;
        }
    }
}