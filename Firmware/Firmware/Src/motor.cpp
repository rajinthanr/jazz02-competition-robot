#include "motor.h"
#include "loop.h"
#include <cstdlib>
#include <string>
#include "ir_sensor.h"

using namespace std;

int64_t left_encoder;
int64_t right_encoder;
double left_s;
double right_s;

uint64_t last_step_time_r;
uint64_t last_step_time_l;
uint64_t current_step_time_r;
uint64_t current_step_time_l;

void left_motor(float speed);
void right_motor(float speed);
bool finished();

void motor_init()
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
}

void start_Pwm(TIM_HandleTypeDef *htim, uint32_t channel)
{
    HAL_TIM_PWM_Start(htim, channel);
}

void analogWrite(TIM_HandleTypeDef *htim, uint32_t channel, int32_t duty_cycle)
{
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.Pulse = 0;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.Pulse = duty_cycle;
    HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, channel);
}

void left_motor(float speed)
{
    speed = -speed;
    speed = (speed < -100.0f) ? -100.0f : (speed > 100.0f ? 100.0f : speed);
    uint32_t duty_cycle = (uint32_t)(abs(speed) * 1023.0 / 100);

    if (speed >= 0)
    {
        analogWrite(&htim3, TIM_CHANNEL_1, duty_cycle);
        analogWrite(&htim3, TIM_CHANNEL_2, 0);
    }
    else if (speed < 0)
    {
        analogWrite(&htim3, TIM_CHANNEL_1, 0);
        analogWrite(&htim3, TIM_CHANNEL_2, duty_cycle);
    }
}

void right_motor(float speed)
{
    speed = speed;
    speed = (speed < -100.0f) ? -100.0f : (speed > 100.0f ? 100.0f : speed);
    uint32_t duty_cycle = (uint32_t)(abs(speed) * 1023.0 / 100);

    if (speed >= 0)
    {
        analogWrite(&htim3, TIM_CHANNEL_3, duty_cycle);
        analogWrite(&htim3, TIM_CHANNEL_4, 0);
    }
    else if (speed < 0)
    {
        analogWrite(&htim3, TIM_CHANNEL_3, 0);
        analogWrite(&htim3, TIM_CHANNEL_4, duty_cycle);
    }
}

void update_encoder(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
        static uint16_t pre_count = 0;
        static int64_t round = 0;
        static int64_t last_count = 0;
        static float acceleration;
        uint16_t count = __HAL_TIM_GET_COUNTER(&htim1);

        if (count == pre_count)
        {
            static float last_interval = 0;
            static float pulse_interval = 0;
            last_interval = pulse_interval;
            uint32_t now = micros();                                   // Get current time
            float time_since_last_pulse = (now - current_step_time_l); // Time in useconds
            pulse_interval = (current_step_time_l - last_step_time_l); // Interval in useconds

            if (pulse_interval > 0 && time_since_last_pulse < 500000)
            {
                // Calculate speed in steps per second
                static float speed;
                static float last_speed = 0;
                last_speed = speed;
                speed = (left_encoder - last_count) / pulse_interval;
                acceleration = (speed - last_speed) / last_interval;

                // Interpolate position
                left_s = left_encoder + (speed * time_since_last_pulse) +                       // Linear interpolation
                         (0.5f * acceleration * time_since_last_pulse * time_since_last_pulse); // Acceleration term
            }
            else
            {
                // No pulses detected recently; assume position is the last known step
                left_s = -(round * 10000 + count);
            }
        }
        else
        {
            if (abs(count - pre_count) > 9000)
            {
                if (count > 5000)
                    round--;
                else
                    round++;
            }

            last_step_time_l = current_step_time_l;
            current_step_time_l = micros();
            pre_count = count;
            last_count = left_encoder;
            left_encoder = (round * 10000 + count);
            left_s = -(round * 10000 + count);
        }
    }
    if (htim->Instance == TIM2)
    {
        static uint16_t pre_count = 0;
        static int64_t round = 0;
        static int64_t last_count = 0;
        static float acceleration;
        uint16_t count = __HAL_TIM_GET_COUNTER(&htim2);

        if (count == pre_count)
        {
            static float last_interval = 0;
            static float pulse_interval = 0;
            last_interval = pulse_interval;
            uint32_t now = micros();                                   // Get current time
            float time_since_last_pulse = (now - current_step_time_r); // Time in useconds
            pulse_interval = (current_step_time_r - last_step_time_r); // Interval in useconds

            if (pulse_interval > 0 && time_since_last_pulse < 500000)
            {
                // Calculate speed in steps per second
                static float speed;
                static float last_speed = 0;
                last_speed = speed;
                speed = (right_encoder - last_count) / pulse_interval;
                acceleration = (speed - last_speed) / last_interval;

                // Interpolate position
                right_s = right_encoder + (speed * time_since_last_pulse) +                      // Linear interpolation
                          (0.5f * acceleration * time_since_last_pulse * time_since_last_pulse); // Acceleration term
            }
            else
            {
                // No pulses detected recently; assume position is the last known step
                right_s = (round * 10000 + count);
            }
        }
        else
        {
            if (abs(count - pre_count) > 9000)
            {
                if (count > 5000)
                    round--;
                else
                    round++;
            }

            last_step_time_r = current_step_time_r;
            current_step_time_r = micros();
            pre_count = count;
            last_count = right_encoder;
            right_encoder = -(round * 10000 + count);
            right_s = (round * 10000 + count);
        }
    }
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    // update_encoder(htim);
    // LED_TOGGLE
}

void update_encoders()
{
    update_encoder(&htim1);
    update_encoder(&htim2);
}
