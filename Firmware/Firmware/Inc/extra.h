#ifndef EXTRA_H
#define EXTRA_H

#ifdef __cplusplus
#include "loop.h"
extern "C"
{
#endif

    extern bool got_ultrasonic_reading;
    extern bool got_tof_reading;
    extern float ultrasonic_reading;
    extern float tof_reading;
    extern bool button_1;
    extern bool button_2;

    void setup_servos();
    uint32_t angle_to_pulse(int angle);
    void set_servo_position(int servo_num, int angle);
    void servo1(int angle);
    void servo2(int angle);
    void setup_leds();
    void led1(int duty_cycle);
    void led2(int duty_cycle);
    void led3(int duty_cycle);
    bool isWhiteDetected();
    bool detectWhiteToBlackTransition();
    float get_dis_cm();
    void drop_coin();
    void request_ultrasonic_reading();
    void request_tof_reading();

#ifdef __cplusplus
}
#endif

#endif // EXTRA_H