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
     : AudioProcessor (BusesProperties().withInput  ("Input",  AudioChannelSet::stereo(), true)
                                        .withOutput ("Output", AudioChannelSet::stereo(), true)
                                        .withInput  ("Sidechain", AudioChannelSet::stereo(), true)
                       )
#endif
{
    addParameter(rmsWindow = new AudioParameterFloat ("rmsWindow", "RMSWindow", 10.0f, 500.0f, 60.0f));
    addParameter(expandTime = new AudioParameterFloat ("expandTime", "ExpandTime", 1.0f, 3000.0f, 1500.0f));
    addParameter(compressTime = new AudioParameterFloat ("compressTime", "CompressTime", 1.0f, 1000.0f, 150.0f));
    addParameter(loudnessGoal = new AudioParameterFloat ("loudnessGoal", "LoudnessGoal", -60.0f, 0.0f, -23.0f));
    addParameter(gainRange = new AudioParameterFloat ("gainRange", "GainRange", 0.0f, 10.0f, 6.0f));
    addParameter(maxIdleTime = new AudioParameterFloat ("maxIdleTime", "MaxIdleTime", 0.0f, 500.0f, 0.0f));
    addParameter(gate1 = new AudioParameterFloat ("gate1", "Gate1", -100.0f, -10.0f, -33.0f));
    addParameter(delayLength = new AudioParameterFloat ("delayLength", "DelayLength", 0.0f, (maxDelayInSec * 1000.0f) - 1.0f, 100.0f));
    addParameter(alpha = new AudioParameterFloat ("alpha", "Alpha", 100.0f, 2000.0f, 1000.0f));
    addParameter(currentGain = new AudioParameterFloat ("currentGain", "CurrentGain", -10.0f, 10.0f, 0.0f));
    addParameter(read = new AudioParameterBool("read","Read",false));
    clipRange = Range<double>(-*gainRange, *gainRange);
    delayBufferLength = 1;
    delayReadPos = 0;
    delayWritePos = 0;
    count = 0;
    upBefore = false;
    newLoudness = *loudnessGoal;
    scFilterSample.push_back(0.0); // CHEESY, SAUBERER MACHEN ok? cool.
    scFilterSample.push_back(0.0);
    scRms2.push_back(0.0);
    scRms2.push_back(0.0);
    scGated.push_back(0.0);
    scGated.push_back(0.0);
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

void AutoVocalCtrlAudioProcessor::updateAlphaCo()
{
    alphaCo = getTimeConstant(*alpha);
}

void AutoVocalCtrlAudioProcessor::updateBetaCo()
{
    double sec = (*alpha / 1000.);
    betaCo = 1.f - exp(-2.2*(1./sec)/5.);
}

void AutoVocalCtrlAudioProcessor::updateTimeConstants()
{
    updateRmsCo();
    updateExpandTCo();
    updateCompressTCo();
    updateAlphaCo();
    updateBetaCo();
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
    int diff = getMainBusNumInputChannels() - lastNumInputChannels;
    if (diff > 0) {
        for (diff = diff; diff > 0; --diff) {
            filterSample.push_back(0.0);
            rms2.push_back(0.0);
            gated.push_back(0.0);
            gain.push_back(0.0);
            alphaGain.push_back(0.0);
        }
    } else if (diff < 0) {
        for (diff = diff; diff < 0; ++diff) {
            filterSample.pop_back();
            rms2.pop_back();
            gated.pop_back();
            gain.pop_back();
            alphaGain.pop_back();
        }
        
    }
    lastNumInputChannels = getMainBusNumInputChannels();
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
    scLowcut.setCoefficients(38.0, sampleRate, (1.0/2.0));
    scHighshelf.setCoefficientsShelf(1681.0, sampleRate, 4.0);
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

double AutoVocalCtrlAudioProcessor::updateFilterSample(double sample, AutoVocalCtrlFilter hs, AutoVocalCtrlFilter lc)
{
    return hs.process(lc.process(sample)); //eingehen darauf das sich über die zeit selbst zurückstezen?? wie lange dauert?
}

double AutoVocalCtrlAudioProcessor::updateRMS2(double sample, double last)
{
    return (1. - rmsCo) * last + rmsCo * (sample * sample);
    // RMS Calculation based on Book: Digital Audio Signal Processing by Udo Zölzer
}

double AutoVocalCtrlAudioProcessor::updateGate(double rms2)
{
    const double currentRMS = 10 * std::log10(rms2);
    if (currentRMS < *gate1)
        return *loudnessGoal;
    else
        return currentRMS;
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
//    *currentGain = getCurrentGainControl();
    const bool up = lastGain < currGain;
    const bool dChange = up != upBefore;
    const bool jump = 1.0 < abs(gainAtPoint - currGain);
    const bool waited = count > (currentSampleRate / 4);
    if (dChange || jump) {
        automateCurrentGain();
        upBefore = up;
        gainAtPoint = currGain;
        count = 0;
    }
    ++count;
    lastGain = currGain;
}

void AutoVocalCtrlAudioProcessor::updateLoudnessGoal()
{
    double med = 0;
    for (int i = 0; i < alphaGain.size(); ++i) {
        med += alphaGain[i];
    }
    med = med / alphaGain.size();
    newLoudness = (1 - betaCo) * newLoudness + betaCo * (*loudnessGoal - med);
    *loudnessGoal = newLoudness;
    std::fill(alphaGain.begin(), alphaGain.end(), 0.0);
}

double AutoVocalCtrlAudioProcessor::updateGain(double sample, double scSample, double lastGn, int channel) // channel muss hier noch raus
{
    const double g = *loudnessGoal - sample;
    const double co = g < lastGn ? compressTCo:expandTCo;
    updateAutomation();
    alphaGain[channel] = (1 - alphaCo) * alphaGain[channel] + alphaCo * lastGn;
    if (channel == getMainBusNumInputChannels() - 1)
    {
        ++count2;
        if (count2 > (currentSampleRate * (*alpha / 1000))) {
            updateLoudnessGoal();
            count2 = 0;
        }
    }
    return clipRange.clipValue((1 - co) * lastGn + co * g);
}

void AutoVocalCtrlAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    AudioSampleBuffer mainInputOutput = getBusBuffer(buffer, true, 0);
    AudioSampleBuffer sideChainInput  = getBusBuffer(buffer, true, 1);
    
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getMainBusNumInputChannels();
    const int totalNumOutputChannels = getMainBusNumOutputChannels();
//    const int totalNumInputChannels  = getTotalNumInputChannels();
//    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        mainInputOutput.clear (i, 0, mainInputOutput.getNumSamples());

    int channel, dpr, dpw;
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    for (channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = mainInputOutput.getWritePointer (channel);
        const float* sideChainData = sideChainInput.getReadPointer(sideChainInput.getNumChannels() - 1);
        float* delayData = delayBuffer.getWritePointer(jmin(channel, delayBuffer.getNumChannels() - 1));
        dpr = delayReadPos;
        dpw = delayWritePos;
        
        for (int sample = 0; sample < mainInputOutput.getNumSamples(); ++sample) {
            if (*read) {
                gain[channel] = *currentGain;
                const double g = pow(10, gain[channel]/20);
                channelData[sample] = channelData[sample] * g; //HIER NOCH CLIPPEN!?
            } else {
                // hier deutlich vereinfachen::
                filterSample[channel] = updateFilterSample(channelData[sample], highshelf, lowcut);
                scFilterSample[0] = updateFilterSample(sideChainData[sample], scHighshelf, scLowcut);
                rms2[channel] = updateRMS2(filterSample[channel], rms2[channel]);
                scRms2[0] = updateRMS2(scFilterSample[0], scRms2[0]);
                gated[channel] = updateGate(rms2[channel]);
                scGated[0] = updateGate(scRms2[0]);
                gain[channel] = updateGain(gated[channel], scGated[0], gain[channel], channel);
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
    if (getMainBusNumInputChannels() > 1)
        return (gain[0] + gain[1]) / 2;
    else
        return gain[0];
}
