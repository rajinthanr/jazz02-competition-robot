#include "stranger_things.h"
#include "loop.h"
#include "extra.h"
#include "virtual_arena.h"
#include "bar_code_arena.h"
#include "color_arena.h"
#include "dot_arena.h"
#include "box_arena.h"
#include "hidden_task_arena.h"
#include "terrain.h"

int phase = 0;
bool binaryCode[100];
int index = 0;
int decimalValue = 0;
int current_arena = 0;

int x_pos = 0;
int y_pos = 0;

Direction heading = NORTH;

void setup_world()
{
    stop();
    fade(1);
    // current_arena = 2;
    if (current_arena == 0)
        setup_bar_code_arena();
    if (current_arena == 1)
        setup_virtual_arena();
    if (current_arena == 2)
        setup_color_arena();
    if (current_arena == 3)
        setup_dot_arena();
    if (current_arena == 4)
        setup_box_arena();
    if (current_arena == 5)
        setup_hidden_task_arena();
    if (current_arena == 6)
        setup_terrain();
}

void world()
{
    if (current_arena == 0)
        bar_code_arena();
    if (current_arena == 1)
        virtual_arena();
    if (current_arena == 2)
        color_arena();
    if (current_arena == 3)
        dot_arena();
    if (current_arena == 4)
        box_arena();
    if (current_arena == 5)
        hidden_task_arena();
    if (current_arena == 6)
        terrain();
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

void line_handler()
{
    get_reading();
    if (line_width() > 3.5)
    {
        // forward(0.3, 1, 0);
        drive(max_speed, max_speed);
    }
    else if (line_width() > 1)
    {
        line_following();
    }
    else
    {
        // forward(0.3, 1, 0);
        drive(max_speed, max_speed);
    }
    if (moved_mm(3))
    {
        save_reading();
        // printf("n: %d\n", r_index);
    }
}

void line_handler_color()
{
    get_reading_color();
    if (line_width() > 3.5)
    {
        // forward(0.3, 1, 0);
        drive(max_speed, max_speed);
    }
    else if (line_width() > 1)
    {
        line_following();
    }
    else
    {
        // forward(0.3, 1, 0);
        drive(max_speed, max_speed);
    }
    if (moved_mm(3))
    {
        save_reading();
        // printf("n: %d\n", r_index);
    }
}

bool is_invertion()
{
    int rising_index = 7;
    int falling_index = 7;

    for (int a = 1; a < 8; a++)
        diff_reading[a - 1] = reading[a] - reading[a - 1];

    for (int a = 0; a < 7; a++)
    {
        if (diff_reading[a] > 500)
            rising_index = a;
        if (diff_reading[6 - a] < -500)
            falling_index = 6 - a;
    }

    if (rising_index > falling_index && rising_index != 7 && falling_index != 7)
    {
        // delay(10);
        forward(1, 1, 0, 0);
        printf("inverted rising: %d  falling: %d\n", rising_index, falling_index);
        return true;
    }
    return false;
}