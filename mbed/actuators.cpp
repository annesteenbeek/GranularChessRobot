#include "actuators.h"
#include "PID.h"
#include "mbed.h"
#include "config.h"
#include "encoder.h"
#include "HIDScope.h"
#include "buttons.h"
#include "Servo.h"

// Motor control constants
#define pwm_frequency 50000 // still High, could be lowered
#define PI 3.14159265

// functions for controlling the motors
bool motorsEnable = false;
bool safetyOn = true; 


double encoder1Counts = 0;
double encoder2Counts = 0;

bool direction1 = false; // CCW is false(positive rotation), CW is true (neg rotation)
bool direction2 = false;

double motor1Pos = 0;
double motor2Pos = 0;

double motor1Speed = 0;
double motor2Speed = 0;

double motor1SetSpeed = 0;
double motor2SetSpeed = 0;

double setXSpeed = 0;
double setYSpeed = 0;

double motor1PWM = 0;
double motor2PWM = 0;

// Set PID values
double Kp1 = 0.008; 
double Ki1 = 0.08; 
double Kd1 = 0;

double Kp2 = 0.008; 
double Ki2 = 0.08; 
double Kd2 = 0;

double motor1PrevCounts = 0;
double motor2PrevCounts = 0;
double prevTime = 0;
double now = 0;
double timechange;

// Set servo values
double servoSpeed = 0;
const int servoStartPos = 1300; // Servo ranges from 600ms(-90) to 2000ms(90), 1300 is 0 deg

// Create object instances
// Safety Pin
DigitalIn safetyIn(safetyPin);

// Initialze motors
PwmOut motor1(motor1PWMPin);
PwmOut motor2(motor2PWMPin);

// initialize Servo
Servo servo(servoPin);


// Initialize encoders
Encoder encoder1(enc1A, enc1B);
Encoder encoder2(enc2A, enc2B);

// Set direction pins     
DigitalOut motor1Dir(motor1DirPin);
DigitalOut motor2Dir(motor2DirPin);  

// create PID instances
PID motor1PID(&motor1Speed, &motor1PWM, &motor1SetSpeed, Kp1, Ki1, Kd1);
PID motor2PID(&motor2Speed, &motor2PWM, &motor2SetSpeed, Kp2, Ki2, Kd2);

// set pumpswitch
DigitalOut pumpSwitch(pumpPin);


Timer t;  
    
void motorInit(){
    
    motor1Dir.write(direction1);
    motor2Dir.write(direction2);

    // Set motor PWM period
    motor1.period(1/pwm_frequency);
    motor2.period(1/pwm_frequency);
    
    motor1PID.SetSampleTime(motorCall);
    motor2PID.SetSampleTime(motorCall);
    
    motor1PID.SetOutputLimits(0,1);
    motor2PID.SetOutputLimits(0,1);
    
    // Turn PID on
    motor1PID.SetMode(AUTOMATIC);
    motor2PID.SetMode(AUTOMATIC);

    // set servo period
    servo.Enable(servoStartPos, 20000);               

    // start the timer
    t.start();
}


void motorControl(){
  // EMG signals to motor speeds
  if(!usePotmeters && controlAngle){
      double scaleVel = 20;
      motor1SetSpeed = x_velocity*scaleVel;
      motor2SetSpeed = y_velocity*scaleVel;
      servoSpeed = z_velocity*scaleVel;
  }
  if(!usePotmeters && controlDirection){
      double scaleVel = 0.1;
      setXSpeed = x_velocity*scaleVel;
      setYSpeed = y_velocity*scaleVel;
      servoSpeed = z_velocity*scaleVel;
    }
    // get encoder positions in degrees
        // 131.25:1 gear ratio
        // getPosition uses X2 configuration, so 32 counts per revolution
        // encoder reads CCW negative, and CW positive, so multiply by -1 to make CCW positive

        encoder1Counts = encoder1.getPosition();
        encoder2Counts = encoder2.getPosition();

        motor1Pos = ((encoder1Counts/32)/131.25)*360;
        motor2Pos = ((encoder2Counts/32)/131.25)*360;

        // check if motor's are within rotational boundarys
    // get  encoder speeds in deg/sec
        now = t.read(); 
        timechange = (now - prevTime);
        motor1Speed = ((((encoder1Counts - motor1PrevCounts)/32)/131.25)*360)/timechange;
        motor2Speed = ((((encoder2Counts - motor2PrevCounts)/32)/131.25)*360)/timechange;
        prevTime = now;
        motor1PrevCounts = encoder1Counts;
        motor2PrevCounts = encoder2Counts;
        
    // calculate motor setpoint speed in deg/sec from setpoint x/y speed

    // exclude kinematics when still calibrating
    if (calReady && !controlAngle){
      kinematics();
    }

    if(motorsEnable){  // only run motors if switch is enabled
    // compute new PID parameters using setpoint angle speeds and encoder speed
        writeMotors();
        servoControl();
    }else{
        // write 0 to motors
        motor1.write(0);
        motor2.write(0);
    }
}

void writeMotors(){
    motor1PID.Compute(); // calculate PID outputs, output changes automatically
    motor2PID.Compute();
    // write new values to motor's
    if (motor1SetSpeed ==0 ){
      motor1PID.SetOutputLimits(0,0);
    }
    if (motor1SetSpeed > 0 ){ // CCW rotation 
        direction1 = true;
        motor1PID.SetOutputLimits(0,1); // change pid output direction
    }else{
        direction1 = false; // CW rotation
        motor1PID.SetOutputLimits(-1,0);
    }
    if (motor2SetSpeed ==0 ){
      motor2PID.SetOutputLimits(0,0);
    }
    if (motor2SetSpeed > 0 ){ // CCW rotation 
        direction2 = true;
        motor2PID.SetOutputLimits(0,1);
    }else{
        direction2 = false; // CW rotation
        motor2PID.SetOutputLimits(-1,0);
    }
    motor1Dir.write(direction1);
    motor2Dir.write(direction2);

    motor1.write(abs(motor1PWM));
    motor2.write(abs(motor2PWM));
}

void servoControl(){
    servo.SetPosition(1300 + 700*servoSpeed);
}

const double motor1StartPos = (-10*32*131.25)/360; // angle to encoder counts
const double motor2StartPos = (120*32*131.25)/360; 

// Set calibration values
double motor1CalSpeed = -10; // deg/sec
double motor2CalSpeed = 5;
bool springHit = false;
float lastCall = 0;
bool calibrating1 = true;
bool calibrating2 = false;
double looseTime = 0;

bool calibrateMotors(){
   safetyOn = false; // safety springs off
   motorsEnable = true; // motors on
   while (calibrating1 || calibrating2){
       if (calibrating1){
            motor1SetSpeed = motor1CalSpeed;
            redLed.write(1); greenLed.write(0); blueLed.write(1);
           if(safetyIn.read() !=1 && !springHit){ // check if arm reached safety position
               encoder1.setPosition(motor1StartPos); // set motor 1 cal angle
               motor1PrevCounts = motor1StartPos; // set previous count to prevent speed spike
               motor1CalSpeed = -motor1CalSpeed; // move away
               springHit = true;
               looseTime = t.read(); // timer to compensate spring movement
           }else{ 
              // if hit and after is no longer touching spring and 0.5seconds passed
               if(springHit && ((t.read() - looseTime) > 2)){ 
                motor1SetSpeed = 0;
                springHit = false;
                calibrating2 = true; // start calibrating 2
                calibrating1 = false;
               }
           }
       }
       if (calibrating2){
            motor2SetSpeed = motor2CalSpeed; 
            redLed.write(1); greenLed.write(1); blueLed.write(0);
           if(safetyIn.read() !=1 && !springHit){ // check if arm reached safety position
               encoder2.setPosition(motor2StartPos); // set motor 2 cal angle
               motor2PrevCounts = motor2StartPos; // set previous cunt to prevent speed spike
               motor2CalSpeed = -motor2CalSpeed; // move away
               springHit = true;
               looseTime = t.read();
           }else{ 
               if(springHit && ((t.read() - looseTime) > 2)){ 
                motor2SetSpeed = 0;
                springHit = false;
                calibrating2 = false; // stop calibrating 2
               }
           }
       }
       // now = t.read(); // call motor using timer instead of wait
       // if(now - lastCall > motorCall){
           motorControl();
           wait(motorCall); // keep calling PID's with motorCall frequency
           // lastCall = now;
       // }

   }
    redLed.write(0); greenLed.write(1); blueLed.write(1);
    motorsEnable = false; // turn motor's off again
    safetyOn = true; // turn safety on after callibration
    return true; // return true when finished
}


void safety(){
    if (safetyOn){
        if (safetyIn.read() != 1){
            motorsEnable = false;
        }
    }
}

const double L1 = 0.436; // first arm in m
const double L2 = 0.120; // first arm in m
const double L3 = 0.255; // servo arm in m
const double Xmax = 0.3;
const double Xmin = -0.3;
const double Ymax = 0.645;
const double Ymin = 0.33;
double Xpos = 0; // set values
double Ypos = 0;



bool kinematics(){
  // calculate current x and Y
  Xpos = (L2+L3)*cos((motor1Pos + motor2Pos)*PI/180) + L1*cos(motor1Pos*PI/180);
  Ypos = (L2+L3)*sin((motor1Pos + motor2Pos)*PI/180) + L1*sin(motor1Pos*PI/180);
  // check if x and y are within limits
  //  else  Store the constraint line
  //      check if movement is in direction of constraint
  //      else return false no movement (anglespeed = 0)
  // calculate required angle speeds
  if( (Xpos>Xmax && setXSpeed > 0 )|| \
      (Xpos<Xmin && setXSpeed < 0 )|| \
      (Ypos>Ymax && setYSpeed > 0 )|| \
      (Ypos<Ymin && setYSpeed < 0 )   \
  ){
      motor1SetSpeed = 0;
      motor2SetSpeed = 0;
      return false;
  }
  motor1SetSpeed = (180/PI)*((setXSpeed*cos((motor1Pos + motor2Pos)*PI/180) + \
      setYSpeed*sin((motor1Pos + motor2Pos)*PI/180))/(L1*sin(motor2Pos*PI/180)));
  motor2SetSpeed = (180/PI)*(-(setXSpeed*(L2+L3)*cos((motor1Pos + motor2Pos)*PI/180) + \
      setYSpeed*(L2+L3)*sin((motor1Pos + motor2Pos)*PI/180) + \
      setXSpeed*L1*cos(motor1Pos*PI/180) + \
      setYSpeed*L1*sin(motor1Pos*PI/180))/(L1*(L2+L3)*sin(motor2Pos*PI/180)));
    return true;
}

void runPump(){
  pumpSwitch.write(enablePump);
}