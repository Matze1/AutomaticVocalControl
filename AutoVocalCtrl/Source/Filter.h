/*
  ==============================================================================

    Filter.h
    Created: 10 Feb 2018 4:08:37pm
    Author:  Nils Heine

  ==============================================================================
*/

#pragma once

class AutoVocalCtrlFilter
{
    public:
    //==============================================================================
    AutoVocalCtrlFilter();
    ~AutoVocalCtrlFilter();
    
    //==============================================================================
    void printCoefficients();
    void resetCoefficients();
    void resetTimeBuffer();
    
    //==============================================================================
    void setCoefficientsDirect(double b_0, double b_1, double b_2, double a_1, double a_2);
    
    // Type:
    // 0 = highpass / lowcut
    // 1 = highcut / lowpass
    // 2 = allpass
    // 3 = bandpass
    // 4 = bandreject
    // 5 = lowshelf
    // 6 = highshelf
    
    void setCoefficients(double cutOff, double sampleRate, int type = 0);
    // Type 0,1,2
    
    void setCoefficients(double cutOff, double sampleRate, double q, int type = 0);
    // Type 0,1,2,3,4
    
    void setCoefficientsShelf(double cutOff, double sampleRate, double gain, int type = 6);
    // Type 5 or 6
    
    //==============================================================================
    double process(double sample);
    
    private:
    double b0;
    double b1;
    double b2;
    double a1;
    double a2;
    double z_1;
    double z_2;
};
