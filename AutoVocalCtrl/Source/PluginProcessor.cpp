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
    addParameter(rmsWindow = new AudioParameterFloat ("rmsWindow", "RMSWindow", 10.0f, 500.0f, 220.0f));
    addParameter(expandTime = new AudioParameterFloat ("expandTime", "ExpandTime", 1.0f, 3000.0f, 1500.0f));
    addParameter(compressTime = new AudioParameterFloat ("compressTime", "CompressTime", 1.0f, 1000.0f, 600.0f));
    addParameter(loudnessGoal = new AudioParameterFloat ("loudnessGoal", "LoudnessGoal", -60.0f, 0.0f, -20.0f));
    addParameter(gainRange = new AudioParameterFloat ("gainRange", "GainRange", 0.0f, 10.0f, 6.0f));
    addParameter(maxIdleTime = new AudioParameterFloat ("maxIdleTime", "MaxIdleTime", 0.0f, 500.0f, 0.0f));
    addParameter(gate1 = new AudioParameterFloat ("gate1", "Gate1", -100.0f, -10.0f, -33.0f));
    addParameter(delayLength = new AudioParameterFloat ("delayLength", "DelayLength", 0.0f, (maxDelayInSec * 1000.0f) - 1.0f, 60.0f));
    addParameter(alpha = new AudioParameterFloat ("alpha", "Alpha", 100.0f, 4000.0f, 1600.0f));
    addParameter(currentGain = new AudioParameterFloat ("currentGain", "CurrentGain", -10.0f, 10.0f, 0.0f));
    addParameter(scGainUI = new AudioParameterFloat ("scGainUI", "SCGainUI", -10.0f, 10.0f, 0.0f));
    addParameter(read = new AudioParameterBool("read","Read",false));
    addParameter(detect = new AudioParameterBool("detect","Detect",false));
    addParameter(sc = new AudioParameterBool("sc","SC",false));
    addParameter(scf = new AudioParameterBool("scf","SCF",true));
    clipRange = Range<double>(-*gainRange, *gainRange);
    delayBufferLength = 1;
    delayReadPos = 0;
    delayWritePos = 0;
    count = 0;
    detCount = 0;
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
    rmsCo = getTimeConstant(40.0);
    scRmsCo = getTimeConstant(*rmsWindow);
    scRmsCoFast = getTimeConstant(10.0);
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
            rms2.push_back(0.0);
            iRms2.push_back(0.0);
            scRms2.push_back(0.0);
            scRms2Fast.push_back(0.0);
            oRms2.push_back(0.0);
            gain.push_back(0.0);
            alphaGain.push_back(0.0);
            v2bDiff.push_back(0.0);
        }
    } else if (diff < 0) {
        for (diff = diff; diff < 0; ++diff) {
            rms2.pop_back();
            iRms2.pop_back();
            scRms2.pop_back();
            scRms2Fast.pop_back();
            oRms2.pop_back();
            gain.pop_back();
            alphaGain.pop_back();
            v2bDiff.pop_back();
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
    highshelf.setCoefficientsShelf(1681.0, sampleRate, 4.0); // so oft das selbe? einfacher machen..
    scLowcut.setCoefficients(38.0, sampleRate, (1.0/2.0));
    scHighshelf.setCoefficientsShelf(1681.0, sampleRate, 4.0);
    scfLowcut.setCoefficients(38.0, sampleRate, (1.0/2.0));
    scfHighshelf.setCoefficientsShelf(1681.0, sampleRate, 4.0);
    oLowcut.setCoefficients(38.0, sampleRate, (1.0/2.0));
    oHighshelf.setCoefficientsShelf(1681.0, sampleRate, 4.0);
    iLowcut.setCoefficients(38.0, sampleRate, (1.0/2.0));
    iHighshelf.setCoefficientsShelf(1681.0, sampleRate, 4.0);
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
    
//    if (layouts.getChannelSet(true, 1) != AudioChannelSet::stereo())
//        return false;
    
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

double AutoVocalCtrlAudioProcessor::updateRMS2(double sample, double last, double co)
{
    return (1. - co) * last + co * (sample * sample);
    // RMS Calculation based on Book: Digital Audio Signal Processing by Udo Zölzer
}

double AutoVocalCtrlAudioProcessor::updateGate(double rms2, double gate = -33.0)
{
    const double currentRMS = 10.0 * std::log10(rms2 + 1e-10);
    if (currentRMS < gate)
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
    const bool up = lastGain < currGain;
    const bool dChange = up != upBefore;
    const bool jump = 1.0 < abs(gainAtPoint - currGain);
//    const bool waited = count > (currentSampleRate / 4);
    if (dChange || jump) {
        automateCurrentGain();
        upBefore = up;
        gainAtPoint = currGain;
        count = 0;
    }
    ++count;
    lastGain = currGain;
}

double AutoVocalCtrlAudioProcessor::getInputRMSdB()
{
    double sum = 0.0;
    for (int i = 0; i < getMainBusNumInputChannels(); ++i)
        sum += iRms2[i];
    return 10 * log10(sum / getMainBusNumInputChannels() + 1e-10);
}

double AutoVocalCtrlAudioProcessor::getScInputRMSdB(int j = -1)
{
    if (j > 0 && j < numSCChannels)
        return 10 * log10(scRms2[j] + 1e-10);
    double sum = 0;
    for (int i = 0; i < numSCChannels; ++i)
        sum += scRms2[i];
    return 10 * log10(sum / numSCChannels + 1e-10);
}

double AutoVocalCtrlAudioProcessor::getOutputdB()
{
    double sum = 0;
    for (int i = 0; i < getMainBusNumInputChannels(); ++i)
        sum += oRms2[i];
    return 10 * log10(sum / getMainBusNumInputChannels() + 1e-10);
}

double AutoVocalCtrlAudioProcessor::getAlphaGain()
{
    if (detCount == 0)
        return 0;
    double med = 0;
    for (int i = 0; i < getMainBusNumInputChannels(); ++i)
        med += alphaGain[i] / detCount;
    return med / getMainBusNumInputChannels();
}

void AutoVocalCtrlAudioProcessor::updateLoudnessGoal()
{
    double med = getAlphaGain();
    *loudnessGoal = *loudnessGoal - med;
    std::fill(alphaGain.begin(), alphaGain.end(), 0.0);
    detCount = 0;
}

double AutoVocalCtrlAudioProcessor::updateGain(double sample, double lastGn)
{
    const double g = *loudnessGoal - sample;
    const double co = g < lastGn ? compressTCo:expandTCo;
    updateAutomation();
    return clipRange.clipValue((1 - co) * lastGn + co * g);
}

void AutoVocalCtrlAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    AudioSampleBuffer mainInputOutput = getBusBuffer(buffer, true, 0);
    AudioSampleBuffer sideChainInput  = getBusBuffer(buffer, true, 1);
    numSCChannels = sideChainInput.getNumChannels();
    
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels  = getMainBusNumInputChannels();
    const int totalNumOutputChannels = getMainBusNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        mainInputOutput.clear (i, 0, mainInputOutput.getNumSamples());

    int channel, dpr, dpw;
    
    for (channel = 0; channel < totalNumInputChannels; ++channel)
    {
        float* channelData = mainInputOutput.getWritePointer (channel);
        const int scChannel = channel % numSCChannels;
        const float* sideChainData = sideChainInput.getReadPointer(scChannel);
        float* delayData = delayBuffer.getWritePointer(jmin(channel, delayBuffer.getNumChannels() - 1));
        dpr = delayReadPos;
        dpw = delayWritePos;
        
        for (int sample = 0; sample < mainInputOutput.getNumSamples(); ++sample) {
            if (*read) {
                gain[channel] = *currentGain;
                const double g = pow(10, gain[channel]/20);
                oRms2[channel] = updateRMS2(channelData[sample] * g, oRms2[channel], rmsCo);
                channelData[sample] = channelData[sample] * g; //HIER NOCH CLIPPEN!?
            } else {
                if (*sc) {
                    scRms2[channel] = updateRMS2(updateFilterSample(sideChainData[sample], scHighshelf, scLowcut),
                                                 scRms2[scChannel], scRmsCo);
                    scRms2Fast[channel] = updateRMS2(updateFilterSample(sideChainData[sample], scfHighshelf, scfLowcut),
                                                 scRms2Fast[scChannel], scRmsCoFast);
                }
                rms2[channel] = updateRMS2(updateFilterSample(channelData[sample], highshelf, lowcut), rms2[channel], rmsCo);
                const double newGate = *loudnessGoal - *gainRange;
                gain[channel] = updateGain(updateGate(rms2[channel], newGate), gain[channel]);
                double g = pow(10, gain[channel]/20);
                delayData[dpw] = channelData[sample];
                if (*sc) {
                    double scGated = updateGate(scRms2Fast[scChannel], newGate);
                    if (scGated != *loudnessGoal || !*scf)
                        scGated = updateGate(scRms2[scChannel], newGate);
                    v2bDiff[channel] = (1 - alphaCo) * v2bDiff[channel] +
                                       alphaCo * ((scGated + *scGainUI) - *loudnessGoal);
                    g = g * pow(10, v2bDiff[scChannel]/20); //sinnvoll oder mittelwert aus beiden seiten? auch für zeile darüber?
                }
                if (*detect && (abs(gain[channel]) > 0.1)) {
                    alphaGain[channel] += gain[channel];
                    detCount++;
                    g = 1.0;
                }
                const double o = delayData[dpr] * g;
                iRms2[channel] = updateRMS2(updateFilterSample(delayData[dpr], iHighshelf, iLowcut), iRms2[channel], rmsCo);
                oRms2[channel] = updateRMS2(updateFilterSample(o, oHighshelf, oLowcut), oRms2[channel], rmsCo);
                channelData[sample] = o;
                
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
    double gainControl = 0.0;
    for (int i = 0; i < getMainBusNumInputChannels(); i++) {
        gainControl += gain[i];
    }
    return gainControl / getMainBusNumInputChannels();
}
