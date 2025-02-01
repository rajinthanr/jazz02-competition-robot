
#ifndef SPEEDY_H
#define SPEEDY_H

#ifdef __cplusplus
extern "C"
{
#endif

#define STEPS_PER_CM 180
#define STEPS_PER_DEGEREE 34.3
#include "main.h"

    extern float max_speed;
    extern float target_speed;
    extern float bot_speed;
    extern float bot_angle;
    extern float acceleration;

    extern float kp_motor;
    extern float ki_motor;
    extern float kd_motor;

    extern int64_t left_ref;
    extern int64_t right_ref;
    extern int64_t left_steps;
    extern int64_t right_steps;

    extern double left_target;
    extern double right_target;

    void init_speedy();
    void line_following();
    void get_reference();
    bool finished();
    void update();
    void drive(float motor_A_speed, float motor_B_speed);
    void forward(float distance, bool complete = true, bool full = 1, float acceleration = acceleration);
    void turn_right(float angle, bool complete = 1);
    void turn_left(float angle, bool complete = 1);
    void stop(bool brake = 1);
    void rotate(float angle, bool complete = 1, bool full = 1);
    bool moved_mm(float mm = 1);
    void go_left(int n = 0);
    void go_right();
    void u_turn();
    float left_remaining_dis();
    float right_remaining_dis();

#ifdef __cplusplus
}
#endif

#endif