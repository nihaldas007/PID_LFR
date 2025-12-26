#include <Arduino.h>
#include "motor.h"
#include "pid.h"
#include "SectionT.h"
#include "variable.h"
#include "sensorRead.h"
#include "endStop.h"
#include "gapline.h"
#include "sum.h"

void motor(int leftSpeed, int rightSpeed);
void pidControl();
void sensorsGlobal();
void turnLogic();
void tsection(bool isLR);
void endpoint();
void gapline();
void sumsGlobal();

void setup()
{
  Serial.begin(9600);
  pinMode(RIGHT_DIR1, OUTPUT);
  pinMode(RIGHT_DIR2, OUTPUT);
  pinMode(RIGHT_EN, OUTPUT);

  pinMode(LEFT_DIR1, OUTPUT);
  pinMode(LEFT_DIR2, OUTPUT);
  pinMode(LEFT_EN, OUTPUT);
}

void loop()
{
  turnLogic();
  sensorsGlobal();
  if (sum == 8)
  {
    endpoint();
  }
  if (sum >= 6 && (s[3] || s[4]))
  {
    tsection(false);
    return;
  }
  if (sum == 0)
  {
    gapline();
    return;
  }
  if (flag != 0 && sum == 0)
    return;
  if (sum == 1 || sum == 2)
    pidControl();
}

void turnLogic()
{
  if ((flag != 0 || cross != 0) && (millis() - resetTimer > 20))
  {
    flag = 0;
    k90 = 0;
    cross = 0;
  }



  if (sum >= 3 && sum <= 6)
  {


    if (sensor == 0b11111100 || sensor == 0b11111000 || sensor == 0b11110000 || sensor == 0b11100000) 
    {
      flag = 1;
      k90 = 1;
      cross = 0;
      resetTimer = millis();
      if (!counter)
      {
        m1 = m2 = millis();
        while (sum != 8 && sum != 0)
        {
          sensorsGlobal();
          m2 = millis();
          if (m2 - m1 >= 10)
          {
            cross = 1;
            // counter = false;
            break;
          }
        }
      }
    }



    else if (sensor == 0b00111111 || sensor == 0b00011111 || sensor == 0b00001111 || sensor == 0b00000111) 
    {
      flag = 2;
      k90 = 2;
      resetTimer = millis();
      // if (counter)
      // {
      //   m1 = m2 = millis();
      //   while (sum != 8 && sum != 0)
      //   {
      //     sensorsGlobal();
      //     m2 = millis();
      //     if (m2 - m1 >= 10)
      //     {
      //       cross = 2;
      //       counter = false;
      //       break;
      //     }
      //   }
      // }
    }
  }
  sumsGlobal();
}
