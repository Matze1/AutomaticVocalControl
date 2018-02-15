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
    addParameter(delayLength = new AudioParameterFloat ("delayLength", "DelayLength", 0.0f, (maxDelayInSec * 1000.0f) - 1.0f, 100.0f));
    addParameter(alpha = new AudioParameterFloat ("alpha", "Alpha", 0.0f, 1.0f, 1.0f));
    addParameter(currentGain = new AudioParameterFloat ("currentGain", "CurrentGain", -10.0f, 10.0f, 0.0f));
    addParameter(read = new AudioParameterBool("read","Read",false));
    clipRange = Range<double>(-*gainRange, *gainRange);
    delayBufferLength = 1;
    delayReadPos = 0;
    delayWritePos = 0;
    upBefore = false;
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
    for (int diff = getTotalNumInputChannels() - lastNumInputChannels; diff > 0; --diff) { //vector größe benutzen statt last num inoutksdll
        filterSample.push_back(0.0);
        rms.push_back(0.0);
        mls.push_back(0.0);
        gain.push_back(0.0);
        alphaGain.push_back(0.0);
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

void AutoVocalCtrlAudioProcessor::updateDelay()
{
    int delayInSamples = msToSamples(*delayLength);
    delayReadPos = (int)(delayWritePos - delayInSamples + delayBufferLength) % delayBufferLength;
    setLatencySamples(delayInSamples);
}

//==============================================================================
void AutoVocalCtrlAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    updatePrivateParameter();
    updateVectors();
    lowcut.setCoefficients(38.0, sampleRate, (1.0/2.0));
    highshelf.setCoefficientsShelf(1681.0, sampleRate, 4.0);
    delayBufferLength = (int)(maxDelayInSec*sampleRate);
    if(delayBufferLength < 1)
        delayBufferLength = 1;
    delayBuffer.setSize(2, delayBufferLength);
    delayBuffer.clear();
    updateDelay();
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
        mls[channel] = *loudnessGoal;
//        if (idleCount > maxIdleSamples) {
//            mls[channel] = *loudnessGoal;
//            idleCount = 0;
//        } else {
//            ++idleCount;
//        }
    } else
        mls[channel] = currentRMS;
}

void AutoVocalCtrlAudioProcessor::automateCurrentGain()
{
    *currentGain = getCurrentGainControl();
    beginParameterChangeGesture(currentGain->getParameterIndex());
    endParameterChangeGesture(currentGain->getParameterIndex());
}

void AutoVocalCtrlAudioProcessor::updateAutomation()
{
    const double currGain = getCurrentGainControl();
    const bool up = lastGain < currGain;
    const bool dChange = up != upBefore;
    const bool jump = 1.0 < abs(gainAtPoint - currGain);
    if (dChange || jump) {
        automateCurrentGain();
        upBefore = up;
        gainAtPoint = currGain;
    }
    lastGain = currGain;
}

void AutoVocalCtrlAudioProcessor::updateGain(int channel)
{
    const double prevGain = gain[channel];
    const double g = *loudnessGoal - mls[channel];
    const double co = g < gain[channel] ? compressTCo:expandTCo;
    gain[channel] = clipRange.clipValue((1 - co) * gain[channel] + co * g);
    updateAutomation();
    alphaGain[channel] = *alpha * gain[channel] + (1 - *alpha) * prevGain; // was mache ich damit?
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

    int channel, dpr, dpw;
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    for (channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);
        float* delayData = delayBuffer.getWritePointer(jmin(channel, delayBuffer.getNumChannels() - 1));
        dpr = delayReadPos;
        dpw = delayWritePos;
        
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            if (*read) {
                gain[channel] = *currentGain;
                const double g = pow(10, gain[channel]/20);
                channelData[sample] = channelData[sample] * g;
            } else {
                updateFilterSample(channelData[sample], channel);
                updateRMS(channel);
                updateMLS(channel);
                updateGain(channel);
                const double g = pow(10, gain[channel]/20);
                delayData[dpw] = channelData[sample]; //kann andersrum also erst read dann write falls immer mit lookahead (fest?) nur damit 0 geht
                channelData[sample] = delayData[dpr] * g;
                
                if (++dpr >= delayBufferLength)
                    dpr = 0;
                if (++dpw >= delayBufferLength)
                    dpw = 0;
            }
        }
    }
    
    delayReadPos = dpr;
    delayWritePos = dpw;
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
    if (getTotalNumInputChannels() > 1)
        return (gain[0] + gain[1]) / 2;
    else
        return gain[0];
}
