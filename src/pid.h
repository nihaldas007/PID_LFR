
#pragma once
#include <Arduino.h>
#include "motor.h"
#include "variable.h"


void pidControl()
{
  long avg = 0;
  long activeCount = 0;

  for (int i = 7; i >= 0; i--)
  {
    if (s[i] == 1)
    {
      avg += sensorPos[i];
      activeCount++;
    }
  }

  if (activeCount != 0)
  {
    error = avg / activeCount;
  }

  long P = error;
  errorSum += error;
  long D = error - lastError;
  lastError = error;

  long output = Kp * P + Ki * errorSum + Kd * D;

  int leftMotor = baseSpeed - output;
  int rightMotor = baseSpeed + output;

  motor(constrain(leftMotor, -255, 255), constrain(rightMotor, -255, 255));
}
