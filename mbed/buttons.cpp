#include "buttons.h"
#include "mbed.h"
#include "config.h"
#include "actuators.h"
#include "PID.h"

#define SETPOS


// functions for reading all the buttons and switches 
AnalogIn pot1(pot1Pin);
AnalogIn pot2(pot2Pin);

// Led states: 	
// 	0 leds: motors disabled
//	redLed: 	control X speed
//	greenLed:	control Y speed
//	blueLed:	control Servo pos
DigitalOut redLed(LED_RED);
DigitalOut greenLed(LED_GREEN);
DigitalOut blueLed(LED_BLUE);

DigitalIn button1(button1Pin);
DigitalIn button2(button2Pin);
bool button1Pressed = false;
bool button2Pressed = false;
int actuatorState = 0;
int PIDparam = 0;

double pot1Val = 0;
double pot2Val = 0;
double Kp = 1;
double Ki = 0;
double Kd = 0;


void checkSwitches(){

	// read potmeter values
    pot1Val = pot1.read();
    pot2Val = pot2.read();

    // button 1 pressed
    if(button1.read() == 0){
		if (button1Pressed != true){
			motorsEnable = !motorsEnable;
			button1Pressed = true;
		}
	}else{
		button1Pressed = false;
	}

	#ifdef TUNEPID
		setForPID(); // if TUNEPID is defined, use potmeters for PID tuning
	#endif
	#ifdef SETPOS
	if(usePotmeters){
		setForPositions(); // else use it for motor control
	}
	#endif
}

void setForPID(){
	if(button2.read() == 0){
		if(button2Pressed != true){
			PIDparam++;
			if(PIDparam==3){
				PIDparam = 0;
			}
		button2Pressed = true;
		}
	}else{
		button2Pressed = false;
	}

	if(motorsEnable){
		motor2SetSpeed = 300*pot2.read();
    	switch (PIDparam){  
        	case 0: 				// potmeters control P gain
        		redLed.write(0); greenLed.write(1); blueLed.write(1);
        		Kp =3* pot1.read()+1;
        		break;
        	case 1:         		// potmeters control I gain
        		redLed.write(1); greenLed.write(0); blueLed.write(1);
        		Ki = pot1.read();
        		break;
        	case 2:         		// potmeters control D gain
        		redLed.write(1); greenLed.write(1); blueLed.write(0);
        		Kd = pot1.read();
        		break;
        }
        motor2PID.SetTunings(Kp, Ki, Kd);
    }else{
    	redLed.write(1); greenLed.write(1); blueLed.write(1);
    }
}

void setForPositions(){
		// 3 states: X control, Y control en Servo control
	//	 button 1 to enable/disable actuators
	// 	 button 2 to switch between states


	if(button2.read() == 0){
		if(button2Pressed != true){
			actuatorState++;
			if(actuatorState==3){
				actuatorState = 0;
			}
		button2Pressed = true;
		}
	}else{
		button2Pressed = false;
	}



    if(motorsEnable){
    	switch (actuatorState){
        	case 0: 				// potmeters control X speed
        		redLed.write(0); greenLed.write(1); blueLed.write(1);
        		setXSpeed = 0.1*(pot2.read()-pot1.read());
        		break;
        	case 1:         		// potmeters control Y speed
        		redLed.write(1); greenLed.write(0); blueLed.write(1);
        		setYSpeed = 0.1*(pot2.read()-pot1.read());
        		break;
        	case 2:         		// potmeters control Servo pos
        		redLed.write(1); greenLed.write(1); blueLed.write(0);
        		servoSpeed = pot2.read();
        		break;
        }
    }else{
    	redLed.write(1); greenLed.write(1); blueLed.write(1);
    }
}
