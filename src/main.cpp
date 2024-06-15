#include "mbed.h"
#include "PID.hpp"

InterruptIn button(PC_8);       // リミットスイッチ
InterruptIn button_reset(PC_9); // 起動スイッチ
int DJI_ID = 0x200;

CAN can(PA_11, PA_12, (int)1e6);
volatile int32_t encoder_position = 0;

void reset_can()
{
    can.reset();
    ThisThread::sleep_for(100ms); // リセット後に少し待つ
}

int main()
{
    uint8_t DATA[8] = {0};
    int16_t speed = 0;
    bool speed_set = false; // フラグを追加
    Timer debounce_timer;   // デバウンス用のタイマー

    button_reset.mode(PullUp);
    BufferedSerial pc(USBTX, USBRX, 115200);
    auto pre = HighResClock::now();
    auto pre_1 = pre;

    debounce_timer.start(); // デバウンス用タイマーを開始

    while (1)
    {
        auto now = HighResClock::now();
        bool sw = button_reset.read();

        // ボタンのデバウンス処理（ボタンが押されてから50ms安定するまで待つ）
        if (sw == 0 && debounce_timer.elapsed_time() > 50ms)
        {
            debounce_timer.reset(); // タイマーをリセットして次のデバウンス処理に備える
        }

        if (speed_set && now - pre > 120ms)
        {
            speed = 0; // 速度0
            for (int i = 0; i < 8; i += 2)
            {
                DATA[i] = (speed >> 8) & 0xFF; // 上位バイト
                DATA[i + 1] = speed & 0xFF;    // 下位バイト
            }
            speed_set = false; // フラグをfalseにする
        }

        else if (!speed_set && now - pre > 1000ms && sw == 0)
        {
            speed = 15000;                  // 速度MAX
            DATA[0] = -(speed >> 8) & 0xFF; // 上位バイト
            DATA[1] = speed & 0xFF;         // 下位バイト
            DATA[2] = (speed >> 8) & 0xFF;  // 上位バイト
            DATA[3] = speed & 0xFF;         // 下位バイト
            DATA[4] = -(speed >> 8) & 0xFF; // 上位バイト
            DATA[5] = speed & 0xFF;         // 下位バイト
            DATA[6] = (speed >> 8) & 0xFF;  // 上位バイト
            DATA[7] = speed & 0xFF;         // 下位バイト
            speed_set = true;               // フラグをtrueにする
            pre = now;                      // タイマーリセット
        }

        if (now - pre_1 > 10ms)
        {
            CANMessage msg(DJI_ID, DATA, 8);
            if (can.write(msg))
            {
                printf("OK\n");
            }
            else
            {
                printf("Can't send Message\n");
                // 送信失敗時の追加デバッグ情報
                printf("CAN Bus Error Status: %d\n", can.rderror());
                printf("CAN Bus Write Error Count: %d\n", can.tderror());
                if (can.rderror() == 255 || can.tderror() == 249)
                {
                    printf("Resetting CAN controller\n");
                    reset_can(); // CANコントローラをリセット
                }
            }
            pre_1 = now;
        }
    }
}
