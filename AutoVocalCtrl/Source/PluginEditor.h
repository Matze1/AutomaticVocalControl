/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/
class AutoVocalCtrlAudioProcessorEditor  : public AudioProcessorEditor, public Slider::Listener, private Timer
{
public:
    AutoVocalCtrlAudioProcessorEditor (AutoVocalCtrlAudioProcessor&);
    ~AutoVocalCtrlAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AutoVocalCtrlAudioProcessor& processor;
    
    void sliderValueChanged (Slider* slider) override;
    void timerCallback() override;
    
    Slider rmsSlider;
    Slider expandTimeSlider;
    Slider compressTimeSlider;
    Slider loudnessGoalSlider;
    Slider gainRangeSlider;
    Slider maxIdleTimeSlider;
    Slider gainControlSlider;
    Slider gateSlider;
    
    Label rmsLabel;
    Label expandTimeLabel;
    Label compressTimeLabel;
    Label loudnessGoalLabel;
    Label gainRangeLabel;
    Label maxIdleTimeLabel;
    Label gainControlLabel;
    Label gateLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoVocalCtrlAudioProcessorEditor)
};
