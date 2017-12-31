/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (390, 300);
    
    thresholdSlider.setSliderStyle (Slider::LinearBarVertical);
    thresholdSlider.setRange(-60.0, 0.0, 0.1);
    thresholdSlider.setTextValueSuffix (" dB");
    thresholdSlider.setValue(-20, sendNotification);
    
    gainSlider.setSliderStyle (Slider::LinearBarVertical);
    gainSlider.setRange(-20, 20, 0.01);
    gainSlider.setTextValueSuffix (" dB");
    gainSlider.setValue(0);
    
    maxGainSlider.setSliderStyle (Slider::LinearBarVertical);
    maxGainSlider.setRange(0.0, 20.0, 0.1);
    maxGainSlider.setTextValueSuffix (" dB");
    maxGainSlider.setValue(3.5);
    
    averageSlider.setSliderStyle (Slider::LinearBarVertical);
    averageSlider.setRange(10, 100, 0.1);
    averageSlider.setTextValueSuffix (" ms");
    averageSlider.setValue(80, sendNotification);
    
    influenceSlider.setSliderStyle (Slider::LinearBarVertical);
    influenceSlider.setRange(0, 100, 0.1);
    influenceSlider.setTextValueSuffix (" %");
    influenceSlider.setValue(0, sendNotification);
    
    addAndMakeVisible(thresholdSlider);
    addAndMakeVisible(gainSlider);
    addAndMakeVisible(averageSlider);
    addAndMakeVisible(influenceSlider);
    addAndMakeVisible(maxGainSlider);
    
    thresholdSlider.addListener (this);
    averageSlider.addListener(this);
    influenceSlider.addListener(this);
    maxGainSlider.addListener(this);
    
    addAndMakeVisible (thresholdLabel);
    thresholdLabel.setText ("Threshold", dontSendNotification);
    thresholdLabel.attachToComponent (&thresholdSlider, false);
    
    addAndMakeVisible (maxGainLabel);
    maxGainLabel.setText ("Max Gain Adaptation", dontSendNotification);
    maxGainLabel.attachToComponent (&maxGainSlider, false);
    
    addAndMakeVisible (averageLabel);
    averageLabel.setText ("RMS AV", dontSendNotification);
    averageLabel.attachToComponent (&averageSlider, false);
    
    addAndMakeVisible (influenceLabel);
    influenceLabel.setText ("Influence", dontSendNotification);
    influenceLabel.attachToComponent (&influenceSlider, false);
    
    startTimer(40);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
}

void NewProjectAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    thresholdSlider.setBounds (20, 20, 50, getHeight() - 40);
    averageSlider.setBounds (90, 20, 50, getHeight() - 40);
    influenceSlider.setBounds (160, 20, 50, getHeight() - 40);
    maxGainSlider.setBounds (230, 20, 50, getHeight() - 40);
    gainSlider.setBounds (300, 20, 50, getHeight() - 40);
}

void NewProjectAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if (slider == &thresholdSlider)
        processor.thresholdDB = thresholdSlider.getValue();
    else if (slider == &averageSlider)
    {
        processor.av = averageSlider.getValue();
        processor.updateAverageConstant();
    }
    else if (slider == &influenceSlider)
        processor.influence = influenceSlider.getValue() / 100;
    else if (slider == &maxGainSlider)
        processor.maxgain = maxGainSlider.getValue();
}

void NewProjectAudioProcessorEditor::timerCallback()
{
    double gain = processor.gain[0];
    gainSlider.setValue(20 * log10(gain));
}

