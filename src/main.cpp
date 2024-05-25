#include "mbed.h"
uint8_t DATA[8] = {};
InterruptIn button(PC_8);       // リミットスイッチ
InterruptIn button_reset(PC_9); // 起動スイッチ
BufferedSerial pc(USBTX, USBRX, 115200);
CAN can(PA_11, PA_12, (int)1e6);
int DJI_ID = 0x200;

int16_t motor_1 = 0;
int16_t motor_2 = 0;
int16_t motor_3 = 0;
int16_t speed = 0; // 16ビットの最大値

int reset;
int sw;

void Switch_Stop()
{
    printf("Stop\n");
    speed = 0; // 速度0

    DATA[0] = speed & 0xFF;        // 下位バイト
    DATA[1] = (speed >> 8) & 0xFF; // 上位バイト
    DATA[2] = speed & 0xFF;        // 下位バイト
    DATA[3] = (speed >> 8) & 0xFF; // 上位バイト
    DATA[4] = speed & 0xFF;        // 下位バイト
    DATA[5] = (speed >> 8) & 0xFF; // 上位バイト
}

int main()
{
    button.mode(PullUp);
    button_reset.mode(PullUp);

    button.fall(&Switch_Stop); // 割り込み設定

    while (1)
    {

        auto now = HighResClock::now(); // タイマー設定
        static auto pre = now;
        bool sw = button_reset.read(); // ボタン読み取り

        if (now - pre > 500ms && sw == 0)
        {
            printf("Restart\n");
            speed = 16000; // 8191; // 速度MAX

            DATA[0] = speed & 0xFF; // 下位バイト
            DATA[1] = (speed >> 8) & 0xFF;        // 上位バイト
            DATA[2] =　speed & 0xFF; // 下位バイト
            DATA[3] = (speed >> 8) & 0xFF;        // 上位バイト
            DATA[4] =　speed & 0xFF; // 下位バイト
            DATA[5] = (speed >> 8) & 0xFF;        // 上位バイト
            pre = now;
        }
        CANMessage msg(DJI_ID, DATA, 8); // 送信
        can.write(msg);
    }
}
