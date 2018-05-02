/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

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
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AutoVocalCtrlAudioProcessor& processor;
    
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
    
    TextButton readButton; //verschiedene loudness goal modi in dropdown menü? detect (-> extra button kommt in UI) / SC / selbst einstellen
    TextButton scButton;
    TextButton detectButton;
    TextButton scDetectButton;
    
    
    Label detectLabel;
    Label scDetectLabel;
    
    Label loudnessGoalLabel;
    Label gainControlLabel;
    Label v2bDiffLabel;
    Label scInputGLabel;
    Label inputLabel;
    Label scGainLabel;
    Label outputLabel;
    Label outputGainLabel;
    
    float oldGainRange = 6.0;
    std::stringstream stream;
    
    newLookAndFeel newLookAndFeel;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoVocalCtrlAudioProcessorEditor)
};
