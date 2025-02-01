#include "stranger_things.h"
#include "loop.h"

void setup_bar_code_arena()
{
    phase = 0;
    max_speed = 50;
    acceleration = 200;
}

void bar_code_arena()
{
    get_reading();
    if (phase == 0)
    {
        forward(250, 0);
        phase = 1;
    }
    if (phase == 1)
    {
        static float last_transition_distance = 0;
        if (detectWhiteToBlackTransition())
        {
            // printf("1\n");
            float current_distance = get_dis_cm();
            float distance_between = current_distance - last_transition_distance;
            // printf("%.1f\n",distance_between);

            if (last_transition_distance > 0)
            {
                if (distance_between < 8.5)
                {
                    binaryCode[index++] = false;
                }
                else
                {
                    binaryCode[index++] = true;
                }

                if (index >= 3 && binaryCode[index - 1] == 0 && binaryCode[index - 2] == 0 && binaryCode[index - 3] == 0)
                {
                    // stop();

                    for (int i = 0; i < index - 3; i++)
                    {
                        decimalValue = (decimalValue << 1) | binaryCode[i]; // Left shift and OR
                    }
                    printf("%d\n", decimalValue);
                    phase = 2;
                    return;
                }
            }
            last_transition_distance = current_distance;
        }
        update();
    }
    if (phase == 2)
    {
        static int sub_phase = 0;

        if (sum(line_width, 2, 6) > 4 && sum(line_width, 0, 1) < 3.5)
        {
            clear_past_readings();
            printf("sub: %d\n", sub_phase);

            if (sub_phase == 0)
            {
                forward(2, 1, 0, 0);
                go_right();
            }
            if (sub_phase == 1)
            {
                phase = 3;
                // setup_virtual_arena();
                // current_arena = 1;
                return;
                // go_right();
            }
            if (sub_phase == 2)
                go_left();
            if (sub_phase == 3)
                phase = 3;
            sub_phase++;
        }

        line_handler();
    }

    if (phase == 3)
    {
        current_arena = 1;
        setup_world();
        return;
    }
}