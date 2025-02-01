#ifndef SENSOR_PID_H
#define SENSOR_PID_H

#ifdef __cplusplus
#include <cstdint>
#include <string>
using namespace std;
extern "C"
{
#endif

#define FLASH_CAL_B 10
#define FLASH_CAL_W 20
#define FLASH_K_LINE 30
#define FLASH_LINE_WIDTH 40

#define NUM_PAST_READINGS 100

#include "main.h"

    extern bool inverted;
    extern int reading[8];
    extern int readingb[8];
    extern int readingw[8];
    extern int readingw2[8];
    extern int readingc[8];

    extern int diff_reading[8];
    extern float kp_line;
    extern float ki_line;
    extern float kd_line;
    extern int past_reading[NUM_PAST_READINGS][8];
    extern int r_index;
    extern float line_width_ref;

    void init_pid();
    float line_deviation();
    void print_sensor_readings();
    int8_t sen_on_white();
    int8_t line_position();
    void cal_black();
    void cal_white();
    void get_reading();
    uint16_t readADC(int8_t pin);
    void get_raw_readings(int N = 10);
    float map(float value, float fromLow, float fromHigh, float toLow, float toHigh);
    void init_sensor();
    float line_width(int index = 0);
    int32_t line_sum(int index = 0);
    void calibration();
    float line_side(int index = 0);
    void save_reading();
    void clear_past_readings();
    void line_allign();
    int read_pixel(int pixel);
    void get_reading_color();
    void cal_color();

    void delay(uint32_t ms);
    void delayMicroseconds(uint32_t us);
    double sum(float (*func)(int), double start, double end, double increment = 1);
    uint64_t micros();

#ifdef __cplusplus
}
#endif

#endif
