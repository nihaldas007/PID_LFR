#include <Arduino.h>
#include <Arduino.h>

// ================= PINS (FROM NEW CODE) =================
const byte irPins[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
const int th = 420; // Threshold

#define RIGHT_DIR1 8
#define RIGHT_DIR2 7
#define RIGHT_EN 9 // PWM

#define LEFT_DIR1 5
#define LEFT_DIR2 4
#define LEFT_EN 3 // PWM

// ================= PID CONSTANTS (FROM NEW CODE) =================
float Kp = 0.05;
float Ki = 0.0;
float Kd = 0.8;

long error = 0, lastError = 0, errorSum = 0;
int baseSpeed = 100;

const int sensorPos[8] = {-3500, -2500, -1500, -500, 500, 1500, 2500, 3500};

// ================= 90 DEGREE LOGIC VARS (FROM OLD CODE) =================
int s[8]; // Array to hold 0 or 1
int binary[] = {1, 2, 4, 8, 16, 32, 64, 128};
int sensor = 0; // Bitwise sensor value
int sum = 0;    // Count of active sensors

// Flags for turn logic
long int flag = 0;
int k90 = 0;
int pos = 0; // used in old logic reset

// Turn timing/speed constants (From Old Code)
int tsp = 100; // Turn Speed
int tbr = 20;  // Time Brake/Reverse (Stabilize)
int br = 5;    // Brake/Reverse duration before turn

// ================= FUNCTION DECLARATIONS =================
void motor(int leftSpeed, int rightSpeed);
void pidControl();
bool checkStopCondition();
void readSensorsGlobal(); // Helper to update 'sensor' and 'sum'
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
  // 1. Read Sensors and populate 's[]', 'sensor', and 'sum'
  readSensorsGlobal();

  // 2. Check Stop Condition (New Code Style)
  if (checkStopCondition())
  {
    motor(0, 0);
    return;
  }

  // 3. Check and Execute 90 Degree Turn (Old Code Logic)
  // This function checks the flags and executes the specific move sequences
  // If a turn is executed, we return immediately to skip PID for this loop
  handleOldTurnLogic();

  if (flag != 0 && sum == 0)
  {
    // If flag is set and line is lost, handleOldTurnLogic already handled the turn.
    // We just ensure we don't run PID immediately after.
    return;
  }

  // 4. PID Control (New Code Style)
  // Only runs if we are on the line (sum > 0) or if we haven't triggered a turn flag yet
  if (sum > 0)
  {
    pidControl();
  }
}

// ================= TURN LOGIC (MERGED) =================

void readSensorsGlobal()
{
  sensor = 0;
  sum = 0;
  // Map New Code Pins to Old Code Bitwise logic
  // Assuming irPins[0] is Right-most or Left-most?
  // Standard is usually 0=Left, 7=Right or vice versa.
  // We apply the bitmask exactly as the loop structure suggests.
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

    // Create the bitmask (Old Code: binary[i] corresponds to pin i)
    sensor += s[i] * binary[i];
    sum += s[i];
  }
}

void handleOldTurnLogic()
{
  // --- PART 1: DETECTION (Set Flags) ---
  // This runs while we are still on the line
  if (sum >= 3 && sum <= 6)
  {
    // Left Hand Rule Logic (From Old Code)
    if (sensor == 0b11111100 || sensor == 0b11111000 || sensor == 0b11110000 || sensor == 0b11100000)
    {
      flag = 1;
      k90 = 1;
    }
    // Right Hand Rule Logic (From Old Code)
    else if (sensor == 0b00111111 || sensor == 0b00011111 || sensor == 0b00001111 || sensor == 0b00000111)
    {
      flag = 2;
      k90 = 2;
    }
  }

  // --- PART 2: EXECUTION (Perform Turn) ---
  // This runs when we lose the line (sum == 0) AND a flag was previously set
  if (sum == 0)
  {
    if (flag != 0)
    {

      // LEFT TURN EXECUTION
      if (flag == 1)
      {
        if (k90 == 1)
        {
          motor(-tsp, tsp);
          while (1)
          {
            readSensorsGlobal(); // Update sensors
            if (s[3] == 1 || s[4] == 1)
              break; // Old code: while (s[2] == 0 && s[3] == 0) check();
          }
          motor(tsp, -tsp);
          delay(tbr);

          pos = 0;
        }
      }
      // RIGHT TURN EXECUTION
      else if (flag == 2)
      {
        if (k90 == 2)
        {
          motor(tsp, -tsp);
          while (1)
          {
            readSensorsGlobal(); // Update sensors
            if (s[3] == 1 || s[4] == 1)
              break;
          }
          motor(-tsp, tsp);
          delay(tbr);

          pos = 0;
        }
      }
      // Reset Flag after turn
      flag = 0;
    }
  }
}

// ================= PID & MOTOR (NEW CODE) =================

bool checkStopCondition()
{
  // Using the 'sum' calculated in readSensorsGlobal
  if (sum >= 8)
  { // Or >= 6 depending on preference
    return true;
  }
  return false;
}

void pidControl()
{
  // Uses New Code PID Logic
  long avg = 0;
  long activeCount = 0;

  for (int i = 7; i >= 0; i--)
  {
    if (s[i] == 1)
    { // Uses the s[] array populated in readSensorsGlobal
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
  // Left Motor
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

  // Right Motor
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