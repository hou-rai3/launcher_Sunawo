#include "FP.hpp"
#include "mbed.h"
#include "sengen.hpp"

#include "VelPid.hpp"
#include <cmath>
#include <cstdio>

using namespace std::chrono_literals;

constexpr float target_rpm = 5000; // 目標回転数
constexpr float acc_time = 0.60; // 加速時間ちょっといじるのもありかも
constexpr PidGain pid_gain{
    0.1,  // P
    0.01, // I
    0.00, // D
};
constexpr float limit = 1.0; // 出力制限値 (duty)
constexpr auto dt = 0.060s;  // 制御周期

VelPid pid{{pid_gain, -limit, limit}};
float actual_rpm = 0;
float encoder_value = 0;
float now_actual_rpm = 0;
float pre_actual_rpm = 0;

float get_actual_rpm_from_encoder() {
    if (can.read(msg)) {
        penguin_1.parse_packet(msg);
        printf("enc: %ld\n", penguin_1[0].get_enc());
    }
    return static_cast<float>(penguin_1[0].get_enc());
}

void motor_move(int speed) {
    penguin_2[1].set_duty(-speed); // 左
    penguin_2[0].set_duty(speed);  // 右
    printf("motor_move called with speed: %d\n", speed);
}

void fire_under_pid() {
    printf("fire_under_pid\n");
    float elapsed = 0.00;
    while (elapsed < 0.80) {
        float ref_rpm = target_rpm;
        if (elapsed < acc_time) {
            // 目標回転数をsin波で変化させる
            ref_rpm = target_rpm * std::sin(elapsed * M_PI / acc_time / 2);
        }

        actual_rpm = get_actual_rpm_from_encoder();

        // PIDで計算なう
        auto output =
            pid.calc(ref_rpm / target_rpm, actual_rpm / target_rpm, dt);
        output = clamp(1.0 * output, 0.0, 1.0);

        // 値の監視
        printf(
            "Target RPM: %d\tRef RPM: %5.0f\tActual RPM: %5.0f\tOutput:%4.2f\n",
            (int)target_rpm, ref_rpm, actual_rpm, output);
        elapsed += dt.count();

        penguin_1[0].set_duty(-output);
    }
    printf("end of pid\n");
    // penguin_1[0].set_duty(-0.1);
}

void fire_under(int speed) {
    penguin_1[0].set_duty(-speed);
    // printf("motor_move called with speed: %d\n", speed);
}

// void controler() {

//     if (can2.read(msg) && msg.id == 0x7ff) {
//         PS2Con controller;

//         controller.Circle = (msg.data[4] >> 0) & 0x01;
//         controller.Cross = (msg.data[4] >> 1) & 0x01;
//         controller.Square = (msg.data[4] >> 2) & 0x01;
//         controller.Triangle = (msg.data[4] >> 3) & 0x01;
//         controller.Up = (msg.data[4] >> 4) & 0x01;
//         controller.Right = (msg.data[4] >> 5) & 0x01;
//         controller.Down = (msg.data[4] >> 6) & 0x01;
//         controller.Left = (msg.data[4] >> 7) & 0x01;

//         controller.L2 = (msg.data[5] >> 0) & 0x01;
//         controller.R2 = (msg.data[5] >> 1) & 0x01;
//         controller.L1 = (msg.data[5] >> 2) & 0x01;
//         controller.R1 = (msg.data[5] >> 3) & 0x01;
//         controller.L3 = (msg.data[5] >> 4) & 0x01;
//         controller.R3 = (msg.data[5] >> 5) & 0x01;
//         controller.Start = (msg.data[5] >> 6) & 0x01;
//         controller.Select = (msg.data[5] >> 7) & 0x01;
//         // printf("|Circle: %3d|Cross: %3d|Square: %3d|Triangle: % 3d | Up: %
//         3d ""| Down: % 3d | Right: % 3d | left: % 3d | R1: % 3d | L1: % 3d
//         ""| R2: % 3d | L2: % 3d |\n ",
//         //        controller.Circle, controller.Cross, controller.Square,
//         //        controller.Triangle, controller.Up, controller.Down,
//         //        controller.Right, controller.Right, controller.R1,
//         controller.L1,
//         //        controller.R2, controller.L2);

//         ////////////////////////////////////////////////////////////////////////////////////////////////////////////
//         if (controller.L1 == 1) {
//             ints::signed_speed = static_cast<int16_t>(2000);
//             C610_1[4] = -ints::signed_speed & 0xFF;       // 下位バイト
//             C610_1[5] = (ints::signed_speed >> 8) & 0xFF; // 上位バイト
//         }
//         if (controller.L1 == 0 && controller.L2 == 0) {
//             C610_1[4] = ints::output_int16_zero & 0xFF; // 下位バイト
//             C610_1[5] = (ints::output_int16_zero >> 8) & 0xFF; // 上位バイト
//         }
//         if (controller.L2 == 1) {
//             ints::signed_speed = static_cast<int16_t>(2000);
//             C610_1[4] = ints::signed_speed & 0xFF;         // 下位バイト
//             C610_1[5] = -(ints::signed_speed >> 8) & 0xFF; // 上位バイト
//         }
//         //////////////////////////////////////////////////////////////////////////////////////////////////////////////
//         if (controller.R1 == 1) {
//             ints::signed_speed = static_cast<int16_t>(1000);
//             C610_1[6] = ints::signed_speed & 0xFF;         // 下位バイト
//             C610_1[7] = -(ints::signed_speed >> 8) & 0xFF; // 上位バイト
//         }
//         if (controller.R1 == 0 && controller.R2 == 0) {
//             C610_1[6] = ints::output_int16_zero & 0xFF; // 下位バイト
//             C610_1[7] = (ints::output_int16_zero >> 8) & 0xFF; // 上位バイト
//         }
//         if (controller.R2 == 1) {
//             ints::signed_speed = static_cast<int16_t>(1000);
//             C610_1[6] = -ints::signed_speed & 0xFF;       // 下位バイト
//             C610_1[7] = (ints::signed_speed >> 8) & 0xFF; // 上位バイト
//         }
//         //////////////////////////////////////////////////////////////////////////////////////////////////////////////
//         auto now = HighResClock::now();
//         ints::fire_rock += controller.Circle ? 1 : -1;
//         ints::fire_rock = clamp(ints::fire_rock, 0, 4);
//         if (now - pre > 1000ms && ints::fire_rock == 4 && !Flags::fire_rock
//         &&
//             Flags::Higher_lock) {
//             Flags::fire_rock = true;
//             Flags::Higher_lock = false;
//             motor_move(ints::Higher_speed);
//             pre = now;
//         }
//         if (ints::fire_rock == 0) {
//             Flags::fire_rock = false;
//         }
//         ints::fire_rock_L += controller.Square ? 1 : -1;
//         ints::fire_rock_L = clamp(ints::fire_rock_L, 0, 4);
//         if (now - pre > 1000ms && ints::fire_rock_L == 4 &&
//             !Flags::fire_rock_L && Flags::Lowre_lock) {
//             Flags::fire_rock_L = true;
//             Flags::Lowre_lock = false;
//             fire_under_pid();
//             pre = now;
//         }
//         if (ints::fire_rock_L == 0) {
//             Flags::fire_rock_L = false;
//         }
//         if (now - pre > 500ms && controller.Triangle == 1) {
//             ints::Lower_speed += 50;
//             pre = now;
//         }
//         if (now - pre > 500ms && controller.Cross == 1) {
//             ints::Lower_speed -= 50;
//             pre = now;
//         }
//     }
// }

void can_send() {
    auto now_1 = HighResClock::now();
    if (now_1 - pre_1 > 10ms) {

        // printf("Higher_Reload = %d | Lower_Reload = %d | Higher_stop = %d |
        // Lower_stop = %d\n", Flags::Higher_Reload, Flags::Lower_Reload,
        // Flags::Higher_stop, Flags::Lower_stop); 発射出力
        // printf("ints::Higher_speed %d | ints::Lower_speed %d\n",
        // ints::Higher_speed, ints::Lower_speed); テスト発射スイッチ確認
        // printf("H_fire:%d L_fire:%d\n", Flags::Higher_fire,
        // Flags::Lower_fire); チャタリング確認 // printf("Higher_count : %1d |
        // Lower_count : %2d | Higher_Reload : %d | Lower_Reload : %1d\n",
        // ints::Higher_count, ints::Lower_count, ints::Higher_Reload,
        // ints::Lower_Reload); printf("ints::Higher_speed %d |
        // ints::Lower_speed %d\n", ints::Higher_speed, ints::Lower_speed);
        CANMessage msg1(1, (const uint8_t *)pwm, 8);
        can.write(msg1);
        auto pen1 = penguin_1.to_msg();
        auto pen2 = penguin_2.to_msg();
        // if (can.read(msg)) {
        //     penguin_1.parse_packet(msg);
        //     long encoder_value = penguin_1[0].get_enc();
        //     printf("enc: %ld\n", encoder_value);
        // }
        if (can.write(pen1) && can.write(pen2)) {
            // printf("can OK   spend:10ms   enc: %ld\n",
            // penguin_1[0].get_enc());

        } else {
            printf("can't Message\n");
        }
        pre_1 = now_1;
    }
}

int main() {
    Button button;
    Setup setup;

    setup.ready();
    while (1) {
        // if (can.read(msg))
        // {
        //     penguin_1.parse_packet(msg);
        //     printf("enc: %ld\n", penguin_1[0].get_enc());
        // }
        auto now = HighResClock::now();
        if (now - pre > 1000ms && !Flags::Lower_fire && !Flags::fire_rock_L &&
            Flags::Lowre_lock) {
            Flags::fire_rock_L = true;
            Flags::Lowre_lock = false;
            fire_under_pid();
            pre = now;
        }
        setup.sw_read();

        button.handle_higher();
        button.handle_lower();
        button.higher_reload();
        button.lower_reload();

        can_send();
    }
}
