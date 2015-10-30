#include "mbed.h"
#include "serialcom.h"
//#include "MODSERIAL.h"
#include "config.h"
#include "PID.h"
#include "buttons.h"
#include <string>

Serial pc(USBTX, USBRX);
char buf[128];
int transmitDOF;
bool EMGCalReady = false;
bool enableEMG = false;
bool startCalibration = false;

void serialInit(){
    const int baudrate = 9600;
    redLed.write(1); // dim the leds
    blueLed.write(1);
    greenLed.write(1);
    
    pc.baud(baudrate);
    }

void serialCom(){
    // Receiving
    pc.scanf("%s", buf);
    if(strcmp(buf,"ledRed")==0){
        redLed.write(!redLed.read());
    }
    if(strcmp(buf,"ledBlue")==0){
        blueLed.write(!blueLed.read());
    }
    if(strcmp(buf,"ledGreen")==0){
        greenLed.write(!greenLed.read());
    }
    if(strcmp(buf, "switchPump")==0){
        enablePump = !enablePump;
    }
    if(strcmp(buf, "startCalibration")==0){
        startCalibration = true;
    }
    if(strcmp(buf, "enableEMG")==0){
        enableEMG = true;
        usePotmeters = false;
    }
    if(strcmp(buf, "usePotmeters")==0){
        usePotmeters = true;
        enableEMG = false;
    }
    if(strcmp(buf, "controlAngle")==0){
        controlAngle = true;
        controlDirection = false;
    }
    if(strcmp(buf, "controlDirection")==0){
        controlDirection = true;
        controlAngle = false;
    }

    // Writers
    pc.printf("enablePump %d\n", enablePump);
    pc.printf("startCalibration %d\n", startCalibration);
    pc.printf("enableEMG %d\n", enableEMG);
    pc.printf("usePotmeters %d\n", usePotmeters);
    pc.printf("controlAngle %d\n", controlAngle);
    pc.printf("controlDirection %d\n", controlDirection);
    pc.printf("calReady %d\n", calReady);
    pc.printf("motorsEnable %d\n", motorsEnable);
    pc.printf("EMGCalReady %d\n", EMGCalReady);
    pc.printf("Xpos %.3ld\n", Xpos);
    pc.printf("Ypos %.3ld\n", Ypos);
    pc.printf("motor1Pos %.3ld\n", motor1Pos);
    pc.printf("motor2Pos %.3ld\n", motor2Pos);

    // transmit correct DOF controller
    if(usePotmeters){
        transmitDOF = actuatorState; // DOF controlled by buttons
    }if(enableEMG){
        transmitDOF = DOF; // DOF controlled by EMG
    }
    pc.printf("DOF %i\n", transmitDOF);  // DOF: 1=x 2=y 3=z
}

