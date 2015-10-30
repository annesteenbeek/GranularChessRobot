#ifndef BUTTONS_H
#define BUTTONS_H
#include "PID.h"

extern double motor1SetSpeed;
extern double motor2SetSpeed;
extern bool motorsEnable;
extern double servoPos;
extern PID motor2PID;
extern bool usePotmeters;
extern bool controlAngle;
extern bool controlDirection;

	void setPins();
	void checkSwitches();
	void setForPositions();
	void setForPID();

#endif