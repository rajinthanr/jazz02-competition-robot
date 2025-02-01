#include "extra.h"
#include "loop.h"

// Servo parameters (adjust based on your servo specs)
#define SERVO_MIN_PULSE 400  // Minimum pulse width in microseconds
#define SERVO_MAX_PULSE 2400 // Maximum pulse width in microseconds
#define SERVO_PERIOD 20000   // Servo period in microseconds (50Hz)

bool got_ultrasonic_reading = false;
bool got_tof_reading = false;
float ultrasonic_reading = 0;
float tof_reading = 0;
bool button_1 = false;
bool button_2 = false;

void setup_servos()
{
    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2);
    setup_leds();
}

void setup_leds()
{
    start_Pwm(&htim4, TIM_CHANNEL_4);
    start_Pwm(&htim8, TIM_CHANNEL_4);
    start_Pwm(&htim8, TIM_CHANNEL_3);
}

void led1(int duty_cycle)
{
    analogWrite(&htim4, TIM_CHANNEL_4, duty_cycle);
}

void led2(int duty_cycle)
{
    analogWrite(&htim8, TIM_CHANNEL_4, duty_cycle);
}

void led3(int duty_cycle)
{
    analogWrite(&htim8, TIM_CHANNEL_3, duty_cycle);
}

// Map angle to pulse width
uint32_t angle_to_pulse(int angle)
{
    return SERVO_MIN_PULSE + ((SERVO_MAX_PULSE - SERVO_MIN_PULSE) * angle) / 180;
}

// Set servo position
void set_servo_position(int servo_num, int angle)
{
    uint32_t pulse = angle_to_pulse(angle);
    TIM_OC_InitTypeDef sConfigOC = {0};
    sConfigOC.OCMode = TIM_OCMODE_PWM1;
    sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    sConfigOC.Pulse = pulse;
    if (servo_num == 1)
        HAL_TIM_PWM_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_1);
    else if (servo_num == 2)
        HAL_TIM_PWM_ConfigChannel(&htim12, &sConfigOC, TIM_CHANNEL_2);
}

void servo1(int angle)
{
    set_servo_position(1, angle);
}

void servo2(int angle)
{
    set_servo_position(2, angle);
}

#define NUM_SENSORS 8

bool isWhiteDetected()
{
    int whiteCount = 0;
    for (int i = 0; i < 8; i++)
    {
        if (read_pixel(i) > 500)
        {
            whiteCount++;
        }
    }
    return (whiteCount > NUM_SENSORS / 2);
}

bool detectWhiteToBlackTransition()
{
    static bool previousWhiteDetected = false;
    bool currentWhiteDetected = isWhiteDetected();

    if (previousWhiteDetected && !currentWhiteDetected)
    {
        previousWhiteDetected = currentWhiteDetected;
        return true;
    }

    previousWhiteDetected = currentWhiteDetected;
    return false;
}

float get_dis_cm()
{
    update_encoders();
    static float dis;
    dis = ((left_encoder + right_encoder) / 2) * 1.0 / STEPS_PER_CM;
    return dis;
}

void drop_coin()
{
    for (int i = 0; i < 4; i++)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
        delay(100);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
        delay(300);
    }
}

void request_ultrasonic_reading()
{
    got_ultrasonic_reading = false;
    char buffer[2] = "G";
    HAL_UART_Transmit(&huart3, (uint8_t *)buffer, 2, 1000);
}

void request_tof_reading()
{
    got_tof_reading = false;
    char buffer[2] = "T";
    HAL_UART_Transmit(&huart3, (uint8_t *)buffer,2, 1000);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if (GPIO_Pin == GPIO_PIN_13)
    {
        LED_TOGGLE
        button_1 = true;
    }
    if (GPIO_Pin == GPIO_PIN_1){
        LED_TOGGLE
        button_2 = true;
    }
}

void fade(int led)
{
    for (int i = 0; i < 100; i++)
    {
        if (led == 1)
            led1(i);
        if (led == 2)
            led2(i);
        if (led == 3)
            led3(i);
        delay(10);
    }
    for (int i = 100; i > 0; i--)
    {
        if (led == 1)
            led1(i);
        if (led == 2)
            led2(i);
        if (led == 3)
            led3(i);
        delay(10);
    }
}

void open_hand(){
    servo1(120);
}

void close_hand(){
    servo1(30);
}

void raise_hand(){
    servo2(10);
}

void lower_hand(){
    servo2(100);
}

void grab_box(){
    lower_hand();
    delay(200);
    close_hand();
    delay(200);
    raise_hand();
}

void drop_box(){
    lower_hand();
    delay(200);
    open_hand();
}