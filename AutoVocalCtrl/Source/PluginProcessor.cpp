/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.
 
    Edited by Nils Heine

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <iostream>


//==============================================================================
AutoVocalCtrlAudioProcessor::AutoVocalCtrlAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations // I/O Ports
     : AudioProcessor (BusesProperties().withInput  ("Input",  AudioChannelSet::stereo(), true)
                                        .withOutput ("Output", AudioChannelSet::stereo(), true)
                                        .withInput  ("Sidechain", AudioChannelSet::stereo(), true)
                       )
#endif
{
    // Initialization of parameters accessible at the UI
    addParameter(loudnessGoal = new AudioParameterFloat ("loudnessGoal", "LoudnessGoal", -60.0f, 0.0f, -20.0f));
    addParameter(gainRange = new AudioParameterFloat ("gainRange", "GainRange", 0.0f, 10.0f, 6.0f));
    addParameter(automationGain = new AudioParameterFloat ("automationGain", "AutomationGain", -15.0f, 15.0f, 0.0f));
    addParameter(scGainUI = new AudioParameterFloat ("scGainUI", "SCGainUI", -9.9f, 9.9f, 0.0f));
    addParameter(oGain = new AudioParameterFloat ("oGain", "OGain", -9.9f, 9.9f, 0.0f));
    addParameter(read = new AudioParameterBool("read","Read",false));
    addParameter(detect = new AudioParameterBool("detect","Detect",false));
    addParameter(scDetect = new AudioParameterBool("scDetect","SCDetect",false));
    addParameter(sc = new AudioParameterBool("sc","SC",false));
    
    // Initialization of remaining parameters
    clipRange = Range<double>(-*gainRange, *gainRange);
    scClipRange = Range<double>(-10.0, 10.0);
    finalClipRange = Range<double>(0.0, 6.0);
    newGate = *loudnessGoal - *gainRange;
    delayBufferLength = 1;
    delayReadPos = 0;
    delayWritePos = 0;
    count = 0;
    detCount = 0;
    gainAtPoint = 0.0;
    delayLength = 50.0;
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
    return 1;
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

/*
 Calculates time constants for adaption processes by current sample rate and duration in miliseconds "ms"
 
 Calculations based on Digital Audio Signal Processing by Udo Zölzer, John Wiley & Sons Lid., 2008
 
 @param ms : duration of adaption process by using the resulting time constant (in ms)
 @return : the time constant
*/
float AutoVocalCtrlAudioProcessor::getTimeConstant(float ms)
{
    if (ms > 0.f)
        return 1.f - exp(-2.2*(1./currentSampleRate)/(ms/1000.));
    else
        return 1.f;
}

/*
 Returns total number of samples in a time window depending on current sample rate
 
 @param ms : length of time window in ms
 @return : total number of samples
*/
int AutoVocalCtrlAudioProcessor::msToSamples(float ms)
{
    return (int) ((currentSampleRate * (ms / 1000)) + 0.5);
}

void AutoVocalCtrlAudioProcessor::updateCompressTCo()
{
    compressTCo = getTimeConstant(600.0);
}

void AutoVocalCtrlAudioProcessor::updateExpandTCo()
{
    ampTCo = getTimeConstant(1500.0);
}

void AutoVocalCtrlAudioProcessor::updateRmsCo()
{
    rmsCo = getTimeConstant(30.0);
}

void AutoVocalCtrlAudioProcessor::updateAlphaCo()
{
    alphaCo = getTimeConstant(1600.0);
}

void AutoVocalCtrlAudioProcessor::updateTimeConstants()
{
    updateRmsCo();
    updateExpandTCo();
    updateCompressTCo();
    updateAlphaCo();
}

// Increases or decreases the dimension of all vectors according to the number of input channels
void AutoVocalCtrlAudioProcessor::updateVectors()
{
    int diff = getMainBusNumInputChannels() - lastNumInputChannels;
    if (diff > 0) {
        for (diff = diff; diff > 0; --diff) {
            rms2.push_back(0.0);
            iRms2.push_back(0.0);
            scRms2.push_back(0.0);
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
            oRms2.pop_back();
            gain.pop_back();
            alphaGain.pop_back();
            betaGain.pop_back();
            v2bDiff.pop_back();
        }
        
    }
    lastNumInputChannels = getMainBusNumInputChannels();
}

// Updates the allowed range for the gain adaption and informs the UI about a change
void AutoVocalCtrlAudioProcessor::updateClipRange()
{
    clipRange = Range<double>(-*gainRange, *gainRange);
    refresh = true;
}

// This function is called when the number od input channels is changed
void AutoVocalCtrlAudioProcessor::numChannelsChanged()
{
    updateVectors();
}

// Initializes the pointers on the delay buffer and informs the DAW about the resulting latency
void AutoVocalCtrlAudioProcessor::updateDelay()
{
    int delayInSamples = msToSamples(delayLength);
    delayReadPos = (int)(delayWritePos - delayInSamples + delayBufferLength) % delayBufferLength;
    setLatencySamples(delayInSamples);
}

//==============================================================================
// Called at every startup
void AutoVocalCtrlAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    updateTimeConstants();
    updateVectors();
    
    // Filter initialization
    lowcut.setCoefficients(38.0, sampleRate, (1.0/2.0));
    highshelf.setCoefficientsShelf(1681.0, sampleRate, 4.0);
    scLowcut.setCoefficients(38.0, sampleRate, (1.0/2.0));
    scHighshelf.setCoefficientsShelf(1681.0, sampleRate, 4.0);
    oLowcut.setCoefficients(38.0, sampleRate, (1.0/2.0));
    oHighshelf.setCoefficientsShelf(1681.0, sampleRate, 4.0);
    iLowcut.setCoefficients(38.0, sampleRate, (1.0/2.0));
    iHighshelf.setCoefficientsShelf(1681.0, sampleRate, 4.0);
    
    // Delay initialization
    delayBufferLength = (int)(sampleRate);
    if(delayBufferLength < 1)
        delayBufferLength = 1;
    delayBuffer.setSize(2, delayBufferLength);
    delayBuffer.clear();
    updateDelay();
    
    // Initializes idle time
    maxIdleSamples = sampleRate/2;
    scMaxIdleSamples = sampleRate*2;
    
    refresh = true;
}

void AutoVocalCtrlAudioProcessor::releaseResources()
{
    
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool AutoVocalCtrlAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // Allowed I/O Layouts
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

// Returns current adapted gain in dB (average of all channels)
double AutoVocalCtrlAudioProcessor::getCurrentGainControl()
{
    double gainControl = 0.0;
    for (int i = 0; i < getMainBusNumInputChannels(); i++) {
        gainControl += gain[i];
    }
    return gainControl / getMainBusNumInputChannels();
}

// Returns current input level in dB (average of all channels)
double AutoVocalCtrlAudioProcessor::getInputRMSdB()
{
    double sum = 0.0;
    for (int i = 0; i < getMainBusNumInputChannels(); ++i)
        sum += iRms2[i];
    return 10 * log10(sum / getMainBusNumInputChannels() + 1e-10);
}

// Returns current side chain input level in dB (average of all channels)
double AutoVocalCtrlAudioProcessor::getScInputRMSdB(int j = -1)
{
    if (j >= 0 && j < numSCChannels)
        return 10 * log10(scRms2[j] + 1e-10);
    double sum = 0;
    for (int i = 0; i < numSCChannels; ++i)
        sum += scRms2[i];
    const double scIn = 10 * log10(sum / numSCChannels + 1e-10);
    return (scIn != scIn) ? -60.0:scIn;
}

// Returns current output level in dB (average of all channels)
double AutoVocalCtrlAudioProcessor::getOutputdB()
{
    double sum = 0;
    for (int i = 0; i < getMainBusNumInputChannels(); ++i)
        sum += oRms2[i];
    return 10 * log10(sum / getMainBusNumInputChannels() + 1e-10);
}

// Returns the current loudness goal detection results (dB difference to previous loudness goal)
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

// Resets side chain input-gain detection
void AutoVocalCtrlAudioProcessor::clearScDetect()
{
    betaGain[0] = 0.0;
    bDetCount = 0;
}

// Returns the current side chain input-gain detection results (dB difference to previous input gain)
double AutoVocalCtrlAudioProcessor::getBetaGain()
{
    if (bDetCount == 0)
        return 0;
    return betaGain[0] / (bDetCount/getMainBusNumInputChannels());
}

/*
 Changes the LoudnessGoal and adjusts the gate
 
 @param newGoal : the new LoudnessGoal
*/
void AutoVocalCtrlAudioProcessor::setLoudnessGoal(double newGoal)
{
    *loudnessGoal = newGoal;
    newGate = *loudnessGoal - *gainRange;
}

// Applies loudnessGoal detection results
void AutoVocalCtrlAudioProcessor::updateLoudnessGoal()
{
    double med = getAlphaGain();
    setLoudnessGoal(*loudnessGoal - med);
    std::fill(alphaGain.begin(), alphaGain.end(), 0.0);
    detCount = 0;
    refresh = true;
}

// Applies side chain input-gain detection results
void AutoVocalCtrlAudioProcessor::updateSCGain()
{
    double med = getBetaGain();
    *scGainUI = *scGainUI - med;
    std::fill(betaGain.begin(), betaGain.end(), 0.0);
    bDetCount = 0;
    refresh = true;
}

/*
 A sample is processed by two filters.
 
 @param sample : the sample to be processed
 @param lc : the first filter (in this case lowcut)
 @param hs : the second filter (in this case highshelf)
 @return : the processed sample
*/
double AutoVocalCtrlAudioProcessor::updateFilterSample(double sample, AutoVocalCtrlFilter hs, AutoVocalCtrlFilter lc)
{
    return hs.process(lc.process(sample));
}

/*
 Calculates a squared root mean square value of the transferred samples (= RMS^2)
 
 Calculations based on Digital Audio Signal Processing by Udo Zölzer, John Wiley & Sons Lid., 2008
 
 @param sample : the current sample to be processed
 @param last : the last determined RMS^2 value
 @return : the RMS^2
*/
double AutoVocalCtrlAudioProcessor::updateRMS2(double sample, double last)
{
    return (1. - rmsCo) * last + rmsCo * (sample * sample);
}

/*
 Converts the transferred sample from linear number space into logarithmic dB.
 Subsequently it replaces all samples below the 'gate' threshold with the current loudnessGoal.
 
 @param rms2 : a squared, rms averaged sample
 @param gate : the threshold of the gate
 @param gain : a gain which can be applied to the sample in dB
 @return : the gated RMS value in dB
*/
double AutoVocalCtrlAudioProcessor::updateGate(double rms2, double gate = -33.0, double gain = 0.0)
{
    const double currentRMS = 10.0 * std::log10(rms2 + 1e-10) + gain; // Final RMS value is determined in dB
    if (currentRMS < gate)
        return *loudnessGoal;
    else
        return currentRMS;
}

// Informs the DAW of a change in the automationGain parameter
void AutoVocalCtrlAudioProcessor::automateCurrentGain()
{
    beginParameterChangeGesture(automationGain->getParameterIndex());
    endParameterChangeGesture(automationGain->getParameterIndex());
}

// Updates the automationGain
void AutoVocalCtrlAudioProcessor::updateAutomation()
{
    const double currGain = getCurrentGainControl();
    const bool jump = 0.1 < abs(gainAtPoint - currGain);
    const bool waited = count > (currentSampleRate * 0.08);
    *automationGain = getCurrentGainControl() + v2bDiff[0];
    if (waited && jump) { // Write automation into DAW after 80ms and a adapted gain change > 0.1 dB
        automateCurrentGain();
        gainAtPoint = currGain;
        count = 0;
    }
    ++count;
}

/*
 Calculates the final gain adaption. Stops the adaption for the duration of the idle time,
 while there is no singal above the gate (sample == loudnessGoal).
 Gain decreasments are adapted faster than increasments (different time coefficients for calculation).
 The resulting gain is clipped at the currently allowed gainRange.
 
 @param sample : a rms averaged and gated sample in dB
 @param lastGn : the last determined gain
 @return : the final gain (to be multiplied with the input signal)
*/
double AutoVocalCtrlAudioProcessor::updateGain(double sample, double lastGn)
{
    if (sample != *loudnessGoal) {
        idleCount = 0;
    } else if (idleCount < maxIdleSamples) {
        idleCount++;
        return lastGn;
    }
    double g = *loudnessGoal - sample;
    double co;
    if (g < lastGn) {
        g = g * 0.66; // Additional compression slope
        co = compressTCo;
    } else
        co = ampTCo;
    return clipRange.clipValue((1 - co) * lastGn + co * g);
}

/*
 Calculates the gain difference between side chain input and loudnessGoal.
 Stops the adaption for the duration of the side chain idle time,
 while there is no singal above the gate (sample == loudnessGoal).
 The resulting gain is clipped at the allowed side chain gainRange.
 
 @param sample : a rms averaged and gated side chain sample in dB
 @param lastGn : the last determined gain
 @return : the resulting gain
*/
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

// Processes the input signal blockwise
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
            
            if (*read) { // Bypass main calculations in read-mode
                gain[channel] = *automationGain;
                double g = pow(10, gain[channel]/20);
                const double fg = finalClipRange.clipValue(g * pow(10, *oGain/20));
                
                // Update I/O Metering for UI
                iRms2[channel] = updateRMS2(updateFilterSample(channelData[sample], iHighshelf, iLowcut), iRms2[channel]);
                oRms2[channel] = updateRMS2(updateFilterSample(channelData[sample] * fg, oHighshelf, oLowcut), oRms2[channel]);
                
                channelData[sample] = channelData[sample] * fg;
                
            } else { // Write-mode
                // Main Calculations
                rms2[channel] = updateRMS2(updateFilterSample(channelData[sample], highshelf, lowcut), rms2[channel]);
                gain[channel] = updateGain(updateGate(rms2[channel], newGate), gain[channel]);
                double g = pow(10, gain[channel]/20);
                delayData[dpw] = channelData[sample];
                
                if (*sc) { // Side chain calculations
                    scRms2[channel] = updateRMS2(updateFilterSample(sideChainData[sample], scHighshelf, scLowcut),
                                                 scRms2[scChannel]);
                    const double scGated = updateGate(scRms2[scChannel], *loudnessGoal - 6.0, *scGainUI);
                    v2bDiff[channel] = updateV2BDiff(scGated, v2bDiff[channel]);
                    g = g * pow(10, v2bDiff[scChannel]/20);
                }
                if (*detect) { // loudnessGoal Detection
                    if (abs(gain[channel]) > 0.1) {
                        alphaGain[channel] += gain[channel];
                        detCount++;
                    }
                    g = 1.0;
                }
                if (*scDetect) { // Side chain input-gain Detection
                    if (abs(v2bDiff[channel]) > 0.1) {
                        betaGain[channel] += v2bDiff[channel];
                        bDetCount++;
                    }
                    g = 1.0;
                }
                
                const double fg = finalClipRange.clipValue(g * pow(10, *oGain/20));
                const double o = delayData[dpr] * fg;
                
                // Update I/O Metering for UI
                iRms2[channel] = updateRMS2(updateFilterSample(delayData[dpr], iHighshelf, iLowcut), iRms2[channel]);
                oRms2[channel] = updateRMS2(updateFilterSample(o, oHighshelf, oLowcut), oRms2[channel]);
                
                channelData[sample] = o;
                
                // Update pointers on delay buffer
                if (++dpr >= delayBufferLength)
                    dpr = 0;
                if (++dpw >= delayBufferLength)
                    dpw = 0;
                
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
    return true;
}

AudioProcessorEditor* AutoVocalCtrlAudioProcessor::createEditor()
{
    return new AutoVocalCtrlAudioProcessorEditor (*this);
}

//==============================================================================
// Saves current parameter setting in an xml file
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

// Retrieves parameter setting from xml file, communicates change to UI
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
