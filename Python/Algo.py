import scipy.io.wavfile as wave
import scipy.optimize as optmze
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
    dpr = 0
    global dpw
    dpw = 0
    global rms
    rms = 0
    global gain
    gain = 0
    global delayBufferLength
    global delayData
    delayData = np.zeros(delayBufferLength)
    global lowcut
    lowcut.reset_timeBuffer()
    global highshelf
    highshelf.reset_timeBuffer()
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

r,d1 = helper.readFile("../Vergleich/in_short.wav")
r,d2 = helper.readFile("../Vergleich/out_short.wav")

def compareGainCurve(x):
    global r
    global d1
    global d2
    d1_copy = np.copy(d1)
#    d = useAlgorithmOn(r, d1, -19, x[0], x[1], x[2], x[3], 6, x[4])
#    d = useAlgorithmOn(r, d1, -19, 60, x[0], x[1], -35, 6, x[2])
    d = useAlgorithmOn(r, d1_copy, -19, 60, 300, 500, -35, 6, 100)
    sum = 0
    print(sum)
    i = 0
    for sample in d:
        diff = abs(sample - d2[i])
        sum += diff**2
        i += 1
    print(sum)
#    return np.sqrt(sum) / d2.size
    return x[0] + x[1] + sum / 100.0

lowcut.set_coefficients("lowcut", cutoff, r, (1. / 2.))
highshelf.set_coefficients_shelf("highshelf", cutoff_hs, r, 4.)
delayBufferLength = r
#x=rmsWindow, compressTime, expandTime, gate, delay
#x0 = np.array([60, 300, 500, -35, 100])
#bnds = ((10,500),(1,1000),(1,1000),(-100,-10),(1,999))
#x0 = np.array([300, 500, 100])
#bnds = ((1,1000),(1,1000),(1,999))

x0 = np.array([-35.0, 1.0])
x1 = np.array([-25.0, 1.0])
#print(compareGainCurve(x0))
#print(compareGainCurve(x1))
bnds = ((-100.0,-1.0),(0.0,2.0))
res = optmze.minimize(compareGainCurve, x0, bounds=bnds, options={'disp': True})
#res = optmze.minimize(compareGainCurve, x0, options={'disp': True})
print(res.x)

#d = useAlgorithmOn(r, d, -23, 100, 500, 500, -35, 6, 100)
#helper.writeFile("Output.wav", d, r)
