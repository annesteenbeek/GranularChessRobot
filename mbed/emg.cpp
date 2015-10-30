#include "emg.h"
#include "filter_constants.h"           // All constants for EMG processing
#include "mbed.h"
#include "buttons.h"

// Define objects
AnalogIn    emg1(A0);                   // Analog input 1
AnalogIn    emg2(A1);                   // Analog input 2
Ticker      sample_tick;                // Ticker for sampling
Ticker      output;                     // Ticker for PC output
Timer       normalizing_timer;          // Timer for normalizing
Timer       EMG_timer;                  // Timer for switch statement

// Define program constants
const int     on = 0;                   // On-constant for LEDs for program readability
const int     off = 1;                  // Off-constant for LEDs for program readability

bool ledsEnable = false;
// Initialize sampling constants
double emg_val1 = 0, emg_val2 = 0, emg_filt_val1 = 0, emg_filt_val2 = 0;

// Initialize normalizing parameters
double        max_vol_cont1     = 0;         // Maximum voluntary contraction for scaling EMG1
double        max_vol_cont2     = 0;         // Maximum voluntary contraction for scaling EMG2
int           channel           = 1;         // Channel for normalizing (EMG1 or EMG2)

// Initialize movement parameters
int         DOF = 1;                         // Switch variable for controlled DOF: 1=x 2=y 3=z
bool        enablePump = false;              // Pump switch
bool        thr_pass1 = false;               // Processing threshold passed for signal 1?
bool        thr_pass2 = false;               // Processing threshold passed for signal 2?
double      velocity = 0;                    // Forward velocity
double      x_velocity = 0;                  // x component for velocity
double      y_velocity = 0;                  // y component for velocity
double      z_velocity = 0;                  // z component for velocity


// Reusable BiQuad filter
double biquad(double u, double &v1, double &v2, const double a1, const double a2, const double b0, const double b1, const double b2, const double gain)
{
    double v = u - a1*v1-a2*v2;
    double y = gain*(b0*v+b1*v1+b2*v2);
    v2 = v1;
    v1=v;
    return y;
}


// Apply filters: applies all necesary filters and averaging methods to input signal value
double filtera(double emg_val)
{
    // Filtering signal
    double emg_filt_val;
    emg_filt_val = biquad(emg_val,f11_v1,f11_v2,f11_a1,f11_a2,f11_b0,f11_b1,f11_b2,f11_gain);      // Apply bandstop
    emg_filt_val = biquad(emg_filt_val,f12_v1,f12_v2,f12_a1,f12_a2,f12_b0,f12_b1,f12_b2,f12_gain);
    emg_filt_val = biquad(emg_filt_val,f13_v1,f13_v2,f13_a1,f13_a2,f13_b0,f13_b1,f13_b2,f13_gain);
    emg_filt_val = biquad(emg_filt_val,f21_v1,f21_v2,f21_a1,f21_a2,f21_b0,f21_b1,f21_b2,f21_gain); // Apply highpass
    emg_filt_val = biquad(emg_filt_val,f22_v1,f22_v2,f22_a1,f22_a2,f22_b0,f22_b1,f22_b2,f22_gain);
    emg_filt_val = biquad(emg_filt_val,f23_v1,f23_v2,f23_a1,f23_a2,f23_b0,f23_b1,f23_b2,f23_gain);
    //emg_filt_val = biquad(emg_filt_val,f31_v1,f31_v2,f31_a1,f31_a2,f31_b0,f31_b1,f31_b2,f31_gain); // Apply lowpass
    //emg_filt_val = biquad(emg_filt_val,f32_v1,f32_v2,f32_a1,f32_a2,f32_b0,f32_b1,f32_b2,f32_gain);
    //emg_filt_val = biquad(emg_filt_val,f33_v1,f33_v2,f33_a1,f33_a2,f33_b0,f33_b1,f33_b2,f33_gain);

    // Rectify signal
    emg_filt_val = fabs(emg_filt_val);

    // Averaging signal
    emg_filt_val = biquad(emg_filt_val,a11_v1,a11_v2,a11_a1,a11_a2,a11_b0,a11_b1,a11_b2,a11_gain); // Apply avg. lowpass
    emg_filt_val = biquad(emg_filt_val,a12_v1,a12_v2,a12_a1,a12_a2,a12_b0,a12_b1,a12_b2,a12_gain);
    emg_filt_val = biquad(emg_filt_val,a13_v1,a13_v2,a13_a1,a13_a2,a13_b0,a13_b1,a13_b2,a13_gain);

    return emg_filt_val;
}
double filterb(double emg_val)
{
    // Filtering signal
    double emg_filt_val;
    emg_filt_val = biquad(emg_val,f11_v1b,f11_v2b,f11_a1,f11_a2,f11_b0,f11_b1,f11_b2,f11_gain);      // Apply bandstop
    emg_filt_val = biquad(emg_filt_val,f12_v1b,f12_v2b,f12_a1,f12_a2,f12_b0,f12_b1,f12_b2,f12_gain);
    emg_filt_val = biquad(emg_filt_val,f13_v1b,f13_v2b,f13_a1,f13_a2,f13_b0,f13_b1,f13_b2,f13_gain);
    emg_filt_val = biquad(emg_filt_val,f21_v1b,f21_v2b,f21_a1,f21_a2,f21_b0,f21_b1,f21_b2,f21_gain); // Apply highpass
    emg_filt_val = biquad(emg_filt_val,f22_v1b,f22_v2b,f22_a1,f22_a2,f22_b0,f22_b1,f22_b2,f22_gain);
    emg_filt_val = biquad(emg_filt_val,f23_v1b,f23_v2b,f23_a1,f23_a2,f23_b0,f23_b1,f23_b2,f23_gain);
    //emg_filt_val = biquad(emg_filt_val,f31_v1b,f31_v2b,f31_a1,f31_a2,f31_b0,f31_b1,f31_b2,f31_gain); // Apply lowpass
    //emg_filt_val = biquad(emg_filt_val,f32_v1b,f32_v2b,f32_a1,f32_a2,f32_b0,f32_b1,f32_b2,f32_gain);
    //emg_filt_val = biquad(emg_filt_val,f33_v1b,f33_v2b,f33_a1,f33_a2,f33_b0,f33_b1,f33_b2,f33_gain);

    // Rectify signal
    emg_filt_val = fabs(emg_filt_val);

    // Averaging signal
    emg_filt_val = biquad(emg_filt_val,a11_v1b,a11_v2b,a11_a1,a11_a2,a11_b0,a11_b1,a11_b2,a11_gain); // Apply avg. lowpass
    emg_filt_val = biquad(emg_filt_val,a12_v1b,a12_v2b,a12_a1,a12_a2,a12_b0,a12_b1,a12_b2,a12_gain);
    emg_filt_val = biquad(emg_filt_val,a13_v1b,a13_v2b,a13_a1,a13_a2,a13_b0,a13_b1,a13_b2,a13_gain);

    return emg_filt_val;
}

// Create velocity steps: Converts continious velocity input signal to stepped output
double velocity_step(double velocity)
{
    if (velocity <= 0.33) {
        velocity=0.33;
    } else if(velocity <= 0.66) {
        velocity=0.66;
    } else if(velocity > 0.66) {
        velocity=1;
    }
    return velocity;
}

// Output velocity components
void EMG_velocity(double &x_velocity, double &y_velocity, double &z_velocity, double emg_val, bool thr_pass1, int &DOF, int emg_time)
{
    if (emg_time == 0) {
        EMG_timer.start();
    }

    if (emg_val > LPT) {
        if (emg_time > LST) {
            if(emg_time > UST) {
                // Output velocities -------------------------------------------------------------------
                velocity = (emg_val - LPT)*(1/(1-LPT));

                if(thr_pass1) {
                    velocity = velocity_step(velocity);
                } else {
                    velocity = - velocity_step(velocity);
                }

                switch(DOF) {
                    case 1 :
                        x_velocity = velocity;
                        y_velocity = 0;
                        z_velocity = 0;
                        break;
                    case 2 :
                        x_velocity = 0;
                        y_velocity = velocity;
                        z_velocity = 0;
                        break;
                    case 3 :
                        x_velocity = 0;
                        y_velocity = 0;

                        // z-velocity is not controlled continiously, but set to fixed value +/- 1
                        if(velocity > 0) {
                            z_velocity = 1;
                        } else if( velocity < 0) {
                            z_velocity = -1;
                        } else {
                            z_velocity = 0;
                        }
                        break;
                }
            }
        }
    } else {
        if((emg_time > LST)&&(emg_time < UST)) {
            if(thr_pass1) {
                // Switch channel  -----------------------------------------------------------------------
                DOF = DOF + 1;
                if (DOF == 4) {
                    DOF = 1;
                }
            } else {
                // Switch pump ---------------------------------------------------------------------------
                enablePump = !enablePump;
            }
        }
        // No input: set all value to zero ---------------------------------------------------------------
        x_velocity = 0;
        y_velocity = 0;
        z_velocity = 0;
        thr_pass1 = false;
        thr_pass2 = false;
        EMG_timer.stop();
        EMG_timer.reset();
    }
}

void EMG_check(bool &thr_pass1, bool &thr_pass2, double &x_velocity, double &y_velocity, double &z_velocity, double emg_filt_val1, double emg_filt_val2, int emg_time)
{
    if (emg_filt_val1 > UPT) {
        thr_pass1 = true;
    }
    if (emg_filt_val2 > UPT) {
        thr_pass2 = true;
    }

    if ((thr_pass1 == true) && (thr_pass2 == true)) {
        // If both true terminate
        thr_pass1 = false;
        thr_pass2 = false;
        EMG_timer.stop();
        EMG_timer.reset();
        x_velocity = 0;
        y_velocity = 0;
        z_velocity = 0;

    } else {
        if(thr_pass1) {
            EMG_velocity(x_velocity, y_velocity,  z_velocity, emg_filt_val1, thr_pass1, DOF, emg_time);
        } else if(thr_pass2) {
            EMG_velocity(x_velocity, y_velocity,  z_velocity, emg_filt_val2, thr_pass1, DOF, emg_time);
        }
    }
}


void readEMG(){
    if(mode==normalize && normalizing_timer.read_ms() == 0) {   // Start normalizing timer
        normalizing_timer.reset();
        normalizing_timer.start();
    }

    if(mode ==normalize) {
        emg_val1 = emg1.read();               // Sample EMG value 1 from AnalogIn
        emg_val2 = emg2.read();               // Sample EMG value 2 from AnalogIn

        emg_filt_val1 = filtera(emg_val1);    // Filter and average signal
        emg_filt_val2 = filterb(emg_val2);    // Filter and average signal


        /* Determining normalizing constants
        - Read value from EMG and average as above
        - If mode "normalizing" and normalizing timer below ending time:
            * store averaged EMG value 1 in max_vol_cont1 while overwriting previous value when current value > previous value
        - If normalizing time exceeded, wait for given time
        - Switch channels: redo above procedure for EMG value 2
        - Switch to sampling mode
        */

        // First normalizing step: channel 1
        if (normalizing_timer.read_ms() <= normalize_time && channel == 1) {
            if (ledsEnable){
                redLed.write(off);
                greenLed.write(on);
            }
            if (emg_filt_val1 > max_vol_cont1) {
                max_vol_cont1 = emg_filt_val1;
            }
            // Second normalizing step: wait time, switch channel
        } else if (normalizing_timer.read_ms() > normalize_time && channel == 1) {
            channel = 2;
            if (ledsEnable){
                greenLed.write(off);
                redLed.write(on);
            }
            // Third normalizing step: channel 2
        } else if (normalizing_timer.read_ms() >= (normalize_time + normalize_wait) && normalizing_timer.read_ms() <= (2*normalize_time + normalize_wait) && channel == 2) {
            if (ledsEnable){
                redLed.write(off);
                greenLed.write(on);
            }
            if (emg_filt_val2 > max_vol_cont2) {
                max_vol_cont2 = emg_filt_val2;
            }
            // Final normalizing step: stop normalizing process, start outputting velocities
        } else if (normalizing_timer.read_ms() > (2*normalize_time + normalize_wait)) {
            normalizing_timer.stop();
            normalizing_timer.reset();
            if (ledsEnable){
                greenLed.write(off);
                redLed.write(off);
                blueLed.write(on);
            }
            mode = sample;
        }
    }


    if(mode ==sample) {
        emg_val1 = emg1.read();                             // Sample EMG value 1 from AnalogIn
        emg_val2 = emg2.read();                             // Sample EMG value 2 from AnalogIn

        // Normalize EMG values using pre-determined coefficients
        if(max_vol_cont1 != 0 && max_vol_cont2 != 0) {      // Safety check: normalizing coefficients have to be set first
            emg_val1 = emg_val1 / max_vol_cont1;            // Normalize EMG-value 1
            emg_val2 = emg_val2 / max_vol_cont2;            // Normalize EMG-value 2

            emg_filt_val1 = filtera(emg_val1);              // Filter and average signal 1
            emg_filt_val2 = filterb(emg_val2);              // Filter and average signal 2

            if (emg_filt_val1 > 1) {                        // Safety-function: set max. output to 1, even if muscle contraction exceeds max. voluntary contraction
                emg_filt_val1 = 1;
            }
            if (emg_filt_val2 > 1) {                        // Safety-function: set max. output to 1, even if muscle contraction exceeds max. voluntary contraction
                emg_filt_val2 = 1;
            }

            /* Checking EMG input:
            -   Make sure only one DOF is actuated at a time
            -   Distinct between switching function and motion from one EMG-signal
            */

            EMG_check(thr_pass1, thr_pass2, x_velocity, y_velocity, z_velocity, emg_filt_val1, emg_filt_val2, EMG_timer.read_ms());

        } else {
            if (ledsEnable){
                greenLed.write(off);
                redLed.write(on);
                blueLed.write(off);
            }
        }
    }

    // Graphical output to HIDScope for debugging/ program check
}

