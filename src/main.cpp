#include <Arduino.h>
const byte irPins[8] = { A0, A1, A2, A3, A4, A5, A6, A7 };
const int th = 420;  // Threshold

#define RIGHT_DIR1 8
#define RIGHT_DIR2 7
#define RIGHT_EN 9  // PWM

#define LEFT_DIR1 5
#define LEFT_DIR2 4
#define LEFT_EN 3  // PWM

// PID constants
float Kp = 0.05;  
float Ki = 0.0;   
float Kd = 0.8;   

long error = 0, lastError = 0, errorSum = 0;
int baseSpeed = 100;  // Adjust as needed 120

const int sensorPos[8] = { -3500, -2500, -1500, -500, 500, 1500, 2500, 3500 };

void motor(int leftSpeed, int rightSpeed);
void pidControl();
bool checkStopCondition();
bool check90DegreeTurn();
void turnLeft90();
void turnRight90();

void setup() {
  Serial.begin(9600);
  pinMode(RIGHT_DIR1, OUTPUT);
  pinMode(RIGHT_DIR2, OUTPUT);
  pinMode(RIGHT_EN, OUTPUT);

  pinMode(LEFT_DIR1, OUTPUT);
  pinMode(LEFT_DIR2, OUTPUT);
  pinMode(LEFT_EN, OUTPUT);
}

void loop() {

  if (checkStopCondition()) {
    motor(0, 0);
    return;
  }

  if (check90DegreeTurn()) {
    return;  // 90° handled, skip PID
  }

  pidControl();  // Only when no special condition
}

bool check90DegreeTurn() {

  static unsigned long lastTurnTime = 0;
  if (millis() - lastTurnTime < 300) return false;
  lastTurnTime = millis();
  
  int s[8];

  for (int i = 0; i < 8; i++) {
    s[i] = (analogRead(irPins[i]) > th) ? 1 : 0;
  }

  int left = s[0] + s[1] + s[2];
  int center = s[3] + s[4];
  int right = s[5] + s[6] + s[7];

  // ❌ CROSS / PLUS junction → GO STRAIGHT
  if (left >= 2 && center >= 1 && right >= 2) {
    return false;
  }

  // ✅ RIGHT 90° (L shape)
  if (right >= 2 && center == 0 && left == 0) {
    turnRight90();
    return true;
  }

  // ✅ LEFT 90° (L shape)
  if (left >= 2 && center == 0 && right == 0) {
    turnLeft90();
    return true;
  }

  return false;
}

void turnLeft90() {
  motor(100, -100);  // Rotate left
  delay(160);        // Tune this delay
  motor(0, 0);
}

void turnRight90() {
  motor(-100, 100);  // Rotate right
  delay(160);        // Tune this delay
  motor(0, 0);
}

bool checkStopCondition() {
  int blackCount = 0;
  for (int i = 7; i >= 0; i--) {
    int val = analogRead(irPins[i]);
    if (val > th) {
      blackCount++;
    }
  }
  if (blackCount >= 6) {   
    return true;
  }
  return false;
}

void pidControl() {
  long avg = 0, sum = 0;

  for (int i = 7; i >= 0; i--) {
    int val = analogRead(irPins[i]);
    if (val > th) {
      avg += sensorPos[i];
      sum++;
    }
  }

  if (sum != 0) {
    error = avg / sum;
  }

  long P = error;
  errorSum += error;                 // Integral
  long D = error - lastError;        // Derivative
  lastError = error;

  long output = Kp * P + Ki * errorSum + Kd * D;

  int leftMotor = baseSpeed - output;
  int rightMotor = baseSpeed + output;

  motor(constrain(leftMotor, -255, 255), constrain(rightMotor, -255, 255));
}

void motor(int leftSpeed, int rightSpeed) {
  // Left Motor
  if (leftSpeed >= 0) {
    digitalWrite(LEFT_DIR1, HIGH);
    digitalWrite(LEFT_DIR2, LOW);
    analogWrite(LEFT_EN, leftSpeed);
  } else {
    digitalWrite(LEFT_DIR1, LOW);
    digitalWrite(LEFT_DIR2, HIGH);
    analogWrite(LEFT_EN, -leftSpeed);
  }

  // Right Motor
  if (rightSpeed >= 0) {
    digitalWrite(RIGHT_DIR1, HIGH);
    digitalWrite(RIGHT_DIR2, LOW);
    analogWrite(RIGHT_EN, rightSpeed);
  } else {
    digitalWrite(RIGHT_DIR1, LOW);
    digitalWrite(RIGHT_DIR2, HIGH);
    analogWrite(RIGHT_EN, -rightSpeed);
  }
}