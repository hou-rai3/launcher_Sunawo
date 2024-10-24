#include "FP.hpp"
#include "mbed.h"
#include "sengen.hpp"

#include "VelPid.hpp"
#include <cmath>
#include <cstdio>

using namespace std::chrono_literals;

constexpr float target_rpm = 5000; // 目標回転数
constexpr float acc_time = 0.60; // 加速時間
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
bool pid_active = false;
float elapsed = 0.0;

float get_actual_rpm_from_encoder() {
    if (can.read(msg)) {
        penguin_1.parse_packet(msg);
        printf("I can get enc: %ld\n", penguin_1[0].get_enc());
        return static_cast<float>(penguin_1[0].get_enc());
    } else {
        return 0.0f;
    }
}

void fire_under_pid_non_blocking() {
    if (!pid_active) {
        pid_active = true;
        elapsed = 0.0;
        auto start_time = HighResClock::now();

        while (elapsed < 0.80) {
            auto now = HighResClock::now();
            elapsed = (now - start_time).count() / 1000000.0; // elapsed time in seconds

            float ref_rpm = target_rpm;
            if (elapsed < acc_time) {
                ref_rpm = target_rpm * std::sin(elapsed * M_PI / acc_time / 2);
            }

            actual_rpm = get_actual_rpm_from_encoder() / 8.0;
            auto output = pid.calc(ref_rpm / target_rpm, actual_rpm / target_rpm, dt);
            output = clamp(1.0 * output, 0.0, 1.0);

            printf(
                "Target RPM: %d\tRef RPM: %5.0f\tActual RPM: %5.0f\tOutput:%4.2f\n",
                (int)target_rpm, ref_rpm, actual_rpm, output);

            penguin_1[0].set_duty(-output);
        }

        penguin_1[0].set_duty(0); // モーターを停止
        pid_active = false;
        printf("end of pid\n");
    }
}

// その他の関数はそのまま使用
void motor_move(int speed) {
    penguin_2[1].set_duty(-speed); // 左
    penguin_2[0].set_duty(speed);  // 右
    printf("motor_move called with speed: %d\n", speed);
}

void can_send() {
    // 既存のCAN送信処理を使用
    auto now_1 = HighResClock::now();
    if (now_1 - pre_1 > 10ms) {
        CANMessage msg1(1, (const uint8_t *)pwm, 8);
        can.write(msg1);
        auto pen1 = penguin_1.to_msg();
        auto pen2 = penguin_2.to_msg();

        if (can.write(pen1) && can.write(pen2)) {
            // 成功メッセージを表示（必要に応じて）
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
        auto now = HighResClock::now();
        if (now - pre > 1000ms && !Flags::Lower_fire && !Flags::fire_rock_L &&
            Flags::Lowre_lock && !pid_active) {
            Flags::fire_rock_L = true;
            Flags::Lowre_lock = false;
            fire_under_pid_non_blocking(); // 非ブロッキングPID制御
            pre = now;
        }
        setup.sw_read();

        button.handle_higher();
        button.handle_lower();
        button.higher_reload();
        button.lower_reload();

        can_send(); // CAN送信を並行して実行
    }
}
