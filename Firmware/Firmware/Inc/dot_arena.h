#include "stranger_things.h"
#include "loop.h"

void setup_dot_arena()
{
    phase = 0;
    max_speed = 40;
    acceleration = 200;
}

void dot_arena()
{
    if (phase == 0)
    {
        get_reading();
        if (line_width() < 3.5)
        {
            forward(2, 1, 0, 0);
            phase = 1;
            return;
        }
        forward(1, 1, 0, 0);
    }
    if (phase == 1)
    {
        get_reading();
        if (line_width() > 3.5)
        {
            forward(2, 1, 0, 0);
            phase = 2;
            stop();
            request_tof_reading();
            delay(3000);
            return;
        }
        line_handler();
    }
    if (phase == 2)
    {
        drive(0, 0);

        request_tof_reading();
        delay(300);
        if (tof_reading > 300)
        {
            phase = 3;
        }

        // need to check for door
    }
    if (phase == 3)
    {
        get_reading();
        if (line_width() < 2) // black color
        {
            forward(1, 1, 0, 0);
            phase = 4;
            return;
        }
        forward(1, 1, 0, 0);
    }
    if (phase == 4)
    {
        get_reading();
        if (line_width() > 2) // black color
        {
            forward(2, 1, 0, 0);
            phase = 5;
            return;
        }
        forward(1, 1, 0, 0);
    }
    if (phase == 5)
    {
        current_arena = 4;
    }
}