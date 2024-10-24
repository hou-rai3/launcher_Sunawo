#ifndef SENGEN_HPP
#define SENGEN_HPP
#include <algorithm> // std::clampを使用するために必要
#include <mbed.h>

class Flags {
  public:
    static bool Higher_fire;
    static bool Lower_fire;
    static bool Higher_flag;
    static bool Lower_flag;
    static bool Higher_stop;
    static bool Lower_stop;
    static bool Higher_Reload;
    static bool Lower_Reload;
    static bool mode1;
    static bool mode2;
    static bool Higher_lock;
    static bool Lowre_lock;
    static bool Higher_R_lock;
    static bool Lowre_R_lock;
    static bool mode1_R;
    static bool mode2_R;
    static bool fire_rock;
    static bool fire_rock_L;
};

class Buttons {
  public:
    static InterruptIn Higher_fire;
    static InterruptIn Lower_fire;
    static InterruptIn Higher_stop;
    static InterruptIn Lower_stop;
    static InterruptIn Higher_Reload;
    static InterruptIn Lower_Reload;
    static InterruptIn mode1;
    static InterruptIn mode2;
};

class ints {
  public:
    static int Higher_fire;
    static int Lower_fire;
    static int Higher_Reload;
    static int Lower_Reload;
    static int Higher_speed;
    static int Lower_speed;
    static int Higher_count;
    static int Lower_count;
    static int output_int16_zero;
    static int signed_speed;
    static int speed;
    static int fire_rock;
    static int fire_rock_L;
};

// Flags
bool Flags::Higher_fire = false;
bool Flags::Lower_fire = false;
bool Flags::Higher_flag = false;
bool Flags::Lower_flag = false;
bool Flags::Higher_stop = false;
bool Flags::Lower_stop = false;
bool Flags::Higher_Reload = false;
bool Flags::Lower_Reload = false;
bool Flags::mode1 = false;
bool Flags::mode2 = false;
bool Flags::Higher_lock = false;
bool Flags::Lowre_lock = false;
bool Flags::Higher_R_lock = false;
bool Flags::Lowre_R_lock = false;
bool Flags::mode1_R = false;
bool Flags::mode2_R = false;
bool Flags::fire_rock = false;
bool Flags::fire_rock_L = false;

// Buttons
InterruptIn Buttons::Higher_Reload(PC_0);
InterruptIn Buttons::Lower_Reload(PC_1);
InterruptIn Buttons::Higher_stop(PC_2);
InterruptIn Buttons::Lower_stop(PC_3);
InterruptIn Buttons::Higher_fire(PC_4);
InterruptIn Buttons::Lower_fire(PC_9);
InterruptIn Buttons::mode1(PC_8);
InterruptIn Buttons::mode2(PC_5);

// ints
int ints::Lower_fire = 0;
int ints::Higher_fire = 0;
int ints::Higher_speed = 20400;
int ints::Lower_speed = 30000;
int ints::Higher_Reload = 0;
int ints::Lower_Reload = 0;
int ints::Higher_count = 0;
int ints::Lower_count = 0;
int ints::output_int16_zero = 0;
int ints::signed_speed = 0;
int ints::speed = 10000;
int ints::fire_rock = 0;
int ints::fire_rock_L = 0;

// timer
auto pre = HighResClock::now();
auto pre_1 = HighResClock::now();

// 通信
BufferedSerial pc(USBTX, USBRX, 115200);
// can
CAN can(PA_11, PA_12, (int)1e6);
CAN can2(PB_12, PB_13, (int)1e6);
CANMessage msg;
CANMessage msgC610;
constexpr uint32_t FP_1 = 35;
constexpr uint32_t FP_2 = 40;
FirstPenguinArray penguin_1(FP_1);
FirstPenguinArray penguin_2(FP_2);
uint16_t Data[8] = {};
uint8_t C610_1[8] = {};
uint8_t C610_2[8] = {};
int16_t pwm[4] = {0, 0, 0, 0}; // pwm配列

class Setup {
  public:
    void ready() {
        Flags::Higher_lock = true;
        Flags::Lowre_lock = true;
        Flags::Higher_R_lock = true;
        Flags::Lowre_R_lock = true;
        Buttons::mode1.mode(PullUp);
        Buttons::mode2.mode(PullUp);
        Buttons::Higher_stop.mode(PullUp);
        Buttons::Lower_stop.mode(PullUp);
        Buttons::Higher_Reload.mode(PullUp);
        Buttons::Lower_Reload.mode(PullUp);
        Buttons::Higher_fire.mode(PullUp);
        Buttons::Lower_fire.mode(PullUp);
    }

    void sw_read() {
        Flags::Higher_stop = Buttons::Higher_stop.read();
        Flags::Lower_stop = Buttons::Lower_stop.read();
        Flags::Higher_Reload = Buttons::Higher_Reload.read();
        Flags::Lower_Reload = Buttons::Lower_Reload.read();
        Flags::Higher_fire = Buttons::Higher_fire.read();
        Flags::Lower_fire = Buttons::Lower_fire.read();
        Flags::mode1 = Buttons::mode1.read();
        Flags::mode2 = Buttons::mode2.read();
    }
};

class Button {
  public:
    void handle_higher() {
        ints::Higher_count += !Flags::Higher_stop ? 1 : -1;
        ints::Higher_count =
            std::clamp(ints::Higher_count, 0, 10); // std::clampを使用
        // printf("Higher_count: %d\n", ints::Higher_count);
        if (ints::Higher_count == 7 && !Flags::Higher_flag &&
            !Flags::Higher_lock) {
            Flags::Higher_R_lock = false;
            Flags::Higher_flag = true;
            printf("handle_h\n");
            motor_move(0.1);
        }
        if (ints::Higher_count == 0) {
            Flags::Higher_flag = false;
        }
    }

    void handle_lower() {
        ints::Lower_count += !Flags::Lower_stop ? 1 : -1;
        ints::Lower_count = std::clamp(ints::Lower_count, 0, 4);

        if (ints::Lower_count == 4 && !Flags::Lower_flag &&
            !Flags::Lowre_lock) {
            Flags::Lowre_R_lock = false;
            Flags::Lower_flag = true;
            fire_under(0.1);
            printf("handle_L\n");
        }
        if (ints::Lower_count == 0) {
            Flags::Lower_flag = false;
        }
    }

    void higher_reload() {
        ints::Higher_Reload += !Flags::Higher_Reload ? 1 : -1;
        ints::Higher_Reload = std::clamp(ints::Higher_Reload, 0, 20);
        if (ints::Higher_Reload == 20 && !Flags::Higher_Reload &&
            !Flags::Higher_R_lock) {
            Flags::Higher_lock = true;
            Flags::Higher_Reload = true;
            motor_move(0);
            printf("h_reload\n");
        }
        if (ints::Higher_Reload == 0) {
            Flags::Higher_Reload = false;
        }
    }

    void lower_reload() {
        ints::Lower_Reload += !Flags::Lower_Reload ? 1 : -1;
        ints::Lower_Reload = std::clamp(ints::Lower_Reload, 0, 3);
        if (ints::Lower_Reload == 3 && !Flags::Lower_Reload &&
            !Flags::Lowre_R_lock) {
            Flags::Lowre_lock = true;
            Flags::Lower_Reload = true;
            fire_under(0);
            printf("L_reload\n");
        }
        if (ints::Lower_Reload == 0) {
            Flags::Lower_Reload = false;
        }
    }

  private:
    void motor_move(int speed) {
        penguin_2[1].set_duty(-speed); // 左
        penguin_2[0].set_duty(speed);  // 右
        printf("motor_move called with speed: %d\n", speed);
    }

    void fire_under(int speed) {
        penguin_1[0].set_duty(-speed); // モーターを動かす処理をここに記述
        printf("fire_under called with speed: %d\n", speed);
    }
};

typedef struct {
    signed char LX;
    signed char LY;
    signed char RX;
    signed char RY;

    unsigned char Circle : 1;
    unsigned char Cross : 1;
    unsigned char Square : 1;
    unsigned char Triangle : 1;

    unsigned char Up : 1;
    unsigned char Right : 1;
    unsigned char Down : 1;
    unsigned char Left : 1;

    unsigned char L2 : 1;
    unsigned char R2 : 1;
    unsigned char L1 : 1;
    unsigned char R1 : 1;

    unsigned char L3 : 1;
    unsigned char R3 : 1;
    unsigned char Start : 1;
    unsigned char Select : 1;

} PS2Con;

#endif // SENGEN_HPP
