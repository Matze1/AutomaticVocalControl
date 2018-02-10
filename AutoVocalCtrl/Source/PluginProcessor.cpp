/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>


//==============================================================================
AutoVocalCtrlAudioProcessor::AutoVocalCtrlAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    addParameter(rmsWindow = new AudioParameterFloat ("rmsWindow", "RMSWindow", 10.0f, 500.0f, 60.0f));
    addParameter(expandTime = new AudioParameterFloat ("expandTime", "ExpandTime", 1.0f, 1000.0f, 500.0f));
    addParameter(compressTime = new AudioParameterFloat ("compressTime", "CompressTime", 1.0f, 1000.0f, 300.0f));
    addParameter(loudnessGoal = new AudioParameterFloat ("loudnessGoal", "LoudnessGoal", -60.0f, 0.0f, -23.0f));
    addParameter(gainRange = new AudioParameterFloat ("gainRange", "GainRange", 0.0f, 10.0f, 6.0f));
    addParameter(maxIdleTime = new AudioParameterFloat ("maxIdleTime", "MaxIdleTime", 0.0f, 500.0f, 0.0f));
    addParameter(gate1 = new AudioParameterFloat ("gate1", "Gate1", -100.0f, -10.0f, -35.0f));
    clipRange = Range<double>(-*gainRange, *gainRange);
}

AutoVocalCtrlAudioProcessor::~AutoVocalCtrlAudioProcessor()
{
}

//==============================================================================
const String AutoVocalCtrlAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool AutoVocalCtrlAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AutoVocalCtrlAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AutoVocalCtrlAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AutoVocalCtrlAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AutoVocalCtrlAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AutoVocalCtrlAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AutoVocalCtrlAudioProcessor::setCurrentProgram (int index)
{
}

const String AutoVocalCtrlAudioProcessor::getProgramName (int index)
{
    return {};
}

void AutoVocalCtrlAudioProcessor::changeProgramName (int index, const String& newName)
{
}

// calculates time constants by current sample rate and miliseconds "ms"
float AutoVocalCtrlAudioProcessor::getTimeConstant(float ms)
{
    // Calculation is based on the Book: Digital Audio Signal Processing by Udo Zölzer
    if (ms > 0.f)
        return 1.f - exp(-2.2*(1./currentSampleRate)/(ms/1000.));
    else
        return 1.f;
}

int AutoVocalCtrlAudioProcessor::msToSamples(float ms)
{
    return (int) ((currentSampleRate * (ms / 1000)) + 0.5);
}

void AutoVocalCtrlAudioProcessor::updateCompressTCo()
{
    compressTCo = getTimeConstant(*compressTime);
}

void AutoVocalCtrlAudioProcessor::updateExpandTCo()
{
    expandTCo = getTimeConstant(*expandTime);
}

void AutoVocalCtrlAudioProcessor::updateRmsCo()
{
    rmsCo = getTimeConstant(*rmsWindow);
}

void AutoVocalCtrlAudioProcessor::updateTimeConstants()
{
    updateRmsCo();
    updateExpandTCo();
    updateCompressTCo();
}

void AutoVocalCtrlAudioProcessor::updateMaxIdleSamples()
{
    maxIdleSamples = msToSamples(*maxIdleTime);
}

void AutoVocalCtrlAudioProcessor::updatePrivateParameter()
{
    updateTimeConstants();
    updateMaxIdleSamples();
}

void AutoVocalCtrlAudioProcessor::updateVectors()
{
    for (int diff = getTotalNumInputChannels() - lastNumInputChannels; diff > 0; --diff) {
        filterSample.push_back(0.0);
        rms.push_back(0.0);
        mls.push_back(0.0);
        gain.push_back(0.0);
    }
}

void AutoVocalCtrlAudioProcessor::updateClipRange()
{
    clipRange = Range<double>(-*gainRange, *gainRange);
}


void AutoVocalCtrlAudioProcessor::numChannelsChanged()
{
    updateVectors();
}

//==============================================================================
void AutoVocalCtrlAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    updatePrivateParameter();
    updateVectors();
    lowcut.setCoefficients(38.0, sampleRate, (1.0/2.0));
    highshelf.setCoefficientsShelf(1681.0, sampleRate, 4.0);
}

void AutoVocalCtrlAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AutoVocalCtrlAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void AutoVocalCtrlAudioProcessor::updateFilterSample(double sample, int channel)
{
    filterSample[channel] = highshelf.process(lowcut.process(sample));
}

void AutoVocalCtrlAudioProcessor::updateRMS(int channel)
{
    rms[channel] = (1. - rmsCo) * rms[channel] + rmsCo * (filterSample[channel] * filterSample[channel]);
    // RMS Calculation based on Book: Digital Audio Signal Processing by Udo Zölzer
}

void AutoVocalCtrlAudioProcessor::updateMLS(int channel)
{
    const double currentRMS = 10 * std::log10(rms[channel]);
    if (currentRMS < *gate1) { // gate
        if (idleCount > maxIdleSamples) {
            mls[channel] = *loudnessGoal;
            idleCount = 0;
        } else {
            ++idleCount;
        }
    } else
        mls[channel] = currentRMS;
}

void AutoVocalCtrlAudioProcessor::updateGain(int channel)
{
    const float g = *loudnessGoal - mls[channel];
    const float co = g < gain[channel] ? compressTCo:expandTCo;
    gain[channel] = clipRange.clipValue((1 - co) * gain[channel] + co * g);
}

void AutoVocalCtrlAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            updateFilterSample(channelData[sample], channel);
            updateRMS(channel);
            updateMLS(channel);
            updateGain(channel);
            const double g = pow(10, gain[channel]/20);
            channelData[sample] = channelData[sample] * g;
        }
    }
}

//==============================================================================
bool AutoVocalCtrlAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* AutoVocalCtrlAudioProcessor::createEditor()
{
    return new AutoVocalCtrlAudioProcessorEditor (*this);
}

//==============================================================================
void AutoVocalCtrlAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void AutoVocalCtrlAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AutoVocalCtrlAudioProcessor();
}

double AutoVocalCtrlAudioProcessor::getCurrentGainControl()
{
    return gain[0];
}
