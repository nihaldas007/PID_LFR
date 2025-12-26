#pragma once
#include <Arduino.h>
#include "variable.h"

void sensorsGlobal()
{
  sensor = 0;
  sum = 0;
  for (int i = 0; i < 8; i++)
  {
    int val = analogRead(irPins[i]);
    if (val > th)
    {
      s[i] = 1;
    }
    else
    {
      s[i] = 0;
    }
    sensor += s[i] * binary[i];
    sum += s[i];
  }
}