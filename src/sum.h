#include <Arduino.h>
#include "motor.h"
#include "variable.h"
#include "sensorRead.h"

void sumsGlobal()
{
  if (sum == 0)
  {
    if (flag != 0)
    {
      if (flag == 1)
      {
        if (k90 == 1)
        {
          motor(-tsp, tsp);
          while (1)
          {
            sensorsGlobal();
            if (s[3] == 1 || s[4] == 1)
              break;
          }
          motor(tsp, -tsp);
          delay(tbr);
        }
      }
      else if (flag == 2)
      {
        if (k90 == 2)
        {
          motor(tsp, -tsp);
          while (1)
          {
            sensorsGlobal();
            if (s[3] == 1 || s[4] == 1)
              break;
          }
          motor(-tsp, tsp);
          delay(tbr);
        }
      }
      flag = 0;
      k90 = 0;
    }
    m1 = m2 = millis();
    while (sum == 0)
    {
      sensorsGlobal();
      m2 = millis();
      if (m2 - m1 >= (unsigned long)epoint)
      { //....................................  UTurn ................................
        motor(-tsp, tsp);
        while (s[3] == 0 && s[4] == 0)
          sensorsGlobal();
        motor(tsp, -tsp);
        delay(tbr);
        break;
      }
    }
  }
  else if (sum == 1 || sum == 2)
  {
    if (cross != 0)
    {
      (cross == 1) ? motor(-tsp, tsp) : motor(tsp, -tsp);
      while (s[2] == 1 || s[3] == 1 || s[4] == 1 || s[5] == 1)
        sensorsGlobal();
      while (s[3] == 0 && s[4] == 0)
        sensorsGlobal();
      (cross == 1) ? motor(tsp, -tsp) : motor(-tsp, tsp);
      delay(tbr);
      cross = 0;
    }
  }
}