/*
   ________                      ____        __          __ 
  / ____/ /_  ___  __________   / __ \____  / /_  ____  / /_
 / /   / __ \/ _ \/ ___/ ___/  / /_/ / __ \/ __ \/ __ \/ __/
/ /___/ / / /  __(__  |__  )  / _, _/ /_/ / /_/ / /_/ / /_  
\____/_/ /_/\___/____/____/  /_/ |_|\____/_.___/\____/\__/  
                                                            
*/

#include "mbed.h"
#include "config.h"     // settings and pin configurations
#include "actuators.h"  // all the actuator related functions
#include "buttons.h"    // reading switches and buttons
#include "debug.h"      // HIDscope
#include "emg.h"        // EMG
#include "serialcom.h"  // Communication with web app

Ticker switchesTick, debugTick, motorTick, EMGTick, safetyTick, serialTick;
volatile bool switches_go=false, debug_go=false, motor_go=false, emg_go=false, safety_go=false, serial_go=false;

void switches_activate(){switches_go=true;};
void debug_activate(){debug_go=true;};
void motor_activate(){motor_go=true;};
void emg_activate(){emg_go=true;};
void safety_activate(){safety_go=true;};
void serial_activate(){serial_go=true;};

void tickerAttach(){
    switchesTick.attach(&switches_activate, 0.02f);
    // debugTick.attach(&debug_activate, motorCall);
    motorTick.attach(&motor_activate, motorCall);
    safetyTick.attach(&safety_activate, 0.001f);
    
}

double motorCall = 0.01; // set motor frequency global so it can be used for speed.
const int sample = 0;  // Constant for EMG mode switching
const int normalize = 1; // Constant for EMG mode switching
bool mode = normalize; 
bool tickersActivated = false;
bool calReady = false; // flag for motor calibration

bool usePotmeters = true;  // flag for using the potmeters/EMG to control the system
bool controlAngle = false;  // control the system using motor angles
bool controlDirection = true; // control the system using kinematics
bool enableEMG = false;
bool startCalibration = false;
bool EMGCalReady = false;

int main(){
    serialInit();
    serialTick.attach(&serial_activate, 0.1f); // initialze serial communication first
    EMGTick.attach(&emg_activate, 0.005f);  
    motorInit();

    while (true) {
        if(serial_go){  // communication between serial and the webpage
            serial_go=false;
            serialCom();
        }
        if(startCalibration & !calReady){ // start calibration
            calReady = calibrateMotors();
        }
        if(emg_go){
            emg_go=false;
            if(enableEMG){
                readEMG(); 
            }
            if(mode==0){ 
                EMGCalReady = true;
            }
        }
        if(calReady && !tickersActivated){ // when done with calibration, start rest of tickers
            tickerAttach();
            tickersActivated = true;
        }
        if(safety_go){
            safety_go=false;
            safety(); // springs at the side to constrain arm movement
        }
        if(switches_go){
            switches_go=false;
            checkSwitches(); // read potmeters and buttons on the motor board
        }
        if(debug_go){
            debug_go=false;
            debugProcess(); // send data to HIDscope
        }
        if(motor_go){
            motor_go=false;
            motorControl();  // write data to the motors
        }
    }
}