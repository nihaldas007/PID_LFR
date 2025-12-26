#pragma once
#include <Arduino.h>
#include "motor.h"
#include "variable.h"
#include "sensorRead.h"


void gapline()
{
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