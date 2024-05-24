#include "mbed.h"

uint8_t DATA_1[8] = {};
uint8_t DATA_2[8] = {};
uint8_t DATA_3[8] = {};
InterruptIn button(D4);          // リミットスイッチ
InterruptIn button_reset(PC_13); // 起動スイッチ
BufferedSerial pc(USBTX, USBRX, 115200);
CAN can(PA_11, PA_12, (int)1e6);
int DJI_CAN = 0x200;

int16_t motor_1 = 0;
int16_t motor_2 = 0;
int16_t motor_3 = 0;
int16_t speed = 0; // 16ビットの最大値

int reset;
int sw;

void Switch_Reset()
{
    printf("Restart\n");
    speed = 30000;
    motor_1 = speed;
    motor_2 = speed;
    int16_t output_1 = motor_1;
    int16_t output_2 = motor_2;
    int16_t output_3 = motor_3;
    DATA_1[0] = -output_1 & 0xFF;        // 下位バイト
    DATA_1[1] = (-output_1 >> 8) & 0xFF; // 上位バイト
    DATA_2[2] = output_2 & 0xFF;         // 下位バイト
    DATA_2[3] = (output_2 >> 8) & 0xFF;  // 上位バイト
    DATA_3[4] = output_3 & 0xFF;         // 下位バイト
    DATA_3[5] = (output_3 >> 8) & 0xFF;  // 上位バイト
}

void Switch_Stop()
{
    printf("Stop\n");
    speed = 0;
    motor_1 = speed;
    motor_2 = speed;

    int16_t output_1 = motor_1;
    int16_t output_2 = motor_2;
    int16_t output_3 = motor_3;
    DATA_1[0] = output_1 & 0xFF;        // 下位バイト
    DATA_1[1] = (output_1 >> 8) & 0xFF; // 上位バイト
    DATA_2[2] = output_2 & 0xFF;        // 下位バイト
    DATA_2[3] = (output_2 >> 8) & 0xFF; // 上位バイト
    DATA_3[4] = output_3 & 0xFF;        // 下位バイト
    DATA_3[5] = (output_3 >> 8) & 0xFF; // 上位バイト
}

int main()
{
    button.mode(PullUp);
    button_reset.mode(PullUp);

    button_reset.fall(&Switch_Reset);
    button.fall(&Switch_Stop);

    while (1)
    {
        CANMessage msg0(DJI_CAN, DATA_1, 8);
        CANMessage msg1(DJI_CAN, DATA_2, 8);
        CANMessage msg2(DJI_CAN, DATA_3, 8);
        can.write(msg0);
        can.write(msg1);
        can.write(msg2);
    }
}
