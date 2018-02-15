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
class AutoVocalCtrlAudioProcessorEditor  : public AudioProcessorEditor, public Slider::Listener, private Timer, public Button::Listener
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
    void buttonClicked(Button* button) override;
    void timerCallback() override;
    
    Slider rmsSlider;
    Slider expandTimeSlider;
    Slider compressTimeSlider;
    Slider loudnessGoalSlider;
    Slider gainRangeSlider;
    Slider maxIdleTimeSlider;
    Slider gainControlSlider;
    Slider gateSlider;
    Slider delaySlider;
    Slider alphaSlider;
    
    TextButton readButton;
    
    Label rmsLabel;
    Label expandTimeLabel;
    Label compressTimeLabel;
    Label loudnessGoalLabel;
    Label gainRangeLabel;
    Label maxIdleTimeLabel;
    Label gainControlLabel;
    Label gateLabel;
    Label delayLabel;
    Label alphaLabel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoVocalCtrlAudioProcessorEditor)
};
