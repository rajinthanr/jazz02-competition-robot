#include "stranger_things.h"
#include "loop.h"

void setup_terrain()
{
    phase = 0;
    max_speed = 40;
    acceleration = 200;
}

void terrain()
{
    if (phase == 0)
    {
        rotate(180);
        forward(-80);
        rotate(270);
        // find and drop coin
        drop_coin();
        stop();
        fade(1);
        fade(2);
        fade(3);
        delay(100000);
    }
}