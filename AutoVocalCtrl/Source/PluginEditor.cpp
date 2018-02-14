/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AutoVocalCtrlAudioProcessorEditor::AutoVocalCtrlAudioProcessorEditor (AutoVocalCtrlAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1000, 300);
    
    rmsSlider.setRange(processor.rmsWindow->range.start, processor.rmsWindow->range.end, 1.0f);
    rmsSlider.setSliderStyle(Slider::LinearBarVertical);
    rmsSlider.setTextValueSuffix(" ms");
    rmsSlider.setValue(processor.rmsWindow->get());
    rmsSlider.addListener(this);
    
    expandTimeSlider.setRange(processor.expandTime->range.start, processor.expandTime->range.end, 1.0f);
    expandTimeSlider.setSliderStyle(Slider::LinearBarVertical);
    expandTimeSlider.setTextValueSuffix(" ms");
    expandTimeSlider.setValue(processor.expandTime->get());
    expandTimeSlider.addListener(this);
    
    compressTimeSlider.setRange(processor.compressTime->range.start, processor.compressTime->range.end, 1.0f);
    compressTimeSlider.setSliderStyle(Slider::LinearBarVertical);
    compressTimeSlider.setTextValueSuffix(" ms");
    compressTimeSlider.setValue(processor.compressTime->get());
    compressTimeSlider.addListener(this);
    
    maxIdleTimeSlider.setRange(processor.maxIdleTime->range.start, 0.0f, 1.0f); // zurzeit aus
    maxIdleTimeSlider.setSliderStyle(Slider::LinearBarVertical);
    maxIdleTimeSlider.setTextValueSuffix(" ms");
    maxIdleTimeSlider.setValue(processor.maxIdleTime->get());
    maxIdleTimeSlider.addListener(this);
    
    delaySlider.setRange(processor.delayLength->range.start, processor.delayLength->range.end, 1.0f);
    delaySlider.setSliderStyle(Slider::LinearBarVertical);
    delaySlider.setTextValueSuffix(" ms");
    delaySlider.setValue(processor.delayLength->get());
    delaySlider.addListener(this);
    
    loudnessGoalSlider.setRange(processor.loudnessGoal->range.start, processor.loudnessGoal->range.end, 0.1f);
    loudnessGoalSlider.setSliderStyle(Slider::LinearBarVertical);
    loudnessGoalSlider.setTextValueSuffix(" dB");
    loudnessGoalSlider.setValue(processor.loudnessGoal->get());
    loudnessGoalSlider.addListener(this);
    
    gainRangeSlider.setRange(processor.gainRange->range.start, processor.gainRange->range.end, 0.1f);
    gainRangeSlider.setSliderStyle(Slider::LinearBarVertical);
    gainRangeSlider.setTextValueSuffix(" dB");
    gainRangeSlider.setValue(processor.gainRange->get());
    gainRangeSlider.addListener(this);
    
    gainControlSlider.setRange(-10.0, 10.0, 0.1f);
    gainControlSlider.setSliderStyle(Slider::LinearBarVertical);
    gainControlSlider.setTextValueSuffix(" dB");
    gainControlSlider.setValue(processor.getCurrentGainControl());
    gainControlSlider.addListener(this);
    
    gateSlider.setRange(processor.gate1->range.start, processor.gate1->range.end, 0.1f);
    gateSlider.setSliderStyle(Slider::LinearBarVertical);
    gateSlider.setTextValueSuffix(" dB");
    gateSlider.setValue(processor.gate1->get());
    gateSlider.addListener(this);
    
    alphaSlider.setRange(processor.alpha->range.start, processor.alpha->range.end, 0.01f);
    alphaSlider.setSliderStyle(Slider::LinearBarVertical);
    alphaSlider.setTextValueSuffix(CharPointer_UTF8(" \u03B1"));
    alphaSlider.setValue(processor.alpha->get());
    alphaSlider.addListener(this);
    

    rmsLabel.setText ("RMS Window", dontSendNotification);
    rmsLabel.attachToComponent(&rmsSlider, false);
    
    expandTimeLabel.setText ("Expand Time", dontSendNotification);
    expandTimeLabel.attachToComponent(&expandTimeSlider, false);
    
    compressTimeLabel.setText ("Compress Time", dontSendNotification);
    compressTimeLabel.attachToComponent(&compressTimeSlider, false);
    
    maxIdleTimeLabel.setText ("max. Idle Time", dontSendNotification);
    maxIdleTimeLabel.attachToComponent(&maxIdleTimeSlider, false);
    
    delayLabel.setText ("Lookahead", dontSendNotification);
    delayLabel.attachToComponent(&delaySlider, false);
    
    loudnessGoalLabel.setText ("Loudness Goal", dontSendNotification);
    loudnessGoalLabel.attachToComponent(&loudnessGoalSlider, false);
    
    gainRangeLabel.setText ("Gain Range", dontSendNotification);
    gainRangeLabel.attachToComponent(&gainRangeSlider, false);
    
    gainControlLabel.setText ("Current Gain", dontSendNotification);
    gainControlLabel.attachToComponent(&gainControlSlider, false);
    
    gateLabel.setText ("MLS Gate", dontSendNotification);
    gateLabel.attachToComponent(&gateSlider, false);
    
    alphaLabel.setText ("Alpha", dontSendNotification);
    alphaLabel.attachToComponent(&alphaSlider, false);
    
    
    addAndMakeVisible(rmsSlider);
    addAndMakeVisible(rmsLabel);
    addAndMakeVisible(expandTimeSlider);
    addAndMakeVisible(expandTimeLabel);
    addAndMakeVisible(compressTimeSlider);
    addAndMakeVisible(compressTimeLabel);
//    addAndMakeVisible(maxIdleTimeSlider);
//    addAndMakeVisible(maxIdleTimeLabel);
    addAndMakeVisible(delaySlider);
    addAndMakeVisible(delayLabel);
    addAndMakeVisible(loudnessGoalSlider);
    addAndMakeVisible(loudnessGoalLabel);
    addAndMakeVisible(gainRangeSlider);
    addAndMakeVisible(gainRangeLabel);
    addAndMakeVisible(gainControlSlider);
    addAndMakeVisible(gainControlLabel);
    addAndMakeVisible(gateSlider);
    addAndMakeVisible(gateLabel);
    addAndMakeVisible(alphaSlider);
    addAndMakeVisible(alphaLabel);
    
    startTimer(40);
}

AutoVocalCtrlAudioProcessorEditor::~AutoVocalCtrlAudioProcessorEditor()
{
}

//==============================================================================
void AutoVocalCtrlAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    //g.setColour (Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void AutoVocalCtrlAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    rmsSlider.setBounds(40, 50, 80, 200);
    expandTimeSlider.setBounds(140, 50, 80, 200);
    compressTimeSlider.setBounds(240, 50, 80, 200);
    //maxIdleTimeSlider.setBounds(340, 50, 80, 200);
    delaySlider.setBounds(340, 50, 80, 200);
    loudnessGoalSlider.setBounds(440, 50, 80, 200);
    gainRangeSlider.setBounds(540, 50, 80, 200);
    gateSlider.setBounds(640, 50, 80, 200);
    alphaSlider.setBounds(740, 50, 80, 200);
    
    gainControlSlider.setBounds(880, 50, 80, 200);
}

void AutoVocalCtrlAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if (slider == &rmsSlider)
    {
        processor.rmsWindow->operator=(rmsSlider.getValue());
        processor.updateRmsCo();
    }
    else if (slider == &expandTimeSlider)
    {
        processor.expandTime->operator=(expandTimeSlider.getValue());
        processor.updateExpandTCo();
    }
    else if (slider == &compressTimeSlider)
    {
        processor.compressTime->operator=(compressTimeSlider.getValue());
        processor.updateCompressTCo();
    }
    else if (slider == &maxIdleTimeSlider)
    {
        processor.maxIdleTime->operator=(maxIdleTimeSlider.getValue());
        processor.updateMaxIdleSamples();
    }
    else if (slider == &delaySlider)
    {
        processor.delayLength->operator=(delaySlider.getValue());
        processor.updateDelay();
    }
    else if (slider == &loudnessGoalSlider)
    {
        processor.loudnessGoal->operator=(loudnessGoalSlider.getValue());
        
    }
    else if (slider == &gainRangeSlider)
    {
        processor.gainRange->operator=(gainRangeSlider.getValue());
        processor.updateClipRange();
    }
    else if (slider == &gateSlider)
    {
        processor.gate1->operator=(gateSlider.getValue());
    }
    else if (slider == &alphaSlider)
    {
        processor.alpha->operator=(alphaSlider.getValue());
    }
}

void AutoVocalCtrlAudioProcessorEditor::timerCallback()
{
    gainControlSlider.setValue(processor.getCurrentGainControl());
}
