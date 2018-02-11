/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "Filter.h"


//==============================================================================
/**
*/
class AutoVocalCtrlAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    AutoVocalCtrlAudioProcessor();
    ~AutoVocalCtrlAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;
    
    //==============================================================================
    float getTimeConstant(float ms);
    int msToSamples(float ms);
    void updateRmsCo();
    void updateExpandTCo();
    void updateCompressTCo();
    void updateTimeConstants();
    void updatePrivateParameter();
    void updateMaxIdleSamples();
    void updateVectors();
    void updateClipRange();
    void updateDelay();

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    double getCurrentGainControl();
    
    //==============================================================================
    void updateFilterSample(double sample, int channel);
    void updateRMS(int channel);
    void updateMLS(int channel);
    void updateGain(int channel);
    
    //==============================================================================
    void numChannelsChanged() override;
    
    //==============================================================================
    AudioParameterFloat* rmsWindow;
    AudioParameterFloat* expandTime;
    AudioParameterFloat* compressTime;
    AudioParameterFloat* loudnessGoal;
    AudioParameterFloat* gainRange;
    AudioParameterFloat* maxIdleTime;
    AudioParameterFloat* gate1;
    AudioParameterFloat* delayLength;

private:
    //==============================================================================
    AudioSampleBuffer delayBuffer;
    int delayBufferLength;
    int delayReadPos, delayWritePos;
    double maxDelayInSec = 1.0; // kann dann später auch weg ;-) wenn nicht mehr änderbar
    
    AutoVocalCtrlFilter lowcut;
    AutoVocalCtrlFilter highshelf;
    Range<double> clipRange;
    double currentSampleRate;
    double rmsCo;
    double expandTCo;
    double compressTCo;
    int lastNumInputChannels = 0;
    int maxIdleSamples;
    int idleCount = 0;
    std::vector<double> filterSample;
    std::vector<double> rms;
    std::vector<double> mls;
    std::vector<double> gain;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoVocalCtrlAudioProcessor)
};
