#include "loop.h"
#include "debug.h"
#include "flash.h"
#include "speedy.h"
#include "ir_sensor.h"
#include "extra.h"
#include "stranger_things.h"

int time2;
bool start_speedy = false;
bool start_line_following = false;

void setup()
{
  printf("setup\n");
  init_flash();
  init_debug();
  HAL_TIM_Base_Start(&htim6);
  init_speedy();
  init_sensor();
  print_sensor_readings();
  setup_servos();

  // forward(10);
  //  servo2(100);
  //  servo1(30);
  //  delay(1000);
  //  servo2(45);
  //  delay(1000);
  //  servo1(100);
  // drop_coin();
  // for (int a = 0; a < 10; a++)
  // {
  //   for (int i = 0; i < 100; i++)
  //   {
  //     led1(i);
  //     led2(i);
  //     led3(100-i);
  //     delay(10);
  //   }
  //   for (int i = 0; i < 100; i++)
  //   {
  //     led1(100 - i);
  //     led2(100 - i);
  //     led3(i);
  //     delay(10);
  //   }
  // }
  // drop_coin();
  //current_arena = 5;

  setup_world();

}

void loop(void)
{
  // update_encoders();
  debug();
  if (start_speedy)
  {
    world();
  }
  else
  {
    if (start_line_following)
    {
      get_reading();
      line_following();
    }
    else
    {
      update();
    }
  }
}
