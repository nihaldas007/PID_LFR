#include <Arduino.h>

const byte irPins[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
const int th = 420;

#define RIGHT_DIR1 8
#define RIGHT_DIR2 7
#define RIGHT_EN 9

#define LEFT_DIR1 5
#define LEFT_DIR2 4
#define LEFT_EN 3

float Kp = 0.05;
float Ki = 0.0;
float Kd = 0.8;

long error = 0, lastError = 0, errorSum = 0;
int baseSpeed = 130;

const int sensorPos[8] = {-3500, -2500, -1500, -500, 500, 1500, 2500, 3500};

int s[8];
int binary[] = {1, 2, 4, 8, 16, 32, 64, 128};
int sensor = 0;
int sum = 0;

long int flag = 0;
int k90 = 0;
int cross = 0;
int node = 10;

unsigned long m1, m2;
unsigned long stp = 30;
unsigned long resetTimer = 0; 

int tsp = 80;
int tbr = 30;
int br = 10;
int counter = 0;
int epoint = 20;

void motor(int leftSpeed, int rightSpeed);
void pidControl();
void readSensorsGlobal();
void handleOldTurnLogic();

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
  readSensorsGlobal();

  if (sum == 8)
  {
    m2 = millis();
    flag = 1;
    while (sum == 8)
    {
      readSensorsGlobal();
      if (millis() - m2 > stp)
      {
        motor(0, 0);
        while (sum == 8)
          readSensorsGlobal();
        return;
      }
    }
  }

  handleOldTurnLogic();

  if (flag != 0 && sum == 0)
  {
    return;
  }

  if (sum == 1 || sum == 2)
  {
    pidControl();
  }
}

void readSensorsGlobal()
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

void handleOldTurnLogic()
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
      m1 = millis();
      m2 = millis();

//       if (counter == 0) {
//           while (s[0] && !s[7])readSensorsGlobal();
//           if (!s[0]) {
//             delay(node); readSensorsGlobal();
//             if (sum > 0) {
// //              if (counter == 2) {
//                 cross = 1;
//                 // counter++;
// //              }
// //              else if(counter == 1 || counter == 0)cross = 1;
//             }
//           }
//         }
        // cross = 0;
    }
    else if (sensor == 0b00111111 || sensor == 0b00011111 || sensor == 0b00001111 || sensor == 0b00000111)
    {
      flag = 2;
      k90 = 2;
      resetTimer = millis(); 
    }
  }

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
            readSensorsGlobal();
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
            readSensorsGlobal();
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
        while (sum == 0) {
          readSensorsGlobal();
          m2 = millis();
          if (m2 - m1 >= epoint) {  //....................................  Sum == 0,  Millis(); ................................
            motor(-tsp, -tsp);
            delay(br);
            motor(-tsp, tsp);
            while (s[2] == 0 && s[3] == 0) readSensorsGlobal();
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
        readSensorsGlobal();
      while (s[3] == 0 && s[4] == 0)
        readSensorsGlobal();
      (cross == 1) ? motor(tsp, -tsp) : motor(-tsp, tsp);
      delay(tbr);
      cross = 0;
    }
  }
}

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