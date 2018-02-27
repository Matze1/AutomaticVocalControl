import scipy.io.wavfile as wave
import scipy.optimzie as optmze
import numpy as np
from Filter import Filter
from Helper import Helper

helper = Helper()
gain = 0.
rms = 0.
dpr = 0
dpw = 0
delayData = []
delayBufferLength = 0
lowcut = Filter()
highshelf = Filter()
cutoff = 38.
cutoff_hs = 1681.

def updateFilterSample(sample):
    global highshelf
    global lowcut
    return highshelf.process(lowcut.process(sample))

def updateRMS(sample, avCo):
    global rms
    rms = (1. - avCo) * rms + avCo * (sample * sample)
    return rms

def updateMLS(sample, gate, goal):
    currentRMS = 10 * np.log10(sample + 1e-10)
    if (currentRMS < gate):
        return goal
    else:
        return currentRMS

def updateGain(sample, goal, compressTCo, expandTCo, maxGain):
    global gain
    g = goal - sample;
    if (g < gain):
        co = compressTCo
    else:
        co = expandTCo
    gain = np.clip(((1 - co) * gain + co * g), -maxGain, maxGain)
    return gain

def useAlgorithmOn(rate, data, goal, rmsWindow, compressTime, expandTime, gate, maxGain, delay):
    global dpr
    global dpw
    global delayData
    global delayBufferLength
    delayInSamples = helper.getSamplesPerMs(delay, rate)
    dpr = int((dpw - delayInSamples + delayBufferLength) % delayBufferLength)
    if (data.ndim > 1): #GEHT NUR FÜR MONO!!!
        print ("only mono or stereo files pls")
        return data
    avCo = helper.getTimeConstant(rmsWindow, rate)
    compressTCo = helper.getTimeConstant(compressTime, rate)
    expandTCo = helper.getTimeConstant(expandTime, rate)
    i = -delayInSamples
    delayZeros = np.zeros(delayInSamples)
    for sample in np.append(data,delayZeros):
        g = pow(10, updateGain(updateMLS(updateRMS(updateFilterSample(sample), avCo), gate, goal),
                               goal, compressTCo, expandTCo, maxGain)/20)
        delayData[dpw] = sample
        if (i >= 0):
            data[i] = delayData[dpr] * g
        
        dpr += 1
        dpw += 1
        if (dpr >= delayBufferLength):
            dpr = 0;
        if (dpw >= delayBufferLength):
            dpw = 0;
        
        i += 1
    
    #delaybuffer abziehen???
    return data

def compareGainCurve(x):
    global r
    global d1
    global d2
    d = useAlgorithmOn(r, d1, -19, x[0], x[1], x[2], x[3], 6, x[4])
    sum = 0
    i = 0
    for sample in d:
        sum += abs(sample - d2[i])
        i += 1
    return sum / x.size()

r,d1 = helper.readFile("../Vergleich/original16.wav")
r,d2 = helper.readFile("../Vergleich/withRider16.wav")
lowcut.set_coefficients("lowcut", cutoff, r, (1. / 2.))
highshelf.set_coefficients_shelf("highshelf", cutoff_hs, r, 4.)
delayBufferLength = r
delayData = np.zeros(delayBufferLength)
#x=rmsWindow, compressTime, expandTime, gate, delay
x0 = np.array([60, 300, 500, -35, 100])
bnds = ((10,500),(1,1000),(1,1000),(-100,-10),(1,999))
optmze.minimize(compareGainCurve, x0, bounds=bnds, options={'disp': True})
print(res.x)

#d = useAlgorithmOn(r, d, -23, 100, 500, 500, -35, 6, 100)
#helper.writeFile("Output.wav", d, r)
