
#ifndef __LOOP_H
#define __LOOP_H

#ifdef __cplusplus
#include <string>
#include <sstream>
#include "debug.h"
#include "flash.h"
#include "speedy.h"
#include "motor.h"
#include "ir_sensor.h"
#include "stranger_things.h"
#include "extra.h"
using namespace std;

extern bool start_speedy;
extern bool start_line_following;
extern "C"
{
#endif

#include "main.h"
#include "usart.h"
#include "stdio.h"
#include "tim.h"
#include "gpio.h"

#define LED_ON HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_RESET);
#define LED_OFF HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);
#define LED_BLINK          \
    LED_TOGGLE delay(100); \
    LED_TOGGLE delay(100); \
    LED_TOGGLE
#define LED_TOGGLE HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_2);

    extern int time2;

    void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
    void loop(void);
    void setup(void);
    void debug(void);
    void delay(uint32_t ms);
    void delayMicroseconds(uint32_t us);

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
