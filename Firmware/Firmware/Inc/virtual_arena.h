#include "stranger_things.h"
#include "loop.h"

void setup_virtual_arena()
{
    phase = 0;
    max_speed = 40;
    acceleration = 150;
}

void virtual_arena()
{
    if (phase == 0)
    {
        int target_gate = 1;
        int box_x_pos = decimalValue % 5;
        int box_y_pos = 0;
        { // if box not in 0,0
            go_to(1, 0);
            request_ultrasonic_reading();

            go_to(decimalValue % 5, 0);
            if (got_ultrasonic_reading && ultrasonic_reading < 60)
                target_gate = 3;

            if (target_gate > box_x_pos)
            {
                led3(100);
                go_to(target_gate, 0);
                stop();
                led3(0);
                forward(-3);
                head_to(WEST);
                go_to(target_gate - 1, 1);
            }
            else if (target_gate < box_x_pos)
            {
                forward(-3);
                head_to(WEST);
                forward(-9);
                stop();
                led3(100);
                go_to(target_gate, 0);
                stop();
                led3(0);
                forward(-3);
                head_to(EAST);
                go_to(target_gate + 1, 1);
            }
            else
            {
                // nothing
            }
            go_to(target_gate, 0);
            forward(-3);
            head_to(NORTH);
            forward(-6);
            stop();
            led3(100);
            go_to(target_gate, 1);
        }

        turn_to(NORTH);
        stop();
        drive(0, 0);
        delay(300);
        // led3(0);
        phase = 1;
        return;
    }
    if (phase == 1)
    {
        forward(3, 1, 0, 0);
        current_arena = 2;
        setup_world();
        return;
    }
}

void handle_position()
{
}

void update_pos()
{
    if (heading == NORTH)
        y_pos++;
    if (heading == EAST)
        x_pos++;
    if (heading == SOUTH)
        y_pos--;
    if (heading == WEST)
        x_pos--;

    printf("x: %d y: %d\n", x_pos, y_pos);
}

void turn_to(Direction dir)
{
    // stop();
    if (dir == heading)
        return;
    forward(7, 1, 0, 0);
    stop();
    head_to(dir);
    stop();
}

void head_to(Direction dir)
{
    // forward(7, 1, 0, 0);
    int diff = dir - heading;
    if (diff == 270)
        diff = -90;
    rotate(diff);
    heading = dir;
}

void go_to(int x, int y)
{
    // stop();
    int diff_x = x - x_pos;
    int diff_y = y - y_pos;

    if (diff_x > 0)
        turn_to(EAST);
    if (diff_x < 0)
        turn_to(WEST);

    while (x_pos != x)
    {
        if (sum(line_width, 2, 6) > 4 && sum(line_width, 0, 1) < 3.5)
        {
            update_pos();
            clear_past_readings();
        }
        line_handler();
    }

    if (diff_y > 0)
        turn_to(NORTH);
    if (diff_y < 0)
        turn_to(SOUTH);

    while (y_pos != y)
    {
        if (sum(line_width, 2, 6) > 4 && sum(line_width, 0, 1) < 3.5)
        {
            update_pos();
            clear_past_readings();
        }
        line_handler();
    }
    // stop();
}