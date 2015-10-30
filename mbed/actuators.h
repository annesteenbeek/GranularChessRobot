#ifndef ACTUATORS_H
#define ACTUATORS_H

#include "PID.h"
#include "mbed.h"
#include "config.h"
#include "encoder.h"
#include "buttons.h"
#include "serialcom.h"

extern DigitalOut  redLed;
extern DigitalOut  greenLed;
extern DigitalOut  blueLed;

extern bool motorsEnable;
extern bool safetyOn;

// CCW is false(positive rotation), CW is true (neg rotation)
extern double motor1Pos;
extern double motor2Pos;

extern double motor1SetSpeed;
extern double motor2SetSpeed;
extern double setXSpeed;
extern double setYSpeed;
extern double servoSpeed;

// unscaled EMG velocity's
extern double x_velocity;
extern double y_velocity;
extern double z_velocity;

extern double servoSpeed;
extern double motorCall;

extern bool calReady;

// Set PID values
extern PID motor2PID;
extern bool usePotmeters;
extern bool controlAngle;
extern bool controlDirection;

extern bool enablePump;


void motorInit();
void motorControl();
void servoControl();
bool calibrateMotors();
void writeMotors();
void safety();
bool kinematics();

#endif 