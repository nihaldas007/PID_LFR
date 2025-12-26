#pragma once
#include <Arduino.h>
#include "motor.h"
#include "variable.h"
#include "sensorRead.h"

void endpoint()
{
  m2 = millis();
  while (sum == 8)
  {
    sensorsGlobal();
    if (millis() - m2 > stp)
    {
      motor(0, 0);
      while (sum == 8)
        sensorsGlobal();
      return;
    }
  }
}