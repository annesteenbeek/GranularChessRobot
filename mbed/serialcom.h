#ifndef SERIALCOM_H
#define SERIALCOM_H

#include "mbed.h"
#include "PID.h"
#include "buttons.h"
#include "config.h"
#include "emg.h"

extern PID motor1PID;
extern PID motor2PID;

extern DigitalOut  redLed;
extern DigitalOut  greenLed;
extern DigitalOut  blueLed;
extern bool enablePump;
extern bool startCalibration;
extern bool enableEMG;
extern bool usePotmeters;
extern bool controlAngle;
extern bool controlDirection;
extern bool enablePump;
extern bool startCalibration;
extern bool enableEMG;
extern bool usePotmeters;
extern bool controlAngle;
extern bool controlDirection;
extern bool calReady;
extern bool motorsEnable;
extern bool EMGCalReady;
extern double Xpos;
extern double Ypos;
extern double motor1Pos;
extern double motor2Pos;
extern int DOF;
extern int actuatorState;


void serialCom();
void serialInit();

#endif