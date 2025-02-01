#include "ir_sensor.h"
#include "adc.h"
#include "main.h"
#include "debug.h"
#include "flash.h"

float kp_line = 0.016;
float ki_line = 0.1;
float kd_line = 0.15;

float line_width_ref = 3200;

using namespace std;

int past_reading[NUM_PAST_READINGS][8];
int r_index = 0;
int reading[8];
int readingb[8];
int readingw[8];
int readingw2[8];
int readingc[8];
int diff_reading[8];
bool inverted;

// Read sensor values
void init_pid()
{
    kp_line = getFloat(FLASH_K_LINE);
    ki_line = getFloat(FLASH_K_LINE + 1);
    kd_line = getFloat(FLASH_K_LINE + 2);
    // kp_line = 0.01;
    // kd_line = 0.05;
    // ki_line = 0;
    kp_motor = getFloat(FLASH_K_MOTOR);
    ki_motor = getFloat(FLASH_K_MOTOR + 1);
    kd_motor = getFloat(FLASH_K_MOTOR + 2);
}

void init_sensor()
{
    for (int i = 0; i < 8; i++)
    {
        readingb[i] = getInt(FLASH_CAL_B + i);
        readingw[i] = getInt(FLASH_CAL_W + i);
        line_width_ref = getFloat(FLASH_LINE_WIDTH);
    }
    for (int i = 0; i < 8; i++)
    {
        readingw2[i] = readingw2[i];
    }
}

void cal_black()
{
    get_raw_readings(100);
    for (int i = 0; i < 8; i++)
    {
        readingb[i] = reading[i];
        int r = readingb[i];
        putInt(FLASH_CAL_B + i, r);
    }
}

void line_allign()
{
    int starting_angle = 0;
    int angle_range = 15;
    bool right = true;
    float tem_speed = max_speed;
    // max_speed = 30;
    get_reading();
    while (line_width() < 1.5)
    {
        if (right)
            rotate(3);
        else
            rotate(-3);
        if (abs(bot_angle - starting_angle) > angle_range)
        {
            stop();
            if (right)
                rotate(-angle_range);
            else
                rotate(angle_range);
            stop();
            starting_angle = bot_angle;
            angle_range += 10;
            right = !right;
        }
        get_reading();
    }
    int st = uwTick;
    while (uwTick - st < 200)
    {
        float adj = line_deviation();
        drive(adj, -adj);
    }
    stop();
    max_speed = tem_speed;
}

void calibration()
{
    printf("Calibration started\n");
    LED_BLINK
    LED_OFF
    led1(0);
    delay(2000);
    cal_black();
    LED_ON
    led1(100);
    delay(2000);
    cal_white();
    led2(100);
    LED_BLINK
    LED_BLINK
    delay(2000);
    get_reading();
    line_width_ref = line_sum() / 3;
    putFloat(FLASH_LINE_WIDTH, line_width_ref);
    commit_flash();
    led1(0);
    led2(0);
}

void calibration1()
{
    float t_speed = max_speed;
    max_speed = 25;
    rotate(45);
    stop();
    cal_black();
    rotate(-90, 0);
    for (int i = 0; i < 8; i++)
        readingw[i] = 0;
    while (!finished())
    {
        // update_encoders();
        get_raw_readings(10);
        // delay(10);
        for (int i = 0; i < 8; i++)
        {
            if (abs(reading[i] - readingb[i]) > abs(readingw[i] - readingb[i]))
                readingw[i] = reading[i];
            putInt(FLASH_CAL_W + i, readingw[i]);
        }
        update();
    }
    stop();
    rotate(45);
    stop();
    line_allign();
    delay(1000);
    get_reading();
    line_width_ref = line_sum() / 3;
    putFloat(FLASH_LINE_WIDTH, line_width_ref);
    commit_flash();
    max_speed = t_speed;
}

void cal_white()
{
    get_raw_readings(10);
    for (int i = 0; i < 8; i++)
    {
        readingw[i] = reading[i];
        int r = readingw[i];
        putInt(FLASH_CAL_W + i, r);
    }
}

void cal_color()
{
    get_raw_readings(10);
    for (int i = 0; i < 8; i++)
    {
        readingc[i] = reading[i];
        readingw[i] = readingc[i];
    }
}

int32_t line_sum(int index)
{
    // get_reading();
    int32_t total = 0;
    for (int i = 0; i < 8; i++)
    {
        if (index == 0)
            total += reading[i];
        else
            total += past_reading[(NUM_PAST_READINGS + r_index - index) % NUM_PAST_READINGS][i];
    }
    return total;
}

void clear_past_readings()
{
    for (int i = 0; i < NUM_PAST_READINGS; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            past_reading[i][j] = 3;
        }
    }
}

void save_reading()
{
    for (int i = 0; i < 8; i++)
    {
        past_reading[r_index][i] = reading[i];
    }
    r_index = (r_index + 1) % NUM_PAST_READINGS;
}

float line_width(int index)
{
    return line_sum(index) / line_width_ref;
}

float line_side(int index)
{
    int total = 0;
    if (index != 0)
    {
        for (int i = 0; i < 8; i++)
        {
            if (i < 4)
                total -= past_reading[(NUM_PAST_READINGS + r_index - index) % NUM_PAST_READINGS][i];
            else
                total += past_reading[(NUM_PAST_READINGS + r_index - index) % NUM_PAST_READINGS][i];
        }
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            if (i < 4)
                total -= reading[i];
            else
                total += reading[i];
        }
    }
    return total / line_width_ref;
}

float map(float value, float fromLow, float fromHigh, float toLow, float toHigh)
{
    value = (value - fromLow) * (toHigh - toLow) / (fromHigh - fromLow) + toLow;
    value = (value < toLow) ? toLow : (value > toHigh ? toHigh : value);
    return value;
}

float line_deviation()
{
    int16_t weights[8] = {-7, -5, -3, -1, 1, 3, 5, 7};
    int32_t position = 0;
    int32_t total = 0;
    float error;
    static float previous_error;
    static float integral;
    float pid_output;

    // get_reading();

    for (int i = 0; i < 8; i++)
    {
        total += reading[i];
        position += reading[i] * weights[i];
    }

    if (total < 100)
        total = 100;

    if (line_sum() < 500)
        return 0;
    // error = (float)position / line_width(); // Calculate position error
    error = (float)position * 1000 / line_sum(); // Calculate position error

    integral = integral + error;
    integral = (integral < -20.0f) ? -20.0f : (integral > 20.0f ? 20.0f : integral);
    float derivative = error - previous_error;

    pid_output = kp_line * error + ki_line * integral + kd_line * derivative;

    previous_error = error;

    // Implement control action using pid_output (e.g., adjust motor speeds)
    // For example:
    // setMotorSpeed(pid_output);

    // Debugging output
    // println("pid = " + f2s(pid_output,3));
    // print_sensor_readings();

    return pid_output;
}

void get_raw_readings(int N)
{
    int32_t reading_[600][8];
    for (int i = 0; i < N; i++)
    {
        for (int a = 0; a < 8; a++)
        {
            reading_[i][a] = readADC(a);
            delayMicroseconds(1);
        }
    }

    for (int a = 0; a < 8; a++)
    {
        reading[a] = 0;
        for (int i = 0; i < N; i++)
        {
            reading[a] += reading_[i][a];
        }
        reading[a] /= N;
        reading[a] = 4095 - reading[a];
    }
}

int read_pixel(int pixel)
{
    int value = 4095 - readADC(pixel);
    value = map(value, readingb[pixel], readingw[pixel], 0, 1000);
    if (inverted)
        value = 1000 - value;
    return value;
}

void get_reading()
{
    int rising_index = 7;
    int falling_index = 7;
    get_raw_readings(3);
    for (int a = 0; a < 8; a++)
    {
        reading[a] = map(reading[a], readingb[a], readingw[a], 0, 1000);
        if (inverted)
            reading[a] = 1000 - reading[a];
    }
}

void get_reading_color()
{
    int rising_index = 7;
    int falling_index = 7;
    get_raw_readings(3);
    for (int a = 0; a < 8; a++)
    {
        reading[a] = map(reading[a], readingb[a], readingc[a], 0, 1000);
    }
}

void print_sensor_readings()
{
    // get_reading();
    string str = "";
    for (int a = 0; a < 8; a++)
    {
        printf("%d -- ", reading[a]);
    }

    printf("\n");
}

uint16_t readADC(int8_t pin)
{
    ADC_ChannelConfTypeDef sConfig;
    uint16_t adc_val = 0;
    // if (pin == 7)
    //     pin = -1;
    // pin = pin + 1;

    pin = 7 - pin;

    if (pin >= 0 && pin <= 7)
    {
        switch (pin)
        {
        case 0:
            sConfig.Channel = ADC_CHANNEL_0;
            break;
        case 1:
            sConfig.Channel = ADC_CHANNEL_1;
            break;
        case 2:
            sConfig.Channel = ADC_CHANNEL_2;
            break;
        case 3:
            sConfig.Channel = ADC_CHANNEL_3;
            break;
        case 4:
            sConfig.Channel = ADC_CHANNEL_4;
            break;
        case 5:
            sConfig.Channel = ADC_CHANNEL_5;
            break;
        case 6:
            sConfig.Channel = ADC_CHANNEL_6;
            break;
        case 7:
            sConfig.Channel = ADC_CHANNEL_7;
            break;
        default:
            return 0;
        }

        sConfig.Rank = 1;
        sConfig.SamplingTime = ADC_SAMPLETIME_84CYCLES;

        if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
        {
            Error_Handler();
        }

        HAL_ADC_Start(&hadc1);

        if (HAL_ADC_PollForConversion(&hadc1, 10) == HAL_OK)
        {
            adc_val = HAL_ADC_GetValue(&hadc1);
        }

        HAL_ADC_Stop(&hadc1);
    }

    return adc_val;
}

void delay(uint32_t ms)
{
    HAL_Delay(ms);
}

void delayMicroseconds(uint32_t us)
{
    uint64_t start = micros();
    while (micros() - start < us)
        ;
}

uint64_t micros()
{
    static uint32_t overflow_count = 0;
    static uint16_t last_timer_value = 0;
    uint16_t current_timer_value = __HAL_TIM_GET_COUNTER(&htim6);

    if (current_timer_value < last_timer_value)
    {
        overflow_count++;
    }

    last_timer_value = current_timer_value;

    return (overflow_count << 16) | current_timer_value;
}

double sum(float (*func)(int), double start, double end, double increment)
{
    double sum = 0.0;
    for (double x = start; x <= end; x += increment)
    {
        sum += func(x);
    }
    return sum / (end - start + 1);
}
