#include "stranger_things.h"
#include "loop.h"

void setup_box_arena()
{
    phase = 0;
    heading = NORTH;
}

void box_arena()
{
    int target_box_pos = 0;
    if (phase == 0)
    {
        inverted = true;
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
            return;
        }

        line_handler();
    }
    if (phase == 2)
    {
        turn_to(WEST);
        max_speed = 30;
        float pre_dis = get_dis_cm();
        while (1)
        {
            line_following();
            if (moved_mm(10))
                request_tof_reading();

            if (get_dis_cm() - pre_dis > 20)
            {
                break;
            }

            if (tof_reading < 300)
            {
                forward(1);
                int box_height = tof_reading / 1.41;
                if (box_height > 125)
                    target_box_pos = 1;
                else if (box_height > 75)
                    target_box_pos = 2;
                else
                    target_box_pos = 3;
                forward(box_height);
                break;
            }
        }
        phase = 3;
        forward(0.5);
        grab_box();
        update_pos();
        return;
    }
    if (phase == 3)
    {
        go_to(1, target_box_pos);
    }
}