#ifndef __MOTOR_H__
#define __MOTOR_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "main.h"
#include "tim.h"
#include "main.h"

#define FLASH_K_MOTOR 40

    extern int64_t left_encoder;
    extern int64_t right_encoder;
    extern double left_s;
    extern double right_s;

    extern uint64_t last_step_time_r;
    extern uint64_t last_step_time_l;
    extern uint64_t current_step_time_r;
    extern uint64_t current_step_time_l;

    void motor_init();
    void update_encoder(TIM_HandleTypeDef *htim);
    void update_encoders();
    void left_motor(float speed);
    void right_motor(float speed);
    void start_Pwm(TIM_HandleTypeDef *htim, uint32_t channel);
    void analogWrite(TIM_HandleTypeDef *htim, uint32_t channel, int32_t duty_cycle);

#ifdef __cplusplus
}
#endif

#endif
