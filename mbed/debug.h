#ifndef DEBUG_H
#define DEBUG_H

#include "PID.h"
#include "mbed.h"
#include "emg.h"

extern double motor2SetSpeed;
extern double motor2Speed;
extern double motor2Pos;
extern double motor2PWM;
extern double motor1PWM;
extern double motor1SetSpeed;
extern bool motorsEnable;
extern int actuatorState;
extern double prevTime;
extern double servoSpeed;

extern double Xpos;
extern double Ypos;

extern PID motor2PID;
extern DigitalIn button2;
extern AnalogIn pot2;
extern AnalogIn pot1;
extern double Kp, Ki, Kd;

extern double emg_filt_val1;
extern double emg_filt_val2;
extern double x_velocity;
extern double y_velocity;
extern double z_velocity;
extern bool enablePump;
extern int DOF;
extern bool mode;


void debugProcess();

#endif