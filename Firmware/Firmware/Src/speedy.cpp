#include "speedy.h"
#include "motor.h"
#include "ir_sensor.h"
#include "math.h"
#include "tim.h"
#include "gpio.h"
#include "adc.h"
#include "loop.h"

#include <inttypes.h>

int64_t left_ref;
int64_t right_ref;

uint64_t ref_time;

float bot_angle;
float bot_distance;

double left_target;
double right_target;

float kp_motor = 0;
float ki_motor = 0;
float kd_motor = 0;

float left_speed = 0;
float right_speed = 0;

int64_t left_steps = 0;
int64_t right_steps = 0;

float acceleration = 200;
float current_acceleration = 0;
float max_speed = 40;

bool is_finished = true;

float left_speed_current = 0;
float right_speed_current = 0;

//+++++++++++++++++++++++++++++++++++++++Update++++++++++++++++++++++++++++++++++++++
void update()
{

    update_encoders();
    if (1)
    {
        static float pre_error_left = 0;
        static float I_left = 0;
        float error_left = 0;

        static float pre_error_right = 0;
        static float I_right = 0;
        float error_right = 0;

        error_left = left_target - left_encoder;
        float D_left = (error_left - pre_error_left);
        pre_error_left = error_left;
        I_left += error_left;

        error_right = right_target - right_encoder;
        float D_right = (error_right - pre_error_right);
        pre_error_right = error_right;
        I_right += error_right;

        int band = 20;

        if (I_left > band)
            I_left = band;
        if (I_left < -band)
            I_left = -band;
        if (I_right > band)
            I_right = band;
        if (I_right < -band)
            I_right = -band;

        float kp_motor = 1;
        float ki_motor = 0.8;
        float kd_motor = 10.1;
        float adj_right = error_right * kp_motor + I_right * ki_motor + D_right * kd_motor;
        float adj_left = error_left * kp_motor + I_left * ki_motor + D_left * kd_motor;
        // float adj = error * kp_motor + I * ki_motor + D * kd_motor;

        (left_speed > max_speed) ? left_speed = max_speed : left_speed;
        (right_speed > max_speed) ? right_speed = max_speed : right_speed;

        float left_motor_speed = (adj_left);
        float right_motor_speed = (adj_right);

        left_motor(left_motor_speed);
        right_motor(right_motor_speed);
    }
    else
    {
        drive(0, 0);
        left_speed_current = 0;
        right_speed_current = 0;
    }

    static uint64_t pT;
    uint32_t nT = micros();

    if (nT - pT >= 1000 && !finished())
    {
        pT = nT;
        left_target += (STEPS_PER_CM * left_speed_current / 1000);
        right_target += (STEPS_PER_CM * right_speed_current / 1000);

        if (current_acceleration == 0)
        {
            left_speed_current = left_speed;
            right_speed_current = right_speed;
            return;
        }

        if (left_speed >= 0)
        {
            if ((left_speed_current * left_speed_current) / (2 * current_acceleration) > left_remaining_dis())
            {
                if (left_speed_current < 0)
                    left_speed_current = 0;
                else
                    left_speed_current -= current_acceleration / 1000;
            }
            else if (left_speed_current < left_speed)
            {
                left_speed_current += current_acceleration / 1000;
            }
        }
        else
        {
            if ((left_speed_current * left_speed_current) / (2 * current_acceleration) > -left_remaining_dis())
            {
                if (left_speed_current > 0)
                    left_speed_current = 0;
                else
                    left_speed_current += current_acceleration / 1000;
            }
            else if (left_speed_current > left_speed)
            {
                left_speed_current -= current_acceleration / 1000;
            }
        }

        if (right_speed >= 0)
        {
            if ((right_speed_current * right_speed_current) / (2 * current_acceleration) > right_remaining_dis())
            {
                if (right_speed_current < 0)
                    right_speed_current = 0;
                else
                    right_speed_current -= current_acceleration / 1000;
            }
            else if (right_speed_current < right_speed)
            {
                right_speed_current += current_acceleration / 1000;
            }
        }
        else
        {
            if ((right_speed_current * right_speed_current) / (2 * current_acceleration) > -right_remaining_dis())
            {
                if (right_speed_current > 0)
                    right_speed_current = 0;
                else
                    right_speed_current += current_acceleration / 1000;
            }
            else if (right_speed_current > right_speed)
            {
                right_speed_current -= current_acceleration / 1000;
            }
        }
    }

    // static uint32_t last_print_time = 0;
    // if (uwTick - last_print_time >= 200)
    // {
    //     last_print_time = uwTick;
    //     printf("%.2f %.2f\r\n", left_speed, left_speed_current);
    // }
}

//+++++++++++++++++++++++++++++++++++END OF UPDATE++++++++++++++++++++++++++++++++++++++++++++++++++++

void go_left(int n)
{
    int pixel = 4;
    stop();
    // forward(2, 1, 0);
    if (n == 1)
    {
        turn_right(500, 0);
        pixel = 4;
    }
    else if (n == 0)
    {
        turn_left(500, 0);
        pixel = 4;
    }
    else if (n == 2)
    {
        stop();
        rotate(500, 0);
        pixel = 6;
    }
    get_reading();
    while (read_pixel(pixel) > 400)
    {
        // get_reading();
        update();
    }
    // printf("leaved\n");
    while (read_pixel(pixel) < 600)
    {
        // get_reading();
        update();
    }
    // line_allign();
    stop();
}

void go_right()
{
    go_left(1);
}

void u_turn()
{
    go_left(2);
}

void init_speedy()
{
    motor_init();
    HAL_TIM_Encoder_Start_IT(&htim1, TIM_CHANNEL_ALL);
    HAL_TIM_Encoder_Start_IT(&htim2, TIM_CHANNEL_ALL);
    HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
}

bool moved_mm(float mm)
{
    static int32_t pre_left = left_encoder;
    static int32_t pre_right = right_encoder;
    update_encoders();
    if (left_encoder + right_encoder - (pre_left + pre_right) > mm * STEPS_PER_CM * 2 / 10)
    {
        pre_left = left_encoder;
        pre_right = right_encoder;
        return true;
    }
    return false;
}

void line_following()
{
    static int pt = uwTick;
    if (uwTick - pt >= 4)
    {
        pt = uwTick;
        float adj = line_deviation();
        float l_speed = max_speed + adj;
        float r_speed = max_speed - adj;
        // if (l_speed < 0)
        //     l_speed = 0;
        // if (r_speed < 0)
        //     r_speed = 0;
        // if (l_speed > max_speed)
        //     l_speed = max_speed;
        // if (r_speed > max_speed)
        //     r_speed = max_speed;
        drive(l_speed, r_speed);
    }
}

void stop(bool brake)
{
    drive(0, 0);
    if (brake)
    {
        forward(0, 1, 0, 0);
        int st = uwTick;
        while (uwTick - st < 200)
        {
            update();
        }
    }
    drive(0, 0);
}

void drive(float motor_A_speed, float motor_B_speed)
{
    left_motor(motor_A_speed);
    right_motor(motor_B_speed);
}

void get_reference()
{
    update_encoders();

    left_ref = left_encoder;
    right_ref = right_encoder;
    left_steps = left_ref;
    right_steps = right_ref;
    left_target = left_ref;
    right_target = right_ref;
    ref_time = uwTick;
    is_finished = false;

    left_speed_current = 0;
    right_speed_current = 0;

    current_acceleration = acceleration;
}

void rotate(float angle, bool complete, bool full)
{
    bot_angle += angle;
    if (angle >= 0)
    {
        left_speed = max_speed;
        right_speed = -max_speed;
    }
    else
    {
        left_speed = -max_speed;
        right_speed = max_speed;
    }
    get_reference();
    left_steps += STEPS_PER_DEGEREE * angle / 2;
    right_steps += -STEPS_PER_DEGEREE * angle / 2;

    if (complete)
    {
        while (!finished())
        {
            update();
        }
        drive(0, 0);
        if (full)
            for (int i = 0; i < 10000; i++)
                update();
    }
}

void turn_right(float angle, bool complete)
{
    get_reference();
    if (angle > 0)
        left_speed = max_speed;
    else
        left_speed = -max_speed;

    right_speed = 0;

    left_steps += STEPS_PER_DEGEREE * angle;
    right_steps = 0;
    if (complete)
    {
        while (!finished())
        {
            update();
        }
        for (int i = 0; i < 10000; i++)
            update();
    }
}

void turn_left(float angle, bool complete)
{
    get_reference();

    left_speed = 0;
    if (angle > 0)
        right_speed = max_speed;
    else
        right_speed = -max_speed;

    left_steps = 0;
    right_steps += STEPS_PER_DEGEREE * angle;
    if (complete)
    {
        while (!finished())
        {
            update();
        }
        drive(0, 0);
    }
    for (int i = 0; i < 1000; i++)
        update();
}

void forward(float distance, bool complete, bool full, float acceleration)
{
    get_reference();
    current_acceleration = acceleration;
    if (distance >= 0)
    {
        left_speed = max_speed;
        right_speed = max_speed;
    }
    else
    {
        left_speed = -max_speed;
        right_speed = -max_speed;
    }
    left_steps += STEPS_PER_CM * distance;
    right_steps += STEPS_PER_CM * distance;

    if (complete)
    {
        while (!finished())
        {
            update();
        }
        drive(0, 0);
        if (full)
            for (int i = 0; i < 10000; i++)
                update();
    }
}

float left_remaining_dis()
{
    return ((left_steps - left_s) * 1.0 / STEPS_PER_CM);
}

float right_remaining_dis()
{
    return ((right_steps - right_s) * 1.0 / STEPS_PER_CM);
}

bool finished()
{
    // bool is_finished = false;
    int64_t left_rem = left_steps - left_encoder;
    int64_t right_rem = right_steps - right_encoder;

#define FINISH_THRESHOLD 0
    if (left_speed > 0 && left_rem < FINISH_THRESHOLD)
        is_finished = true;
    if (right_speed > 0 && right_rem < FINISH_THRESHOLD)
        is_finished = true;
    if (left_speed < 0 && left_rem > -FINISH_THRESHOLD)
        is_finished = true;
    if (right_speed < 0 && right_rem > -FINISH_THRESHOLD)
        is_finished = true;

    return is_finished;
}
