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
class NewProjectAudioProcessorEditor  : public AudioProcessorEditor, public Slider::Listener, private Timer
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor();
    
    void sliderValueChanged(Slider* slider) override;
    void timerCallback() override;

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    Slider thresholdSlider;
    Label thresholdLabel;
    
    Slider maxGainSlider;
    Label maxGainLabel;
    
    Slider gainSlider;
    
    Slider averageSlider;
    Label averageLabel;
    
    Slider influenceSlider;
    Label influenceLabel;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NewProjectAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
