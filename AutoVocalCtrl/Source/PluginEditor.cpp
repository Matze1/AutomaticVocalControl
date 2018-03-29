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
    setSize (1100, 400);
    
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
    
//    maxIdleTimeSlider.setRange(processor.maxIdleTime->range.start, 0.0f, 1.0f); // zurzeit aus
//    maxIdleTimeSlider.setSliderStyle(Slider::LinearBarVertical);
//    maxIdleTimeSlider.setTextValueSuffix(" ms");
//    maxIdleTimeSlider.setValue(processor.maxIdleTime->get());
//    maxIdleTimeSlider.addListener(this);
    
    delaySlider.setRange(processor.delayLength->range.start, processor.delayLength->range.end, 1.0f);
    delaySlider.setSliderStyle(Slider::LinearBarVertical);
    delaySlider.setTextValueSuffix(" ms");
    delaySlider.setValue(processor.delayLength->get());
    delaySlider.addListener(this);
    
    loudnessGoalSlider.setRange(processor.loudnessGoal->range.start, processor.loudnessGoal->range.end, 1.0f);
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
    
    alphaSlider.setRange(processor.alpha->range.start, processor.alpha->range.end, 1.0f);
    alphaSlider.setSliderStyle(Slider::LinearBarVertical);
    alphaSlider.setTextValueSuffix(CharPointer_UTF8(" \u03B1"));
    alphaSlider.setValue(processor.alpha->get());
    alphaSlider.addListener(this);
    
    v2bDiffSlider.setRange(processor.v2bDiff->range.start, processor.v2bDiff->range.end, 0.1f);
    v2bDiffSlider.setSliderStyle(Slider::LinearBarVertical);
    v2bDiffSlider.setValue(processor.v2bDiff->get());
    v2bDiffSlider.addListener(this);
    

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
    
    v2bDiffLabel.setText ("V2B Diff", dontSendNotification);
    v2bDiffLabel.attachToComponent(&v2bDiffSlider, false);
    
    
    readButton.addListener(this);
    readButton.setButtonText("Write");
    readButton.setToggleState(processor.read->get(), dontSendNotification);
    
    detectButton.addListener(this);
    detectButton.setButtonText("Detect Best Goal");
    detectButton.setToggleState(processor.detect->get(), dontSendNotification);
    
    scButton.addListener(this);
    scButton.setButtonText("SC MUTE");
    scButton.setToggleState(processor.sc->get(), dontSendNotification);
    
    addAndMakeVisible(readButton);
    addAndMakeVisible(detectButton);
    addAndMakeVisible(scButton);
    
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
    addAndMakeVisible(v2bDiffSlider);
    addAndMakeVisible(v2bDiffLabel);
    
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
    
    readButton.setBounds(40, 290, 180, 40);
    detectButton.setBounds(260, 290, 180, 40);
    scButton.setBounds(480, 290, 180, 40);
    
    rmsSlider.setBounds(40, 50, 80, 200);
    expandTimeSlider.setBounds(140, 50, 80, 200);
    compressTimeSlider.setBounds(240, 50, 80, 200);
    //maxIdleTimeSlider.setBounds(340, 50, 80, 200);
    delaySlider.setBounds(340, 50, 80, 200);
    loudnessGoalSlider.setBounds(440, 50, 80, 200);
    gainRangeSlider.setBounds(540, 50, 80, 200);
    gateSlider.setBounds(640, 50, 80, 200);
    alphaSlider.setBounds(740, 50, 80, 200);
    v2bDiffSlider.setBounds(840, 50, 80, 200);
    
    gainControlSlider.setBounds(980, 50, 80, 200);
}

void AutoVocalCtrlAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button == &readButton) {
        const bool value = processor.read->get();
        processor.read->operator=(!value);
        readButton.setToggleState(!value, dontSendNotification);
        String buttonLabel = !value ? "Read":"Write";
        readButton.setButtonText(buttonLabel);
    } else if (button == &detectButton) {
        const bool value = processor.detect->get();
        if (value) {
            processor.updateLoudnessGoal();
            processor.gainRange->operator=(oldGainRange);
            processor.updateClipRange();
            detectButton.setButtonText("Detect Best Goal");
        } else {
            oldGainRange = processor.gainRange->get();
            processor.gainRange->operator=(processor.gainRange->range.end);
            processor.updateClipRange();
            detectButton.setButtonText("Detecting..."); // SOLLTE NOCH ROT WERDEN ODER SOWAS OK
        }
        processor.detect->operator=(!value);
        detectButton.setToggleState(!value, dontSendNotification);
    } else if (button == &scButton) {
        const bool value = processor.sc->get();
        processor.sc->operator=(!value);
        scButton.setToggleState(!value, dontSendNotification);
        String buttonLabel = !value ? "SC ACTV":"SC MUTE";
        scButton.setButtonText(buttonLabel);
    }
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
        double loudness = loudnessGoalSlider.getValue();
        processor.loudnessGoal->operator=(loudness);
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
        processor.updateAlphaCo();
        processor.updateBetaCo();
    }
}

void AutoVocalCtrlAudioProcessorEditor::timerCallback()
{
    gainControlSlider.setValue(processor.getCurrentGainControl());
    loudnessGoalSlider.setValue(processor.loudnessGoal->get());
    v2bDiffSlider.setValue(processor.v2bDiff->get());
}
