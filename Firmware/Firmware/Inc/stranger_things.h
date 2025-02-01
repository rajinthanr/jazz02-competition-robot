#ifndef STRANGER_THINGS_H
#define STRANGER_THINGS_H

#ifdef __cplusplus
#include "loop.h"
extern "C"
{
#endif

    extern int current_arena;
    extern int phase;
    extern bool binaryCode[100];
    extern int index;
    extern int decimalValue;
    extern int current_arena;
    extern int x_pos;
    extern int y_pos;
    typedef enum
    {
        NORTH = 0,
        EAST = 90,
        SOUTH = 180,
        WEST = 270
    } Direction;
    extern Direction heading;

    void bar_code_arena();
    void setup_bar_code_arena();
    void setup_world();
    void world();
    void setup_color_arena();
    void color_arena();
    void line_handler();
    void line_handler_color();
    bool is_invertion();
    void go_to(int x, int y);
    void turn_to(Direction dir);
    void head_to(Direction dir);
    void fade(int led = 1);
    void open_hand();
    void close_hand();
    void raise_hand();
    void lower_hand();
    void grab_box();
    void drop_box();

    // Your code goes here

#ifdef __cplusplus
}
#endif

#endif // STRANGER_THINGS_H