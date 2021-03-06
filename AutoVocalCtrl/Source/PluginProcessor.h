/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.
 
    Edited by Nils Heine

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
    void updateTimeConstants();
    void updateVectors();
    void updateClipRange();
    void updateDelay();
    void updateAutomation();
    void updateLoudnessGoal();
    void updateSCGain();
    void automateCurrentGain();
    void setLoudnessGoal(double newGoal);
    void clearScDetect();

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    double getCurrentGainControl();
    double getAlphaGain();
    double getBetaGain();
    double getInputRMSdB();
    double getScInputRMSdB(int j);
    double getOutputdB();
    
    //==============================================================================
    double updateFilterSample(double sample, AutoVocalCtrlFilter hs, AutoVocalCtrlFilter lc);
    double updateRMS2(double sample, double last);
    double updateGate(double rms2, double gate, double gain);
    double updateGain(double sample, double lastGn);
    double updateV2BDiff(double sample, double lastGn);
    
    //==============================================================================
    void numChannelsChanged() override;
    
    //==============================================================================
    AudioParameterFloat* loudnessGoal;
    AudioParameterFloat* gainRange;
    AudioParameterFloat* automationGain;
    AudioParameterFloat* scGainUI;
    AudioParameterFloat* oGain;
    AudioParameterBool* read;
    AudioParameterBool* detect;
    AudioParameterBool* scDetect;
    AudioParameterBool* sc;
    std::vector<double> v2bDiff;
    bool refresh;
    

private:
    //==============================================================================
    AudioSampleBuffer delayBuffer;
    int delayBufferLength;
    int delayReadPos, delayWritePos;
    double delayLength;
    double newGate;
    int count;
    int count2;
    int detCount;
    int bDetCount;
    int numSCChannels;
    AutoVocalCtrlFilter lowcut;
    AutoVocalCtrlFilter highshelf;
    AutoVocalCtrlFilter scLowcut;
    AutoVocalCtrlFilter scHighshelf;
    AutoVocalCtrlFilter oLowcut;
    AutoVocalCtrlFilter oHighshelf;
    AutoVocalCtrlFilter iLowcut;
    AutoVocalCtrlFilter iHighshelf;
    Range<double> clipRange;
    Range<double> scClipRange;
    Range<double> finalClipRange;
    double currentSampleRate;
    double rmsCo;
    double scRmsCo;
    double ampTCo;
    double compressTCo;
    double alphaCo;
    double scGain;
    double gainAtPoint;
    int lastNumInputChannels = 0;
    int maxIdleSamples;
    int scMaxIdleSamples;
    int idleCount = 0;
    int scIdleCount = 0;
    int newIdle = 0;
    std::vector<double> gain;
    std::vector<double> rms2;
    std::vector<double> iRms2;
    std::vector<double> scRms2;
    std::vector<double> oRms2;
    std::vector<double> alphaGain;
    std::vector<double> betaGain;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AutoVocalCtrlAudioProcessor)
};
