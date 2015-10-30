#include "mbed.h"
#include "filter_constants.h"
#include "emg.h"

/*------------------------------------
    All constants for EMG processing
------------------------------------*/

const double sample_frequency = 200;         // Sample frequency [Hz]

// Define normalizing parameters
const int     normalize_time    = 2000;      // Normalizing time [ms]
const int     normalize_wait    = 4000;      // Normalizing wait time [ms]

// Define thresholds
const double UPT = 0.35;                     // Upper Treshold Value     (0-1)
const double LPT = 0.2;                      // Lower Treshold Vvalue     (0-1)
const double LST = 100;                      // Lower Switch Time        [ms]
const double UST = 500;                      // Upper Switch Time        [ms]

// Define filter constants
double f11_v1=0, f11_v2=0, f12_v1=0, f12_v2=0, f13_v1=0, f13_v2=0;
double f11_v1b=0, f11_v2b=0, f12_v1b=0, f12_v2b=0, f13_v1b=0, f13_v2b=0;
const double f11_gain = 0.907616, f11_a1 = 0.00000000267,  f11_a2 = 0.77693864001, f11_b0 = 1, f11_b1 = 0.00000000301, f11_b2 = 1; // Constants for pre-filter BiQuad: Bandstop 48-52Hz     #1
const double f12_gain = 0.857718, f12_a1 = -0.20392089403, f12_a2 = 0.88318656585, f12_b0 = 1, f12_b1 = 0.00000000301, f12_b2 = 1; // Constants for pre-filter BiQuad: Bandstop 48-52Hz     #2
const double f13_gain = 1       , f13_a1 = 0.20392089964,  f13_a2 = 0.88318656589, f13_b0 = 1, f13_b1 = 0.00000000301, f13_b2 = 1; // Constants for pre-filter BiQuad: Bandstop 48-52Hz     #3

double f21_v1=0, f21_v2=0, f22_v1=0, f22_v2=0, f23_v1=0, f23_v2=0;
double f21_v1b=0, f21_v2b=0, f22_v1b=0, f22_v2b=0, f23_v1b=0, f23_v2b=0;
const double f21_gain = 0.571868, f21_a1 = -1.01850473939, f21_a2 = 0.26893113176, f21_b0 = 1, f21_b1 = -2, f21_b2 =  1; // Constants for pre-filter BiQuad: Highpass 20 Hz        #1
const double f22_gain = 0.633951, f22_a1 = -1.12909340502, f22_a2 = 0.40671095272, f22_b0 = 1, f22_b1 = -2, f22_b2 =  1; // Constants for pre-filter BiQuad: Highpass 20 Hz        #2
const double f23_gain = 0.780779, f23_a1 = -1.39062059536, f23_a2 = 0.73254153630, f23_b0 = 1, f23_b1 = -2, f23_b2 =  1; // Constants for pre-filter BiQuad: Highpass 20 Hz        #3

//double f31_v1=0, f31_v2=0, f32_v1=0, f32_v2=0, f33_v1=0, f33_v2=0;
//double f31_v1b=0, f31_v2b=0, f32_v1b=0, f32_v2b=0, f33_v1b=0, f33_v2b=0;
//const double f31_gain = 0.130049, f31_a1 = -0.57789344562, f31_a2 = 0.09809080329, f31_b0 = 1, f31_b1 = 2, f31_b2 = 1; // Constants for pre-filter BiQuad: Lowpass  30 Hz       #1
//const double f32_gain = 0.147923, f32_a1 = -0.65731925030, f32_a2 = 0.24901264940, f32_b0 = 1, f32_b1 = 2, f32_b2 = 1; // Constants for pre-filter BiQuad: Lowpass  30 Hz       #2
//const double f33_gain = 0.194139, f33_a1 = -0.86268420183, f33_a2 = 0.63923919773, f33_b0 = 1, f33_b1 = 2, f33_b2 = 1; // Constants for pre-filter BiQuad: Lowpass  30 Hz       #3

double a11_v1=0, a11_v2=0, a12_v1=0, a12_v2=0, a13_v1=0, a13_v2=0;
double a11_v1b=0, a11_v2b=0, a12_v1b=0, a12_v2b=0, a13_v1b=0, a13_v2b=0;
const double a11_gain = 0.001275, a11_a1 = -1.861713, a11_a2 = 0.866812, a11_b0 = 1, a11_b1 = 2, a11_b2 = 1; // Constants for averaging BiQuad:   Lowpass  4 Hz       #1
const double a12_gain = 0.001298, a12_a1 = -1.895537, a12_a2 = 0.900729, a12_b0 = 1, a12_b1 = 2, a12_b2 = 1; // Constants for averaging BiQuad:   Lowpass  4 Hz       #2
const double a13_gain = 0.001340, a13_a1 = -1.957123, a13_a2 = 0.962484, a13_b0 = 1, a13_b1 = 2, a13_b2 = 1; // Constants for averaging BiQuad:   Lowpass  4 Hz       #3
