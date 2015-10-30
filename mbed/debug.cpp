#include "mbed.h"
#include "debug.h"
#include "HIDScope.h"
#include "buttons.h"
#include "actuators.h"
#include "config.h"
// all the debugging functions

// #define TUNEPID  // set to switch between normal opperation or tuning PID
// #define TUNEEMG // set hdiscope for EMG
#define TUNEPWM // set hidscope for rest

#ifdef TUNEPID
HIDScope scope(5);

void debugProcess(){
    scope.set(0, motor2PID.getKp());
	scope.set(1, motor2PID.getKi());
	scope.set(2, motor2PID.getKd());
	scope.set(3, motor2SetSpeed);
	scope.set(4, motor2Speed);
	scope.set(5, motor2PWM);
    scope.send();
}

#endif
#ifdef TUNEPWM
HIDScope scope(6);

void debugProcess(){
    scope.set(0, motor1SetSpeed);
	scope.set(1, motor2SetSpeed);
	scope.set(2, setXSpeed);
	scope.set(3, motor1PWM);
	scope.set(4, motor2PWM);
    scope.set(5, Xpos);
    scope.send();
}

#endif

#ifdef TUNEEMG
HIDScope    scope(5);                   // Number of scopes

void debugProcess(){
    scope.set(0,y_velocity);
    scope.set(1,motor2SetSpeed);
    scope.set(2,z_velocity);
    scope.set(3,mode);
    scope.set(4,DOF);
    scope.set(5,motorsEnable);
    scope.send();
}
#endif