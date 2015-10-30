#ifndef EMG_H
#define EMG_H

#include "mbed.h"
#include "HIDScope.h"
#include "filter_constants.h"
#include "buttons.h"

extern DigitalOut  redLed;
extern DigitalOut  greenLed;
extern DigitalOut  blueLed;

extern const int sample;        // Constant for mode switching for program readability
extern const int normalize;        // Constant for mode switching for program readability
extern bool mode;   // Set program mode


extern const double sample_frequency; 

extern const int     normalize_time;
extern const int     normalize_wait;

extern const double UPT;
extern const double LPT;
extern const double LST;
extern const double UST;

extern double f11_v1, f11_v2, f12_v1, f12_v2, f13_v1, f13_v2;
extern double f11_v1b, f11_v2b, f12_v1b, f12_v2b, f13_v1b, f13_v2b;
extern const double f11_gain, f11_a1,  f11_a2, f11_b0, f11_b1, f11_b2; 
extern const double f12_gain, f12_a1, f12_a2, f12_b0, f12_b1, f12_b2; 
extern const double f13_gain, f13_a1,  f13_a2, f13_b0, f13_b1, f13_b2;

extern double f21_v1, f21_v2, f22_v1, f22_v2, f23_v1, f23_v2;
extern double f21_v1b, f21_v2b, f22_v1b, f22_v2b, f23_v1b, f23_v2b;
extern const double f21_gain, f21_a1,  f21_a2, f21_b0, f21_b1, f21_b2; 
extern const double f22_gain, f22_a1, f22_a2, f22_b0, f22_b1, f22_b2; 
extern const double f23_gain, f23_a1,  f23_a2, f23_b0, f23_b1, f23_b2;

extern double f31_v1, f31_v2, f32_v1, f32_v2, f33_v1, f33_v2;
extern double f31_v1b, f31_v2b, f32_v1b, f32_v2b, f33_v1b, f33_v2b;
extern const double f31_gain, f31_a1,  f31_a2, f31_b0, f31_b1, f31_b2; 
extern const double f32_gain, f32_a1, f32_a2, f32_b0, f32_b1, f32_b2; 
extern const double f33_gain, f33_a1,  f33_a2, f33_b0, f33_b1, f33_b2;

extern double a11_v1, a11_v2, a12_v1, a12_v2, a13_v1, a13_v2;
extern double a11_v1b, a11_v2b, a12_v1b, a12_v2b, a13_v1b, a13_v2b;
extern const double a11_gain, a11_a1,  a11_a2, a11_b0, a11_b1, a11_b2; 
extern const double a12_gain, a12_a1, a12_a2, a12_b0, a12_b1, a12_b2; 
extern const double a13_gain, a13_a1,  a13_a2, a13_b0, a13_b1, a13_b2;

void readEMG();
#endif
