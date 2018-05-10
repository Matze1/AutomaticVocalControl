/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.
 
    Edited by Nils Heine

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include <sstream>
#include <iomanip>
#include "newLookAndFeel.h"

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
    // This reference for editor to access the processor object that created it.
    AutoVocalCtrlAudioProcessor& processor;
    
    newLookAndFeel newLookAndFeel;
    
    void sliderValueChanged (Slider* slider) override;
    void buttonClicked(Button* button) override;
    void timerCallback() override;
    void refreshSliderValues();
    
    Slider loudnessGoalSlider;
    Slider gainRangeSlider;
    Slider gainRangeSlider2;
    Slider gainControlSlider;
    Slider v2bDiffSlider;
    Slider scInputGSlider;
    Slider inputSlider;
    Slider scGainSlider;
    Slider outputSlider;
    Slider outputGainSlider;
    
    TextButton readButton;
    TextButton scButton;
    TextButton detectButton;
    TextButton scDetectButton;
    
    String buttonLabel;
    
    Label detectLabel;
    Label scDetectLabel;
    
    float oldGainRange;
    
    bool a;
    
    std::stringstream stream;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoVocalCtrlAudioProcessorEditor)
};
