/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.
 
    Edited by Nils Heine

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
AutoVocalCtrlAudioProcessorEditor::AutoVocalCtrlAudioProcessorEditor (AutoVocalCtrlAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (400, 400);
    
    // Initialization of UI Sliders
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
    gainControlSlider.setEnabled(false);
    
    v2bDiffSlider.setRange(-10.0, 10.0, 0.1f);
    v2bDiffSlider.setSliderStyle(Slider::LinearBarVertical);
    v2bDiffSlider.setValue(0.0);
    v2bDiffSlider.addListener(this);
    v2bDiffSlider.setEnabled(false);
    
    scInputGSlider.setRange(-60.0, 0.0, 0.1f);
    scInputGSlider.setSliderStyle(Slider::LinearBarVertical);
    scInputGSlider.setValue(-60.0);
    scInputGSlider.addListener(this);
    scInputGSlider.setEnabled(false);
    
    inputSlider.setRange(-60.0, 0.0, 0.1f);
    inputSlider.setSliderStyle(Slider::LinearBarVertical);
    inputSlider.setValue(-60.0);
    inputSlider.addListener(this);
    inputSlider.setEnabled(false);
    
    scGainSlider.setRange(processor.scGainUI->range.start, processor.scGainUI->range.end, 0.01f);
    scGainSlider.setSliderStyle(Slider::LinearBarVertical);
    scGainSlider.setValue(processor.scGainUI->get());
    scGainSlider.addListener(this);
    
    outputSlider.setRange(-60.0, 1.0, 0.1f);
    outputSlider.setSliderStyle(Slider::LinearBarVertical);
    outputSlider.setValue(-60.0);
    outputSlider.addListener(this);
    outputSlider.setEnabled(false);
    
    outputGainSlider.setRange(processor.oGain->range.start, processor.oGain->range.end, 0.1f);
    outputGainSlider.setSliderStyle(Slider::LinearBarVertical);
    outputGainSlider.setValue(processor.oGain->get());
    outputGainSlider.addListener(this);
    
    // Initialization of UI Labels
    detectLabel.setColour(juce::Label::backgroundColourId, juce::Colour(0,0,0));
    detectLabel.setJustificationType(Justification::centred);
    
    scDetectLabel.setColour(juce::Label::backgroundColourId, juce::Colour(0,0,0));
    scDetectLabel.setJustificationType(Justification::centred);
    
    // Initialization of UI Buttons
    readButton.addListener(this);
    readButton.setToggleState(processor.read->get(), dontSendNotification);
    
    detectButton.addListener(this);
    detectButton.setToggleState(processor.detect->get(), dontSendNotification);
    
    scDetectButton.addListener(this);
    scDetectButton.setToggleState(processor.scDetect->get(), dontSendNotification);
    
    scButton.addListener(this);
    scButton.setToggleState(processor.sc->get(), dontSendNotification);
    
    // Add the initialized components to the plug-in UI
    addAndMakeVisible(readButton);
    addAndMakeVisible(detectButton);
    addAndMakeVisible(detectLabel);
    addAndMakeVisible(scDetectButton);
    addAndMakeVisible(scDetectLabel);
    addAndMakeVisible(scButton);
    addAndMakeVisible(loudnessGoalSlider);
    addAndMakeVisible(gainRangeSlider);
    addAndMakeVisible(gainRangeSlider2);
    addAndMakeVisible(gainControlSlider);
    addAndMakeVisible(inputSlider);
    addAndMakeVisible(outputSlider);
    addAndMakeVisible(outputGainSlider);
    addAndMakeVisible(scInputGSlider);
    addAndMakeVisible(scGainSlider);
    addAndMakeVisible(v2bDiffSlider);
    
    // Set new LookAndFeel and declare exceptions
    setLookAndFeel(&newLookAndFeel);
    scInputGSlider.setColour(Slider::textBoxOutlineColourId, Colours::black);
    inputSlider.setColour(Slider::textBoxOutlineColourId, Colours::black);
    outputSlider.setColour(Slider::textBoxOutlineColourId, Colours::black);
    v2bDiffSlider.setColour(Slider::textBoxOutlineColourId, Colours::black);
    gainControlSlider.setColour(Slider::textBoxOutlineColourId, Colours::black);
    
    oldGainRange = 6.0;
    
    // Start timer and set Callback to 40ms
    startTimer(40);
}

AutoVocalCtrlAudioProcessorEditor::~AutoVocalCtrlAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void AutoVocalCtrlAudioProcessorEditor::paint (Graphics& g)
{
    // Paint Background
    Image background = ImageCache::getFromMemory(BinaryData::BAPBG09_png, BinaryData::BAPBG09_pngSize);
    g.drawImageAt(background, 0, 0);
}

void AutoVocalCtrlAudioProcessorEditor::resized()
{
    //Positioning of subcomponents
    scButton.setBounds(40, 260, 120, 40);
    readButton.setBounds(40, 320, 120, 40);
    scDetectButton.setBounds(180, 260, 90, 40);
    scDetectLabel.setBounds(270, 260, 50, 40);
    detectButton.setBounds(180, 320, 90, 40);
    detectLabel.setBounds(270, 320, 50, 40);
    
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

// Called by Button Listener
void AutoVocalCtrlAudioProcessorEditor::buttonClicked(Button* button)
{
    if (button == &readButton) { // Changes between Read and Write Mode of Plug-In
        // Change Parameter
        const bool value = processor.read->get();
        processor.read->operator=(!value);
        readButton.setToggleState(!value, dontSendNotification);
        
        buttonLabel = !value ? "Read":"Write";
        readButton.setButtonText(buttonLabel);
        
        refreshSliderValues();
        scInputGSlider.setValue(-60.0);
        v2bDiffSlider.setValue(0.0);
        
    } else if (button == &detectButton) { // Starts/Ends detection of best suiting loudness goal
        const bool value = processor.detect->get();
        if (value) {
            // Process detection results
            processor.updateLoudnessGoal();
            
            // Restore previous gain range
            processor.gainRange->operator=(oldGainRange);
            processor.updateClipRange();
            
            detectButton.setButtonText("Detect LDNS Goal");
            detectLabel.setText("", dontSendNotification);
        } else {
            // Save current gain range
            oldGainRange = processor.gainRange->get();
            
            // Set gain range to maximum
            processor.gainRange->operator=(processor.gainRange->range.end);
            processor.updateClipRange();
            
            detectButton.setButtonText("Detecting...");
        }
        // Change parameter
        processor.detect->operator=(!value);
        detectButton.setToggleState(!value, dontSendNotification);
        
    } else if (button == &scDetectButton) { // Starts/Ends detection of best suiting side chain input-gain
        const bool value = processor.scDetect->get();
        if (value) {
            // Process detection results
            processor.updateSCGain();
            
            scDetectButton.setButtonText("Detect SC Gain");
            scDetectLabel.setText("", dontSendNotification);
        } else {
            scDetectButton.setButtonText("Detecting...");
        }
        // Change parameter
        processor.scDetect->operator=(!value);
        scDetectButton.setToggleState(!value, dontSendNotification);
        
    } else if (button == &scButton) { // Toggles Side Chain Feature On and Off
        // Change parameter
        const bool value = processor.sc->get();
        processor.sc->operator=(!value);
        scButton.setToggleState(!value, dontSendNotification);
        
        buttonLabel = !value ? "SC ACTV":"SC";
        scButton.setButtonText(buttonLabel);
        
        if (value) {
            // Reset sliders
            scInputGSlider.setValue(-60.0);
            v2bDiffSlider.setValue(0.0);
        }
        // Enable/Disable side chain associated Buttons/Sliders
        scDetectButton.setToggleState(false, sendNotification);
        processor.clearScDetect();
        scDetectButton.setEnabled(!value);
        scGainSlider.setEnabled(!value);
        Colour cGS = !value ? Colour(109, 151, 110):Colours::black;
        Colour cGSB = !value ? Colour(106, 15, 15):Colour(73, 43, 32);
        scGainSlider.setColour(Slider::textBoxOutlineColourId, cGS);
        scGainSlider.setColour(Slider::thumbColourId, cGSB);
    }
}

// Called by Slider Listener
void AutoVocalCtrlAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if (slider == &loudnessGoalSlider) {
        double loudness = loudnessGoalSlider.getValue();
        processor.setLoudnessGoal(loudness);
        
    } else if (slider == &gainRangeSlider) { // Acts as reflection of gainRangeSlider2
        const double newValue = gainRangeSlider.getValue();
        gainRangeSlider2.setValue(-newValue); // Sets value of gainRangeSlider2
        processor.gainRange->operator=(newValue);
        processor.updateClipRange();
    } else if (slider == &gainRangeSlider2) { // Acts as reflection of gainRangeSlider
        const double newValue = gainRangeSlider2.getValue();
        gainRangeSlider.setValue(-newValue); // Sets value of gainRangeSlider
        processor.gainRange->operator=(-newValue);
        processor.updateClipRange();
        
    } else if (slider == &scGainSlider) {
        processor.scGainUI->operator=(scGainSlider.getValue());
        
    } else if (slider == &outputGainSlider) {
        processor.oGain->operator=(outputGainSlider.getValue());
    }
}

// Refreshes the UI subcomponents (for processor internal changes)
void AutoVocalCtrlAudioProcessorEditor::refreshSliderValues()
{
    // Refresh Sliders
    gainRangeSlider.setValue(processor.gainRange->get());
    gainRangeSlider2.setValue(-processor.gainRange->get());
    outputGainSlider.setValue(processor.oGain->get());
    loudnessGoalSlider.setValue(processor.loudnessGoal->get());
    scGainSlider.setValue(processor.scGainUI->get());
    
    // Refresh Buttons
    a = processor.scDetect->get();
    buttonLabel = a ? "Detecting...":"Detect SC Gain";
    scDetectButton.setButtonText(buttonLabel);
    a = processor.detect->get();
    buttonLabel = a ? "Detecting...":"Detect LDNS Goal";
    detectButton.setButtonText(buttonLabel);
    a = processor.sc->get();
    buttonLabel = a ? "SC Active":"SC";
    scButton.setButtonText(buttonLabel);
    scDetectButton.setEnabled(a);
    scGainSlider.setEnabled(a);
    Colour cGS = a ? Colour(109, 151, 110):Colours::black;
    scGainSlider.setColour(Slider::textBoxOutlineColourId, cGS);
    a = processor.read->get();
    buttonLabel = a ? "Read":"Write";
    readButton.setButtonText(buttonLabel);
}

// Refreshes permanent changing subcomponents 25 times per second
void AutoVocalCtrlAudioProcessorEditor::timerCallback()
{
    if (processor.detect->get()) {
        // Displays the current loudness goal detection results (dB difference to previous loudness goal)
        stream.str(std::string());
        const double d = round(processor.getAlphaGain() * 100) / 100;
        stream << std::setprecision(2) << d;
        detectLabel.setText(stream.str(), dontSendNotification);
    }
    if (processor.scDetect->get()) {
        // Displays the current side chain input-gain detection results (dB difference to previous input gain)
        stream.str(std::string());
        const double d = round(processor.getBetaGain() * 100) / 100;
        stream << std::setprecision(2) << d;
        scDetectLabel.setText(stream.str(), dontSendNotification);
    }
    if (processor.sc->get() && !processor.read->get()) {
        // Refreshes side chain associated sliders when this feature is enabled
        scInputGSlider.setValue(processor.getScInputRMSdB(0) + processor.scGainUI->get());
        v2bDiffSlider.setValue(processor.v2bDiff[0]);
    }
    inputSlider.setValue(processor.getInputRMSdB());
    outputSlider.setValue(processor.getOutputdB());
    gainControlSlider.setValue(processor.getCurrentGainControl());
    
    if (processor.refresh) {
        refreshSliderValues();
        processor.refresh = false;
    }
}
