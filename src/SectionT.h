#pragma once

#include <Arduino.h>
#include "motor.h"
#include "variable.h"
#include "sensorRead.h"

void tsection(bool isLR)
{
  // Move forward a bit
  motor(baseSpeed, baseSpeed);
  delay(40);

  //  LEFT == true
  if (isLR == true)
    motor(-tsp, tsp);
  else
    motor(tsp, -tsp);

  // Wait until center sensors catch line
  while (true)
  {
    sensorsGlobal();
    if (s[3] || s[4])
      break;
  }

  if (isLR == true)
    motor(tsp, -tsp);
  else
    motor(-tsp, tsp); // motor(tsp, -tsp); // left
  // motor(-tsp, tsp);//right
  delay(tbr);
}