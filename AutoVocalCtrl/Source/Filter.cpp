/*
  ==============================================================================

    Filter.cpp
    Created: 10 Feb 2018 4:08:29pm
    Author:  Nils Heine

  ==============================================================================
*/

#include "Filter.h"
#include <iostream>
#include <math.h>

AutoVocalCtrlFilter::AutoVocalCtrlFilter()
{
    resetCoefficients();
}

AutoVocalCtrlFilter::~AutoVocalCtrlFilter()
{
}

void AutoVocalCtrlFilter::printCoefficients()
{
    std::cout << b0 << " " << b1 << " " << b2 << " " << a1 << " " << a2;
}

void AutoVocalCtrlFilter::resetCoefficients()
{
    b0 = 1.0;
    b1 = 0.0;
    b2 = 0.0;
    a1 = 0.0;
    a2 = 0.0;
    resetTimeBuffer();
}

void AutoVocalCtrlFilter::resetTimeBuffer()
{
    z_1 = 0.0;
    z_2 = 0.0;
}

/*
 Processing a sample through the biquad architecture of the
 current filter.
 
 @param sample : the transferred sample
 @return : the processed sample
*/
double AutoVocalCtrlFilter::process(double sample)
{
    const double mid = sample - a1 * z_1 - a2 * z_2;
    const double out = b0 * mid + b1 * z_1 + b2 * z_2;
    
    z_2 = z_1;
    z_1 = mid;
    
    return out;
}

void AutoVocalCtrlFilter::setCoefficientsDirect(double b_0, double b_1, double b_2, double a_1, double a_2)
{
    b0 = b_0;
    b1 = b_1;
    b2 = b_2;
    a1 = a_1;
    a2 = a_2;
}

/*
 Setting the coefficients for a requested first order filter.
 
 Calculations based on DAFX: Digital Audio Effects edited by Udo Zölzer, John Wiley & Sons Lid., 2011
 
 @param cutOff : the cutoff frequency of the resulting filter
 @param sampleRate : the current samplerate
 @param type : type of the requested filter:
    0 = highpass / lowcut
    1 = highcut / lowpass
    2 = allpass
*/
void AutoVocalCtrlFilter::setCoefficients(double cutOff, double sampleRate, int type)
{
    const double k = tan(M_PI * cutOff / sampleRate);
    if (type == 0) {
        b0 = 1.0 / (k + 1.0);
        b1 = -b0;
    } else if (type == 1) {
        b0 = k / (k + 1.0);
        b1 = b0;
    } else if (type == 2) {
        b0 = (k - 1.0) / (k + 1.0);
        b1 = 1.0;
    } else {
        std::cout << "wrong type. try 0 for highpass, 1 for lowpass or 2 for allpass.";
    }
    a1 = (k - 1.0) / (k + 1.0);
    b2 = 0.0;
    a2 = 0.0;
}

/*
 Setting the coefficients for a requested second order filter.
 
 Calculations based on DAFX: Digital Audio Effects edited by Udo Zölzer, John Wiley & Sons Lid., 2011
 
 @param cutOff : the cutoff frequency of the resulting filter
 @param sampleRate : the current samplerate
 @param q : changes height of resonance / bandwith (depending on type)
 @param type : type of the requested filter:
    0 = highpass / lowcut
    1 = highcut / lowpass
    2 = allpass
    3 = bandpass
    4 = bandreject
*/
void AutoVocalCtrlFilter::setCoefficients(double cutOff, double sampleRate, double q, int type)
{
    const double k = tan(M_PI * cutOff / sampleRate);
    const double kk = k * k;
    const double x = kk * q;
    const double y = k + q;
    const double z = x + y;
    const double zm = x - k + q;
    const double zb = 2.0 * q * (kk - 1.0) / z;
    if (type == 0) {
        b0 = q / z;
        b1 = -2.0 * b0;
        b2 = b0;
    } else if (type == 1) {
        b0 = x / z;
        b1 = 2.0 * b0;
        b2 = b0;
    } else if (type == 3) {
        b0 = k / z;
        b1 = 0.0;
        b2 = -b0;
    } else if (type == 4) {
        b0 = q * (1.0 + kk) / z;
        b1 = zb;
        b2 = b0;
    } else if (type == 2) {
        b0 = zm / z;
        b1 = zb;
        b2 = 1.0;
    } else {
        std::cout << "wrong type. try 0 for highpass, 1 for lowpass or 2 for allpass.";
        std::cout << "try 3 for bandpass or 4 for bandreject.";
    }
    a1 = zb;
    a2 = zm / z;
}

/*
 Setting the coefficients for a requested second order shelving filter.
 
 Calculations based on DAFX: Digital Audio Effects edited by Udo Zölzer, John Wiley & Sons Lid., 2011
 
 @param cutOff : the center frequency of the resulting filter
 @param sampleRate : the current samplerate
 @param gain : the gain boost applied by the filter
 @param type : type of the requested filter:
    5 = lowshelf
    6 = highshelf
*/
void AutoVocalCtrlFilter::setCoefficientsShelf(double cutOff, double sampleRate, double gain, int type)
{
    const double k = tan(M_PI * cutOff / sampleRate);
    const double v0 = pow(10.0,(gain/20.0));
    const double kk = k * k;
    const double w2k = sqrt(2.) * k;
    const double w2v0k = sqrt(2. * v0) * k;
    if (gain == 0.0) {
        resetCoefficients();
    } else if (type == 5) {
        if (gain > 0.0) {
            const double denom = 1.0 +  w2k + kk;
            const double v0kk = v0 * kk;
            b0 = (1.0 + w2v0k + v0kk) / denom;
            b1 = 2.0 * (v0kk - 1.0) / denom;
            b2 = (1.0 - w2v0k + v0kk) / denom;
            a1 = 2.0 * (kk - 1.0) / denom;
            a2 = (1.0 - w2k + kk) / denom;
        } else {
            const double denom = v0 + w2v0k + kk;
            b0 = v0 * (1. + w2k + kk) / denom;
            b1 = 2. * v0 * (kk - 1.) / denom;
            b2 = v0 * (1. - w2k + kk) / denom;
            a1 = 2. * (kk - v0) / denom;
            a2 = (v0 - w2v0k + kk) / denom;
        }
    } else if (type == 6) {
        if (gain > 0.0) {
            const double denom = 1.0 +  w2k + kk;
            b0 = (v0 + w2v0k + kk) / denom;
            b1 = 2.0 * (kk - v0) / denom;
            b2 = (v0 - w2v0k + kk) / denom;
            a1 = 2.0 * (kk - 1.0) / denom;
            a2 = (1.0 - w2k + kk) / denom;
        } else {
            const double denom = 1.0 + w2v0k + v0 * kk;
            b0 = v0 * (1.0 + w2k + kk) / denom;
            b1 = 2.0 * v0 * (kk - 1.0) / denom;
            b2 = v0 * (1.0 - w2k + kk) / denom;
            a1 = 2.0 * (v0 * kk - 1.0) / denom;
            a2 = (1.0 - w2v0k + v0 * kk) / denom;
        }
    } else {
        std::cout << "wrong type. try 5 for lowshelf or 6 for highshelf.";
    }
}
