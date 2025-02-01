#include "debug.h"
#include "loop.h"

char buffer[50];
char rx_buffer[1];
char rx_buffer_2[1];
volatile bool data_received = false;

void debug()
{
    static uint32_t last_press_time = 0;
    static uint32_t last_press_time_2 = 0;
    if (button_1)
    {
        stop();
        led2(100);
        delay(100);
        button_1 = false;
        delay(1500);
        if (button_1)
        {
            button_1 = false;
            while (!button_1)
            {
                update_encoders();
                current_arena = right_encoder / 200;
                led3(100 * (current_arena % 2));
                led2(100 * ((current_arena / 2) % 2));
                led1(100 * ((current_arena / 4) % 2));
            }
            led1(0);
            led2(0);
            led3(0);
            button_1 = false;
            setup_world();
            return;
        }
        start_speedy = !start_speedy;
        fade(2);
        button_1 = false;
    }

    if (button_2)
    {
        stop();
        fade(1);
        calibration();
        button_2 = false;
    }

    if (!data_received)
        return;

    data_received = false;
    string key;
    float k[3];
    stringstream rxdata(buffer);

    rxdata >> key;
    printf("Key: %s\n", key.c_str());

    if (key == "start")
    {
        start_speedy = !start_speedy;
        stop();
    }

    if (key == "stop")
        start_speedy = false;

    if (key == "line_follow")
    {
        start_line_following = !start_line_following;
        printf("Line following: %d\n", start_line_following);
        if (!start_line_following)
            forward(0);
    }

    if (key == "calibrate")
    {
        printf("Calibrate command received\n");
        calibration();
        printf("Calibration Completed\n");
    }

    if (key == "motor")
    {
        printf("Motor PID command received\n");
        floats(&rxdata, k);
        kp_motor = k[0];
        ki_motor = k[1];
        kd_motor = k[2];
        printf("motor kp = %.3f, ki = %.3f, kd = %.3f\n", kp_motor, ki_motor, kd_motor);
        putFloat(FLASH_K_MOTOR, kp_motor);
        putFloat(FLASH_K_MOTOR + 1, ki_motor);
        putFloat(FLASH_K_MOTOR + 2, kd_motor);
        commit_flash();
    }

    if (key == "pid")
    {
        printf("PID command received\n");
        floats(&rxdata, k);
        kp_line = k[0];
        ki_line = k[1];
        kd_line = k[2];
        printf("kp = %.3f, ki = %.3f, kd = %.3f\n", kp_line, ki_line, kd_line);
        putFloat(FLASH_K_LINE, kp_line);
        putFloat(FLASH_K_LINE + 1, ki_line);
        putFloat(FLASH_K_LINE + 2, kd_line);
        commit_flash();
    }

    if (key == "forward")
    {
        floats(&rxdata, k);
        forward(k[0], 0);
        printf("Forward: %.1f\n", k[0]);
    }

    if (key == "servo1")
    {
        floats(&rxdata, k);
        servo1(k[0]);
    }

    if (key == "servo2")
    {
        floats(&rxdata, k);
        servo2(k[0]);
    }

    if (key == "rotate")
    {
        floats(&rxdata, k);
        rotate(k[0]);
        printf("Rotate: %.1f\n", k[0]);
    }

    if (key == "turn_right")
    {
        floats(&rxdata, k);
        turn_right(k[0]);
    }

    if (key == "turn_left")
    {
        floats(&rxdata, k);
        turn_left(k[0]);
    }

    if (key == "speed")
    {
        floats(&rxdata, k);
        max_speed = k[0];
        printf("Max speed: %.1f\n", k[0]);
    }

    if (key == "reset")
    {
        NVIC_SystemReset();
    }

    if (key == "arena")
    {
        floats(&rxdata, k);
        current_arena = k[0];
    }

    if (key == "info")
    {
        get_reading();
        stop();
        printf("\n");
        printf("width: %.1f ld:%.2f\n", line_width(), line_side());
        print_sensor_readings();
        printf("Line  kp: %f, ki: %f, kd: %f\n", kp_line, ki_line, kd_line);
        printf("Motor kp: %f, ki: %f, kd: %f\r\n", kp_motor, ki_motor, kd_motor);
        printf("Max speed: %.1f\n", max_speed);
        printf("Current Arena: %d\n", current_arena);
        printf("phase: %d\n", phase);

        delay(500);
    }

    if (key == "sensorT")
    {
        request_tof_reading();
    }

    if (key == "sensorG")
    {
        request_ultrasonic_reading();
    }
}

void init_debug()
{
    HAL_UART_Receive_IT(&huart1, (uint8_t *)rx_buffer, 1);
    HAL_UART_Receive_IT(&huart3, (uint8_t *)rx_buffer_2, 1);
}

void floats(stringstream *_rxdata, float *k)
{
    for (int i = 0; i < 3; i++)
    {
        *_rxdata >> k[i];
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        static uint8_t index = 0;
        if (rx_buffer[0] == '\n')
        {
            buffer[index] = '\n';
            index = 0;
            data_received = true;

            string key;
            stringstream rxdata(buffer);
            rxdata >> key;
            if (key == "reset")
                NVIC_SystemReset();
        }
        else
            buffer[index++] = rx_buffer[0];
        // Restart the UART receive interrupt
        HAL_UART_Receive_IT(huart, (uint8_t *)rx_buffer, 1);
    }
    else if (huart->Instance == USART3)
    {

        static uint8_t index_2 = 0;
        static char buffer_2[10];
        if (rx_buffer_2[0] == '\n')
        {
            buffer_2[index_2] = '\n';
            index_2 = 0;

            string key;
            stringstream rxdata(buffer_2);
            rxdata >> key;
            float k[3];
            floats(&rxdata, k);
            if (key[0] == 'G')
            {
                ultrasonic_reading = k[0];
                got_ultrasonic_reading = true;
            }

            if (key[0] == 'T')
            {
                tof_reading = k[0];
                got_tof_reading = true;
            }

            printf("Key: %s\n", key.c_str());
            printf(": %.1f\n", k[0]);

            if (key == "reset")
                NVIC_SystemReset();
        }
        else
            buffer_2[index_2++] = rx_buffer_2[0];
        HAL_UART_Receive_IT(huart, (uint8_t *)rx_buffer_2, 1);
    }
}
