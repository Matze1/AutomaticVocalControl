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
    setSize (400, 400);
    
    loudnessGoalSlider.setRange(processor.loudnessGoal->range.start, processor.loudnessGoal->range.end, 1.0f);
    loudnessGoalSlider.setSliderStyle(Slider::LinearVertical);
    loudnessGoalSlider.setTextValueSuffix(" dB");
    loudnessGoalSlider.setValue(processor.loudnessGoal->get());
    loudnessGoalSlider.addListener(this);
    
    gainRangeSlider.setRange(processor.gainRange->range.start, processor.gainRange->range.end, 0.1f);
    gainRangeSlider.setSliderStyle(Slider::LinearVertical);
    gainRangeSlider.setValue(processor.gainRange->get());
    gainRangeSlider.addListener(this);
    gainRangeSlider.setLookAndFeel(&newLookAndFeel);
    
    gainRangeSlider2.setRange(-processor.gainRange->range.end, processor.gainRange->range.start, 0.1f);
    gainRangeSlider2.setSliderStyle(Slider::LinearVertical);
    gainRangeSlider2.setValue(-processor.gainRange->get());
    gainRangeSlider2.addListener(this);
    
    gainControlSlider.setRange(-10.0, 10.0, 0.1f);
    gainControlSlider.setSliderStyle(Slider::LinearBarVertical);
    gainControlSlider.setTextValueSuffix(" dB");
    gainControlSlider.setValue(processor.getCurrentGainControl());
    gainControlSlider.addListener(this);
    
    v2bDiffSlider.setRange(-10.0, 10.0, 0.1f);
    v2bDiffSlider.setSliderStyle(Slider::LinearBarVertical);
    v2bDiffSlider.setValue(0.0);
    v2bDiffSlider.addListener(this);
    
    scInputGSlider.setRange(-60.0, 0.0, 0.1f);
    scInputGSlider.setSliderStyle(Slider::LinearBarVertical);
    scInputGSlider.setValue(-60.0);
    scInputGSlider.addListener(this);
    
    inputSlider.setRange(-60.0, 0.0, 0.1f);
    inputSlider.setSliderStyle(Slider::LinearBarVertical);
    inputSlider.setValue(-60.0);
    inputSlider.addListener(this);
    
    scGainSlider.setRange(processor.scGainUI->range.start, processor.scGainUI->range.end, 0.01f);
    scGainSlider.setSliderStyle(Slider::LinearBarVertical);
    scGainSlider.setValue(processor.scGainUI->get());
    scGainSlider.addListener(this);
    
    outputSlider.setRange(-60.0, 1.0, 0.1f);
    outputSlider.setSliderStyle(Slider::LinearBarVertical);
    outputSlider.setValue(-60.0);
    outputSlider.addListener(this);
    
    outputGainSlider.setRange(processor.oGain->range.start, processor.oGain->range.end, 0.1f);
    outputGainSlider.setSliderStyle(Slider::LinearBarVertical);
    outputGainSlider.setValue(processor.oGain->get());
    outputGainSlider.addListener(this);
    
    loudnessGoalLabel.setText ("Goal", dontSendNotification);
    loudnessGoalLabel.attachToComponent(&loudnessGoalSlider, false);
    
    gainControlLabel.setText ("ADPT", dontSendNotification);
    gainControlLabel.attachToComponent(&gainControlSlider, false);
    
    v2bDiffLabel.setText ("+", dontSendNotification);
    v2bDiffLabel.attachToComponent(&v2bDiffSlider, false);
    
    scInputGLabel.setText ("SC", dontSendNotification);
    scInputGLabel.attachToComponent(&scInputGSlider, false);
    
    inputLabel.setText ("I", dontSendNotification);
    inputLabel.attachToComponent(&inputSlider, false);
    
    scGainLabel.setText ("G", dontSendNotification);
    scGainLabel.attachToComponent(&scGainSlider, false);
    
    outputLabel.setText ("O", dontSendNotification);
    outputLabel.attachToComponent(&outputSlider, false);
    
    outputGainLabel.setText ("G", dontSendNotification);
    outputGainLabel.attachToComponent(&outputGainSlider, false);
    
    detectLabel.setColour(juce::Label::backgroundColourId, juce::Colour(0,0,0));
    detectLabel.setJustificationType(Justification::centred);
    
    scDetectLabel.setColour(juce::Label::backgroundColourId, juce::Colour(0,0,0));
    scDetectLabel.setJustificationType(Justification::centred);
    
    readButton.addListener(this);
    readButton.setButtonText("Write");
    readButton.setToggleState(processor.read->get(), dontSendNotification);
    
    detectButton.addListener(this);
    detectButton.setButtonText("Detect LDNS Goal");
    detectButton.setToggleState(processor.detect->get(), dontSendNotification);
    
    scDetectButton.addListener(this);
    scDetectButton.setButtonText("Detect SC Gain");
    scDetectButton.setToggleState(processor.scDetect->get(), dontSendNotification);
    
    scButton.addListener(this);
    scButton.setButtonText("SC MUTE");
    scButton.setToggleState(processor.sc->get(), dontSendNotification);
    
    addAndMakeVisible(readButton);
    addAndMakeVisible(detectButton);
    addAndMakeVisible(detectLabel);
    addAndMakeVisible(scDetectButton);
    addAndMakeVisible(scDetectLabel);
    addAndMakeVisible(scButton);
    
    addAndMakeVisible(loudnessGoalSlider);
    addAndMakeVisible(loudnessGoalLabel);
    addAndMakeVisible(gainRangeSlider);
    addAndMakeVisible(gainRangeSlider2);
    addAndMakeVisible(gainControlSlider);
    addAndMakeVisible(gainControlLabel);
    addAndMakeVisible(inputSlider);
    addAndMakeVisible(inputLabel);
    addAndMakeVisible(outputSlider);
    addAndMakeVisible(outputLabel);
    addAndMakeVisible(outputGainSlider);
    addAndMakeVisible(outputGainLabel);
    addAndMakeVisible(scInputGLabel);
    addAndMakeVisible(scInputGSlider);
    addAndMakeVisible(scGainLabel);
    addAndMakeVisible(scGainSlider);
    addAndMakeVisible(v2bDiffSlider);
    addAndMakeVisible(v2bDiffLabel);
    
    startTimer(40);
    
    setLookAndFeel(&newLookAndFeel);
}

AutoVocalCtrlAudioProcessorEditor::~AutoVocalCtrlAudioProcessorEditor()
{
}

//==============================================================================
void AutoVocalCtrlAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
//    Image background = ImageCache::getFromMemory(BinaryData::test_png, BinaryData::test_pngSize);
//    g.drawImageAt(background, 0, 0);

    //g.setColour (Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), Justification::centred, 1);
}

void AutoVocalCtrlAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    scButton.setBounds(40, 260, 120, 40);
    readButton.setBounds(40, 320, 120, 40);
    scDetectButton.setBounds(180, 260, 90, 40);
    scDetectLabel.setBounds(280, 260, 40, 40);
    detectButton.setBounds(180, 320, 90, 40);
    detectLabel.setBounds(280, 320, 40, 40);
    
    scGainSlider.setBounds(40, 40, 20, 200);
    scInputGSlider.setBounds(60, 40, 30, 200);
    
    loudnessGoalSlider.setBounds(116, 33, 14, 214);
    inputSlider.setBounds(130, 40, 30, 200);
    
    outputSlider.setBounds(180, 40, 30, 200);
    
    outputGainSlider.setBounds(210, 40, 20, 200);
    
    gainRangeSlider.setBounds(256, 33, 14, 114);
    gainRangeSlider2.setBounds(256, 133, 14, 114);
    gainControlSlider.setBounds(270, 40, 30, 200);
    v2bDiffSlider.setBounds(300, 40, 20, 200);
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
            detectButton.setButtonText("Detect LDNS Goal");
            detectLabel.setText("", dontSendNotification);
        } else {
            oldGainRange = processor.gainRange->get();
            processor.gainRange->operator=(processor.gainRange->range.end);
            processor.updateClipRange();
            detectButton.setButtonText("Detecting..."); // SOLLTE NOCH ROT WERDEN ODER SOWAS OK
        }
        processor.detect->operator=(!value);
        detectButton.setToggleState(!value, dontSendNotification);
    } else if (button == &scDetectButton) {
        const bool value = processor.scDetect->get();
        if (value) {
            processor.updateSCGain();
            scDetectButton.setButtonText("Detect SC Gain");
            scDetectLabel.setText("", dontSendNotification);
        } else {
            scDetectButton.setButtonText("Detecting..."); // SOLLTE NOCH ROT WERDEN ODER SOWAS OK
        }
        processor.scDetect->operator=(!value);
        scDetectButton.setToggleState(!value, dontSendNotification);
    } else if (button == &scButton) {
        const bool value = processor.sc->get();
        processor.sc->operator=(!value);
        scButton.setToggleState(!value, dontSendNotification);
        String buttonLabel = !value ? "SC ACTV":"SC MUTE";
        scButton.setButtonText(buttonLabel);
        if (value) {
            scInputGSlider.setValue(-60.0);
            v2bDiffSlider.setValue(0.0);
        }
    }
}

void AutoVocalCtrlAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if (slider == &loudnessGoalSlider) {
        double loudness = loudnessGoalSlider.getValue();
        processor.setLoudnessGoal(loudness);
    } else if (slider == &gainRangeSlider) {
        const double newValue = gainRangeSlider.getValue();
        gainRangeSlider2.setValue(-newValue);
        processor.gainRange->operator=(newValue);
        processor.updateClipRange();
    } else if (slider == &gainRangeSlider2) {
        const double newValue = gainRangeSlider2.getValue();
        gainRangeSlider.setValue(-newValue);
        processor.gainRange->operator=(-newValue);
        processor.updateClipRange();
    } else if (slider == &scGainSlider)
        processor.scGainUI->operator=(scGainSlider.getValue());
    else if (slider == &outputGainSlider)
        processor.oGain->operator=(outputGainSlider.getValue());
}

void AutoVocalCtrlAudioProcessorEditor::refreshSliderValues()
{
    gainRangeSlider.setValue(processor.gainRange->get());
    gainRangeSlider2.setValue(-processor.gainRange->get());
    outputGainSlider.setValue(processor.oGain->get());
}

void AutoVocalCtrlAudioProcessorEditor::timerCallback()
{
    if (processor.detect->get()) {
        stream.str("");
        const double d = round(processor.getAlphaGain() * 100) / 100;
        stream << std::setprecision(2) << d;
        detectLabel.setText(stream.str(), dontSendNotification);
    }
    if (processor.scDetect->get()) {
        stream.str("");
        const double d = round(processor.getBetaGain() * 100) / 100;
        stream << std::setprecision(2) << d;
        scDetectLabel.setText(stream.str(), dontSendNotification);
    }
    inputSlider.setValue(processor.getInputRMSdB());
    outputSlider.setValue(processor.getOutputdB());
    if (processor.sc->get()) {
        scInputGSlider.setValue(processor.getScInputRMSdB(0) + processor.scGainUI->get());
        v2bDiffSlider.setValue(processor.v2bDiff[0]);
    }
    inputSlider.setValue(processor.getInputRMSdB());
    gainControlSlider.setValue(processor.getCurrentGainControl());
    loudnessGoalSlider.setValue(processor.loudnessGoal->get());
    scGainSlider.setValue(processor.scGainUI->get());
    if (processor.refresh) {
        refreshSliderValues();
        processor.refresh = false;
    }
}
