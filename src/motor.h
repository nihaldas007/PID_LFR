#pragma once
#include <Arduino.h>
#define RIGHT_DIR1 8
#define RIGHT_DIR2 7
#define RIGHT_EN 9

#define LEFT_DIR1 5
#define LEFT_DIR2 4
#define LEFT_EN 3

void motor(int leftSpeed, int rightSpeed)
{
  if (leftSpeed >= 0)
  {
    digitalWrite(LEFT_DIR1, HIGH);
    digitalWrite(LEFT_DIR2, LOW);
    analogWrite(LEFT_EN, leftSpeed);
  }
  else
  {
    digitalWrite(LEFT_DIR1, LOW);
    digitalWrite(LEFT_DIR2, HIGH);
    analogWrite(LEFT_EN, -leftSpeed);
  }

  if (rightSpeed >= 0)
  {
    digitalWrite(RIGHT_DIR1, HIGH);
    digitalWrite(RIGHT_DIR2, LOW);
    analogWrite(RIGHT_EN, rightSpeed);
  }
  else
  {
    digitalWrite(RIGHT_DIR1, LOW);
    digitalWrite(RIGHT_DIR2, HIGH);
    analogWrite(RIGHT_EN, -rightSpeed);
  }
}