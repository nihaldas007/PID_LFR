#pragma once
#include <Arduino.h>


float Kp = 0.05;
float Ki = 0.0;
float Kd = 0.8;

long error = 0, lastError = 0, errorSum = 0;
int baseSpeed = 120;

const int sensorPos[8] = {-3500, -2500, -1500, -500, 500, 1500, 2500, 3500};

int s[8];
int binary[] = {1, 2, 4, 8, 16, 32, 64, 128};
int sensor = 0;
int sum = 0;

const byte irPins[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
const int th = 420;


long int flag = 0, tflag = 0;
int k90 = 0;
int cross = 0;
int node = 10;
bool counter = true;

unsigned long m1, m2;
unsigned long stp = 30;
unsigned long resetTimer = 0;

int tsp = 80;
int tbr = 30;
int br = 10;
// int counter;
int epoint = 30;
const int ButtonPin = 11;