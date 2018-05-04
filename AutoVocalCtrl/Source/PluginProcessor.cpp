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
    expandTime = 1500.0;
    compressTime = 600.0;
    delayLength = 50.0;
    alpha = 1600.0;
    addParameter(loudnessGoal = new AudioParameterFloat ("loudnessGoal", "LoudnessGoal", -60.0f, 0.0f, -20.0f));
    addParameter(gainRange = new AudioParameterFloat ("gainRange", "GainRange", 0.0f, 10.0f, 6.0f));
    addParameter(automationGain = new AudioParameterFloat ("automationGain", "AutomationGain", -15.0f, 15.0f, 0.0f));
    addParameter(scGainUI = new AudioParameterFloat ("scGainUI", "SCGainUI", -9.9f, 9.9f, 0.0f));
    addParameter(oGain = new AudioParameterFloat ("oGain", "OGain", -9.9f, 9.9f, 0.0f));
    addParameter(read = new AudioParameterBool("read","Read",false));
    addParameter(detect = new AudioParameterBool("detect","Detect",false));
    addParameter(scDetect = new AudioParameterBool("scDetect","SCDetect",false));
    addParameter(sc = new AudioParameterBool("sc","SC",false));
    clipRange = Range<double>(-*gainRange, *gainRange);
    scClipRange = Range<double>(-10.0, 10.0);
    finalClipRange = Range<double>(0.0, 6.0);
    newGate = *loudnessGoal - *gainRange;
    delayBufferLength = 1;
    delayReadPos = 0;
    delayWritePos = 0;
    count = 0;
    detCount = 0;
    upBefore = false;
    refresh = true;
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
    compressTCo = getTimeConstant(compressTime);
}

void AutoVocalCtrlAudioProcessor::updateExpandTCo()
{
    expandTCo = getTimeConstant(expandTime);
}

void AutoVocalCtrlAudioProcessor::updateRmsCo()
{
    rmsCo = getTimeConstant(30.0);
}

void AutoVocalCtrlAudioProcessor::updateAlphaCo()
{
    alphaCo = getTimeConstant(alpha);
}

void AutoVocalCtrlAudioProcessor::updateBetaCo()
{
    double sec = (alpha / 1000.);
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

void AutoVocalCtrlAudioProcessor::updatePrivateParameter()
{
    updateTimeConstants();
//    updateMaxIdleSamples();
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
            betaGain.push_back(0.0);
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
            betaGain.pop_back();
            v2bDiff.pop_back();
        }
        
    }
    lastNumInputChannels = getMainBusNumInputChannels();
}

void AutoVocalCtrlAudioProcessor::updateClipRange()
{
    clipRange = Range<double>(-*gainRange, *gainRange);
    refresh = true;
}


void AutoVocalCtrlAudioProcessor::numChannelsChanged()
{
    updateVectors();
}

void AutoVocalCtrlAudioProcessor::updateDelay()
{
    int delayInSamples = msToSamples(delayLength);
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
    maxIdleSamples = sampleRate/2;
    scMaxIdleSamples = sampleRate*2;
    refresh = true;
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

double AutoVocalCtrlAudioProcessor::updateRMS2(double sample, double last)
{
    return (1. - rmsCo) * last + rmsCo * (sample * sample);
    // RMS Calculation based on Book: Digital Audio Signal Processing by Udo Zölzer
}

double AutoVocalCtrlAudioProcessor::updateGate(double rms2, double gate = -33.0, double gain = 0.0)
{
    const double currentRMS = 10.0 * std::log10(rms2 + 1e-10) + gain; //in text ändern zumindest bei sc part
    if (currentRMS < gate)
        return *loudnessGoal;
    else
        return currentRMS;
}

void AutoVocalCtrlAudioProcessor::automateCurrentGain()
{
//    *currentGain = getCurrentGainControl();
    beginParameterChangeGesture(automationGain->getParameterIndex());
    endParameterChangeGesture(automationGain->getParameterIndex());
}

void AutoVocalCtrlAudioProcessor::updateAutomation()
{
    const double currGain = getCurrentGainControl();
//    const bool up = lastGain < currGain;
//    const bool dChange = up != upBefore;
    const bool jump = 0.1 < abs(gainAtPoint - currGain);
    const bool waited = count > (currentSampleRate * 0.08);
    *automationGain = getCurrentGainControl() + v2bDiff[0];
    if (waited && jump) {
        automateCurrentGain();
//        upBefore = up;
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
    if (j >= 0 && j < numSCChannels)
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
    const double cdC = detCount/getMainBusNumInputChannels();
    for (int i = 0; i < getMainBusNumInputChannels(); ++i)
        med += alphaGain[i] / cdC;
    return med / getMainBusNumInputChannels();
}

void AutoVocalCtrlAudioProcessor::clearScDetect()
{
    betaGain[0] = 0.0;
    bDetCount = 0;
}

double AutoVocalCtrlAudioProcessor::getBetaGain()
{
    if (bDetCount == 0)
        return 0;
    return betaGain[0] / (bDetCount/getMainBusNumInputChannels());
}

void AutoVocalCtrlAudioProcessor::setLoudnessGoal(double newGoal)
{
    *loudnessGoal = newGoal;
    newGate = *loudnessGoal - *gainRange;
}

void AutoVocalCtrlAudioProcessor::updateLoudnessGoal()
{
    double med = getAlphaGain();
    setLoudnessGoal(*loudnessGoal - med);
    std::fill(alphaGain.begin(), alphaGain.end(), 0.0);
    detCount = 0;
}

void AutoVocalCtrlAudioProcessor::updateSCGain()
{
    double med = getBetaGain();
    *scGainUI = *scGainUI - med;
    std::fill(betaGain.begin(), betaGain.end(), 0.0);
    bDetCount = 0;
}

double AutoVocalCtrlAudioProcessor::updateGain(double sample, double lastGn)
{
    if (sample != *loudnessGoal) {
        idleCount = 0;
    } else if (idleCount < maxIdleSamples) {
        idleCount++;
        return lastGn;
    }
    double g = *loudnessGoal - sample;
//    const double co = g < lastGn ? compressTCo:expandTCo;
    double co;
    if (g < lastGn) {
        g = g * 0.66;
        co = compressTCo;
    } else
        co = expandTCo;
    return clipRange.clipValue((1 - co) * lastGn + co * g);
}

double AutoVocalCtrlAudioProcessor::updateV2BDiff(double sample, double lastGn)
{
    if (sample != *loudnessGoal) {
        scIdleCount = 0;
    } else if (scIdleCount < scMaxIdleSamples) {
        scIdleCount++;
        return lastGn;
    }
    return scClipRange.clipValue((1 - alphaCo) * lastGn + alphaCo * (sample - *loudnessGoal));
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
                gain[channel] = *automationGain;
                double g = pow(10, gain[channel]/20);
                const double fg = finalClipRange.clipValue(g * pow(10, *oGain/20));
                oRms2[channel] = updateRMS2(channelData[sample] * fg, oRms2[channel]);
                channelData[sample] = channelData[sample] * fg; //HIER NOCH CLIPPEN!?
            } else {
                if (*sc) {
                    scRms2[channel] = updateRMS2(updateFilterSample(sideChainData[sample], scHighshelf, scLowcut),
                                                 scRms2[scChannel]);
                }
                rms2[channel] = updateRMS2(updateFilterSample(channelData[sample], highshelf, lowcut), rms2[channel]);
                gain[channel] = updateGain(updateGate(rms2[channel], newGate), gain[channel]);
                double g = pow(10, gain[channel]/20);
                delayData[dpw] = channelData[sample];
                if (*sc) {
                    const double scGated = updateGate(scRms2[scChannel], *loudnessGoal - 6.0, *scGainUI);
                    v2bDiff[channel] = updateV2BDiff(scGated, v2bDiff[channel]);
                    g = g * pow(10, v2bDiff[scChannel]/20); //sinnvoll oder mittelwert aus beiden seiten? auch für zeile darüber?
                }
                if (*detect) {
                    if (abs(gain[channel]) > 0.1) {
                        alphaGain[channel] += gain[channel];
                        detCount++;
                    }
                    g = 1.0;
                }
                if (*scDetect) {
                    if (abs(v2bDiff[channel]) > 0.1) {
                        betaGain[channel] += v2bDiff[channel];
                        bDetCount++;
                    }
                    g = 1.0;
                }
                const double fg = finalClipRange.clipValue(g * pow(10, *oGain/20));
                const double o = delayData[dpr] * fg;
                iRms2[channel] = updateRMS2(updateFilterSample(delayData[dpr], iHighshelf, iLowcut), iRms2[channel]);
                oRms2[channel] = updateRMS2(updateFilterSample(o, oHighshelf, oLowcut), oRms2[channel]);
                channelData[sample] = o;
                
                if (++dpr >= delayBufferLength)
                    dpr = 0;
                if (++dpw >= delayBufferLength)
                    dpw = 0;
                
                //neue position auch im text ändern:
                updateAutomation();
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
    ScopedPointer<XmlElement> xml (new XmlElement ("AutoVocalCtrl"));
    xml->setAttribute ("sc", (bool) *sc);
    xml->setAttribute ("read", (bool) *read);
    xml->setAttribute ("scDetect", (bool) *scDetect);
    xml->setAttribute ("detect", (bool) *detect);
    xml->setAttribute ("loudnessGoal", (float) *loudnessGoal);
    xml->setAttribute ("gainRange", (float) *gainRange);
    xml->setAttribute ("scGainUI", (float) *scGainUI);
    xml->setAttribute ("oGain", (float) *oGain);
    copyXmlToBinary (*xml, destData);
}

void AutoVocalCtrlAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    ScopedPointer<XmlElement> xmlState (getXmlFromBinary (data, sizeInBytes));
    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName ("AutoVocalCtrl"))
        {
            *sc = (bool) xmlState->getBoolAttribute ("sc", false);
            *read = (bool) xmlState->getBoolAttribute ("read", false);
            *scDetect = (bool) xmlState->getBoolAttribute ("scDetect", false);
            *detect = (bool) xmlState->getBoolAttribute ("detect", false);
            *loudnessGoal = (float) xmlState->getDoubleAttribute ("loudnessGoal", -20.0);
            *gainRange = (float) xmlState->getDoubleAttribute ("gainRange", 6.0);
            *scGainUI = (float) xmlState->getDoubleAttribute ("scGainUI", 0.0);
            *oGain = (float) xmlState->getDoubleAttribute ("oGain", 0.0);
        }
    }
    refresh = true;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AutoVocalCtrlAudioProcessor();
}

double AutoVocalCtrlAudioProcessor::getCurrentGainControl()
{
    if (*read) {
        return *automationGain;
    }
    double gainControl = 0.0;
    for (int i = 0; i < getMainBusNumInputChannels(); i++) {
        gainControl += gain[i];
    }
    return gainControl / getMainBusNumInputChannels();
}
