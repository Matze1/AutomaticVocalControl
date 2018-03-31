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
    void updateAlphaCo();
    void updateBetaCo();
    void updateTimeConstants();
    void updatePrivateParameter();
    void updateMaxIdleSamples();
    void updateVectors();
    void updateClipRange();
    void updateDelay();
    void updateAutomation();
    void updateLoudnessGoal();
    void automateCurrentGain();

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    double getCurrentGainControl();
    double getAlphaGain();
    double getInputRMSdB();
    double getScInputRMSdB();
    double getOutputdB();
    
    //==============================================================================
    double updateFilterSample(double sample, AutoVocalCtrlFilter hs, AutoVocalCtrlFilter lc);
    double updateRMS2(double sample, double last, double co);
    double updateGate(double rms2, double gate);
    double updateGain(double sample, double scSample, double lastGn);
    
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
    AudioParameterFloat* alpha;
    AudioParameterFloat* currentGain;
    AudioParameterFloat* v2bDiff;
    AudioParameterFloat* scGainUI;
    AudioParameterBool* read;
    AudioParameterBool* detect;
    AudioParameterBool* sc;
    

private:
    //==============================================================================
    AudioSampleBuffer delayBuffer;
    int delayBufferLength;
    int delayReadPos, delayWritePos;
    double maxDelayInSec = 1.0; // kann dann später auch weg ;-) wenn nicht mehr änderbar
    
    int count;
    int count2;
    int detCount;
    bool upBefore;
    double lastGain;
    double gainAtPoint;
    AutoVocalCtrlFilter lowcut;
    AutoVocalCtrlFilter highshelf;
    AutoVocalCtrlFilter scLowcut;
    AutoVocalCtrlFilter scHighshelf;
    AutoVocalCtrlFilter oLowcut;
    AutoVocalCtrlFilter oHighshelf;
    Range<double> clipRange;
    double currentSampleRate;
    double rmsCo;
    double scRmsCo;
    double expandTCo;
    double compressTCo;
    double alphaCo;
    double betaCo;
    double scGain;
    int lastNumInputChannels = 0;
    int maxIdleSamples;
    int idleCount = 0;
    std::vector<double> gain;
    std::vector<double> rms2;
    std::vector<double> scRms2;
    std::vector<double> oRms2;
    std::vector<double> alphaGain;
    std::vector<double> output;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoVocalCtrlAudioProcessor)
};
