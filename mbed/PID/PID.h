#ifndef PID_H
#define PID_H

class PID{
	public:
	#define MANUAL 0
	#define AUTOMATIC 1
	#define FORWARD 0
	#define REVERSE 1

	PID(double*, double*, double*, double, double, double);

	void SetMode(int Mode);

	bool Compute();  // returns false when in Manual mode

	void SetOutputLimits(double, double);

	void SetSampleTime(double);

	void SetTunings(double, double, double);
	void SetDirection(int);
	
	double getKp();
	double getKi();
	double getKd();

	private:
	void Initialize();
	int controllerDirection;
	double SampleTime;
	double *myInput;              // * Pointers to the Input, Output, and Setpoint variables
    double *myOutput;             //   This creates a hard link between the variables and the 
    double *mySetpoint;           //   PID, freeing the user from having to constantly tell us
                                  //   what these values are.  with pointers we'll just know.
	double error, ITerm, dErr; 
	double lastErr, lastInput;
	double kp, ki, kd;
	double outMin, outMax;
	bool inAuto;

};

#endif