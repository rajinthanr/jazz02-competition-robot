#include "stranger_things.h"
#include "loop.h"

void setup_color_arena()
{
    printf("setup_color_arena\n");
    phase = 0;
    max_speed = 50;
    acceleration = 200;
}

void color_arena()
{
    if (phase == 0)
    {
        if (read_pixel(7) > 100)
        {
            phase = 1;
            return;
        }
        line_handler();
    }
    if (phase == 1)
    {
        stop();
        delay(200);
        forward(3);
        cal_color();
        led3(0);
        phase = 2;
        return;
    }
    if (phase == 2)
    {
        get_reading();
        if (line_width() < 4)
        {
            forward(1, 1, 0, 0);
            phase = 3;
            return;
        }
        else
        {
            forward(1, 1, 0, 0);
        }
    }
    if (phase == 3)
    {
        // if (is_invertion())
        // {
        //     led1(100);
        //     phase = 4;
        //     return;
        // }
        if (sum(line_width, 2, 22) > 5)
        {
            stop();
            cal_color();
            phase = 5;
            return;
        }
        if (sum(line_width, 2, 6) > 4 && sum(line_width, 0, 1) < 3.5)
        {
            forward(7, 1, 0, 0);
            if (sum(line_side, 2, 6) > 1)
                rotate(90);
            else if (sum(line_side, 2, 6) < -1)
                rotate(-90);
            else
                forward(1, 1, 0, 0);
            clear_past_readings();
        }
        line_handler_color();
    }
    if (phase == 4)
    {
        drive(0, 0);
        // start_speedy = false;
        while (line_width() > 4)
        {
            get_reading();
            line_following();
        }
        while (line_width() < 5)
        {
            get_reading();
            line_following();
        }
        phase = 5;
        forward(3, 1, 0, 0);
        cal_color();
        return;
    }
    if (phase == 5)
    {
        current_arena = 3;
        setup_world();
    }
}
