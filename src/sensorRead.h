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

void debug(bool read)
{
  while (1)
  {
    sensorsGlobal();
    for (int i = 7; i >= 0; i--)
    {
      if (read == true)
      {
        Serial.print(s[i]);
        Serial.print(" ");
      }
      else
      {
        Serial.print(analogRead(irPins[i]));
        Serial.print(" ");
      }
    }
    Serial.println(" ");
  }
}