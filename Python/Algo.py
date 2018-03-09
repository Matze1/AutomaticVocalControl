import scipy.io.wavfile as wave
import matplotlib.pyplot as plt
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
test = 0.

def updateFilterSample(sample):
    global highshelf
    global lowcut
    return highshelf.process(lowcut.process(sample))
#    return sample

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
    rms = 0.0
    global gain
    gain = 0.0
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

r,d1 = helper.readFile("../Vergleich/ex3in.wav")
r,d2 = helper.readFile("../Vergleich/ex3out.wav")

def compareGainCurve(x):
    global r
    global d1
    global d2
    print(x)
    d1_copy = np.copy(d1)
#    d = useAlgorithmOn(r, d1_copy, -19, x[0], x[1], x[2], x[3], 6, x[4])
    d = useAlgorithmOn(r, d1_copy, x[0], x[1], x[2], x[3], x[4], 6, x[5])
#    d = useAlgorithmOn(r, d1_copy, -19, 60, x[0], x[1], -35, 6, x[2])
#    d = useAlgorithmOn(r, d1_copy, -19, 60, 300, 500, x[0], 6, x[1])
    sum = 0
    i = 0
    for sample in d:
        diff = abs(sample - d2[i])
        sum += diff
        i += 1
    print(sum)
#    print(sum / d2.size)
#    return np.sqrt(sum) / d2.size
    test = sum + x[1]/5
#    print(test)
    return test

def deNull(x0):
    x = np.copy(x0)
    i = 0
    for y in x:
        if y == 0.0:
            if i > 0:
                x[i] = x[i-1]
            else:
                x[i] = 1.0
        i += 1
    return x

def gainCurveGraph(x):
    global r
    global d1
    global d2
    d1_copy = np.copy(d1)
    d3 = useAlgorithmOn(r, d1_copy, x[0], x[1], x[2], x[3], x[4], x[5], x[6])
    d1_abs = abs(d1)
    d1_abs[d1_abs == 0] = 1e-10
    d2_abs = abs(d2)
    d3_abs = abs(d3)
    d12 = np.divide(d2_abs, d1_abs)
    d13 = np.divide(d3_abs, d1_abs)
#    d12[d12 == 0] = 1.0
#    d13[d13 == 0] = 1.0
    d12 = deNull(d12)
    d13 = deNull(d13)
    d12dB = 20 * np.log10(d12)
    d13dB = 20 * np.log10(d13)
    plt.figure(1)
    plt.subplot(211)
    plt.plot(d12dB, 'r', d13dB, 'g')
    plt.grid(True)
    plt.ylim(-6,6)
    plt.xlim(0)
    plt.subplot(212)
    plt.plot(d1)
    plt.grid(True)
    plt.ylim(-0.4,0.4)
    plt.xlim(0)
    plt.show()


lowcut.set_coefficients("lowcut", cutoff, r, (1. / 2.))
highshelf.set_coefficients_shelf("highshelf", cutoff_hs, r, 4.)
delayBufferLength = r
#x=rmsWindow, compressTime, expandTime, gate, delay
#x0 = np.array([60, 50, 110, -28, 50])
#bnds = ((10,120),(1,200),(10,300),(-50,-20),(20,150))
#x0 = np.array([300, 500, 100])
#bnds = ((1,1000),(1,1000),(1,999))
#x0 = np.array([40, 40])
#bnds = (slice(-21.0, -17.0, 2.0), slice(10.0, 70.0, 20.0), slice(10.0, 90.0, 20.0), slice(50.0, 170.0, 20.0), slice(-32.0, -26.0, 2.0), slice(10.0, 150.0, 20.0))
#x0 = np.array([-28.0, 50.0])
#x1 = np.array([-17.0, 60.0])
#compareGainCurve(x0)
#compareGainCurve(x1)
#bnds = ((-60.0, -17.0),(20, 150))
#res = optmze.brute(compareGainCurve, bnds, full_output=True, finish=optmze.fmin)
#print(res[0])
#print(res[1])
#res = optmze.minimize(compareGainCurve, x0, bounds=bnds, options={'disp': True, 'eps': 20.0})
#res = optmze.minimize(compareGainCurve, x0, options={'disp': True})
#print(res.x)

x1 = np.array([-30, 50, 333, 400, -35, 6, 30])
gainCurveGraph(x1)


#d = useAlgorithmOn(r, d, -23, 100, 500, 500, -35, 6, 100)
#helper.writeFile("Output.wav", d, r)


#recD = np.append(np.append(np.full(44100, 0.01), np.full(44100, 0.9)), np.full(44100, 0.01))
#helper.writeFile("RecImp2.wav", recD, 44100)

