#include "stranger_things.h"
#include "loop.h"

void setup_hidden_task_arena()
{
    phase = 0;
    max_speed = 40;
    acceleration = 100;
    inverted = true;
    drop_box();
}

void hidden_task_arena()
{
    if (phase == 0)
    {
        get_reading();
        line_width_ref = line_sum() / 3;
        phase = 1;
        return;
    }
    if (phase == 1)
    {
        if (sum(line_width, 2, 6) > 4 && sum(line_width, 0, 1) < 3.5)
        {
            clear_past_readings();
            phase = 2;
            forward(4, 1, 0, 0);
            turn_left(90);
            return;
        }
        line_handler();
    }
    if (phase == 2)
    {
        if (sum(line_width, 2, 6) > 4 && sum(line_width, 0, 1) < 3.5)
        {
            clear_past_readings();
            phase = 3;
            forward(3, 1, 0, 0);
            grab_box();
            rotate(180);
            return;
        }
        line_handler();
    }
    if (phase == 3)
    {
        static int sub_task = 0;
        if (sum(line_width, 2, 6) > 4 && sum(line_width, 0, 1) < 3.5)
        {
            clear_past_readings();
            forward(4, 1, 0, 0);
            if (sub_task == 0)
                turn_left(90);
            else if (sub_task == 1)
                forward(3);
            else if (sub_task == 2)
            {
                turn_right(90);
                phase = 4;
                return;
            }
            sub_task++;
        }
        line_handler();
    }
    if (phase == 4)
    {
        if (sum(line_width, 2, 6) < 1 && sum(line_width, 7, 10) > 2.5)
        {
            stop();
            rotate(180);
            phase = 5;
        }
        line_handler();
        // hidden task
    }
    if (phase == 5)
    {
        static int sub_task = 0;
        if (sum(line_width, 2, 6) > 4 && sum(line_width, 0, 1) < 3.5)
        {
            clear_past_readings();
            forward(4, 1, 0, 0);
            if (sub_task == 0)
                turn_right(90);
            else if (sub_task == 1)
            {
                forward(20);
                drop_box();
                forward(-3);
                rotate(180);
            }
            else if (sub_task == 2)
            {
                turn_right(90);
            }
            else if (sub_task == 3)
            {
                current_arena = 6;
                setup_world();
                return;
            }
            sub_task++;
        }
        line_handler();
    }
}
