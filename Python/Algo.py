import scipy.io.wavfile as wave
from scipy.fftpack import fft, fftfreq
import numpy as np
import matplotlib.pyplot as plt
from Filter import Filter

br = 16
inputData = []

def readFile(name):
    global br
    global inputData
    rate, data = wave.read(name)            # sample rate und daten(array) aus wav datei auslesen
    dtype = data.dtype.name
    hoch = 0                                # zum beispiel bei float32
    if(dtype == "int8"):
        hoch = 7
        br = 8
    elif(dtype == "int16"):
        hoch = 15
    elif(dtype == "int32"):
        hoch = 31
        br = 32
    data = data / 2**hoch                   # auf Werte zwischen -1 und 1 begrenzen
    inputData = np.copy(data)
    plt.figure(1)
    if (data.ndim == 1):
        plt.plot(inputData)
        plt.grid(True)
        plt.ylim(-1,1)
        plt.xlim(0)
    elif (data.ndim == 2):
        dataSwap = np.swapaxes(inputData, 0, 1)
        plt.subplot(211)
        plt.plot(dataSwap[0])
        plt.grid(True)
        plt.ylim(-1,1)
        plt.xlim(0)
        plt.subplot(212)
        plt.plot(dataSwap[1])
        plt.grid(True)
        plt.ylim(-1,1)
        plt.xlim(0)
    plt.savefig("1_I.png")

    return rate, data

def writeFile(name, data, rate, bitrate):
    global br
    global inputData
    if bitrate not in [8, 16, 32]:
        print("Bitrate not allowed! Please use: 8, 16 or 32.")
        return
    plt.figure(2)
    if (data.ndim == 1):
        plt.plot(data)
        plt.grid(True)
        plt.ylim(-1,1)
        plt.xlim(0)
    elif (data.ndim == 2):
        dataSwap = np.swapaxes(data, 0, 1)
        plt.subplot(211)
        plt.plot(dataSwap[0])
        plt.grid(True)
        plt.ylim(-1,1)
        plt.xlim(0)
        plt.subplot(212)
        plt.plot(dataSwap[1])
        plt.grid(True)
        plt.ylim(-1,1)
        plt.xlim(0)
    plt.savefig("2_O.png")
    plt.figure(3)
    diffData = np.subtract(inputData, data)
    if (data.ndim == 1):
        plt.plot(diffData, color='red')
        plt.grid(True)
        plt.ylim(-1,1)
        plt.xlim(0)
    elif (data.ndim == 2):
        dataSwap2diff = np.swapaxes(diffData, 0, 1)
        plt.subplot(211)
        plt.plot(dataSwap2diff[0], color='red')
        plt.grid(True)
        plt.ylim(-1,1)
        plt.xlim(0)
        plt.subplot(212)
        plt.plot(dataSwap2diff[1], color='red')
        plt.grid(True)
        plt.ylim(-1,1)
        plt.xlim(0)
    plt.savefig("3_Diff.png")
    plt.figure(1)
    diffData = inputData - data
    if (data.ndim == 1):
        plt.plot(data, color='cyan')
    elif (data.ndim == 2):
        plt.subplot(211)
        plt.plot(dataSwap[0], color='cyan')
        plt.subplot(212)
        plt.plot(dataSwap[1], color='cyan')
    plt.savefig("4_Both.png")
    data = data * 2.**(bitrate - 1)         # Umwandlung in int8, 16 oder 32
    if(bitrate == 8):
        data = data.astype(np.int8)
    elif(bitrate == 16):
        data = data.astype(np.int16)
    elif(bitrate == 32):
        data = data.astype(np.int32)
    wave.write(name, rate, data)            # ausgabe des neuen wav files


def fftPlot(rate, data):
    N = data.size
    if (data.ndim == 1):
        N2 = N
    elif (data.ndim == 2):
        N2 = N / 2
    T = 1 / rate
    xf = fftfreq(int(N2), T)
    #xf = np.linspace(0., 0.5 * rate, N2 / 2)
    dataf = fft(data)
    plt.figure(4)
    #plt.xscale('log')
    plt.plot(xf[:N//2], 2.0 / N * np.abs(dataf[:N2 // 2]))
    plt.xlim(10,20000)
    plt.grid(True)
    plt.show()

def getSineWave(rate, length, freq):
    T = 1 / rate
    x = np.linspace(0., length * T, length)
    data = np.sin(freq * 2. * np.pi * x)
    return data

def getAvWindow(ms, rate):
    return int(rate / 1000 * ms)
    #return ms

def useAlgorithmOn(rate, data, goal):
    if (data.ndim > 2): #GEHT NUR FÜR MONO UND STEREO!!!
        print ("only mono or stereo files pls")
        return data
    lowcut = Filter()
    highshelf = Filter()
    cutoff = 38.
    cutoff_hs = 1681.
    lowcut.set_coefficients("lowcut", cutoff, rate, (1. / 2.))
    highshelf.set_coefficients_shelf("highshelf", cutoff_hs, rate, 4.)
    avCo = getTimeConstant(400, rate)
    avWindow = getAvWindow(100, rate)
    #print(avWindow)
    rms = [0., 0.]
    rmsArray = [[], []]
    gatedRmsArray = [[], []]
    gate = 10**(-70/20)
    gainArray = [[], []]
    setGain = False
    if (data.ndim == 1):
        dataC = np.array([data, []])
        rmsArray = [[], np.zeros(int(data.size / avWindow))]
    else:
        dataC = np.swapaxes(data, 0, 1)
    s = [0., 0.]
    for dim in range(dataC.ndim):
        print('...calculating channel ' + repr(dim + 1))
        i = 0
        for sample in dataC[dim]:
            s[dim] = lowcut.process(sample)
            s[dim] = highshelf.process(s[dim])
            rms[dim] = getRMS(avCo, rms[dim], s[dim])
            if (i + 1) % avWindow == 0:
                rmsArray[dim] = np.append(rmsArray[dim], [rms[dim]])
                if rms[dim] > gate:
                    setGain = True
                    gain = 10**((goal - 10 * np.log10(rms[dim]))/20)
                    gainArray[dim] = np.append(gainArray[dim], [gain])
                else:
                    setGain = False
                    gainArray[dim] = np.append(gainArray[dim], [0])
            if setGain:
                dataC[dim][i] = gain * sample
            i += 1
    print('...calculating loudness')
    rmsArray = np.delete(rmsArray, np.s_[:3], 1) # erste 3 Werte löschen
    rmsArray = np.add(rmsArray[0], rmsArray[1])
    rmsArray = 10 * np.log10(rmsArray) # zu dB
    rmsArray = np.subtract(rmsArray, 0.691) # und - 0.691
    rmsArray = rmsArray[rmsArray > -70] # gate > -70
    rmsArray = np.add(rmsArray, 0.691) # und + 0.691
    rmsArray = 10**(rmsArray/10) # zurück
    l1 = 10 * np.log10(np.sum(rmsArray) / rmsArray.size) - 0.691
    cut2 = l1 - 10.
    rmsArray = 10 * np.log10(rmsArray) # zu dB
    rmsArray = np.subtract(rmsArray, 0.691) # und - 0.691
    rmsArray = rmsArray[rmsArray > cut2] # gate > -cut2
    rmsArray = np.add(rmsArray, 0.691) # und + 0.691
    rmsArray = 10**(rmsArray/10) # zurück
    l2 = 10 * np.log10(np.sum(rmsArray) / rmsArray.size) - 0.691
    #print ('l1 = ' + repr(l1))
    #print ('cut2 = ' + repr(cut2))
    print ('l2 = ' + repr(l2))
    return np.swapaxes(dataC, 0, 1)

def getRMS(avCo, rms, sample):
    return (1 - avCo) * rms + avCo * sample**2

def getLvl(data, unit="lin"):
    sum = 0
    for sample in data:
        sum += sample*sample
    lvl2 = sum/data.size
    if unit == "dB":
        if lvl2 == 0:
            return -100.
        else:
            return 10 * np.log10(lvl2)
    else:
        return np.sqrt(lvl2)

def getAmp(data, unit="lin"):
    half = int(data.size / 2)
    amp = max(data[half:])
    if unit == "dB":
        if amp == 0:
            return -100.
        else:
            return 20 * np.log10(amp)
    else:
        return amp

def getTimeConstant(ms, rate):              # errechnet Zeit-Konstante für ms abhängig von Sample Abstand
    if(ms > 0):
        return 1 - np.exp(-2.2*(1/rate)/(ms/1000))
    else:
        return 1

def filterTest():
    lowcut = Filter()
    highshelf = Filter()
    cutoff = 38.
    cutoff_hs = 1681.
    r = 48000
    lowcut.set_coefficients("lowcut", cutoff, r, (1. / 2.))
    highshelf.set_coefficients_shelf("highshelf", cutoff_hs, r, 4.)
    #lowcut.set_coefficients_d(1., -2., 1., -1.99004745483398, 0.99007225036621)
    lowcut.print_coefficients()
    x = 1000.
    x2 = 5000.
    x3 = 20000.
    start = 0.
    step = 10.
    step2 = 100.
    step3 = 1000.
    i = start
    i2 = x
    i3 = x2
    lvls1 = np.arange(x/step)
    lvls2 = np.arange(x,x2,step2)
    lvls3 = np.arange(x2,x3,step3)
    count1 = np.arange(x/step) * step
    count2 = np.arange(x,x2,step2)
    count3 = np.arange(x2,x3,step3)
    while i < x:
        lowcut.reset_timeBuffer()
        data = getSineWave(r,r,i)
        j = 0
        for sample in data:
            data[j] = lowcut.process(sample)
            data[j] = highshelf.process(data[j])
            j += 1
        lvl = getAmp(data, "dB")
        fl = int(i/step)
        lvls1[fl] = lvl
        i += step
    while i2 < x2:
        lowcut.reset_timeBuffer()
        data = getSineWave(r,r,i2)
        j = 0
        for sample in data:
            data[j] = lowcut.process(sample)
            data[j] = highshelf.process(data[j])
            j += 1
        lvl = getAmp(data, "dB")
        fl = int((i2 - x)/step2)
        lvls2[fl] = lvl
        i2 += step2
    while i3 < x3:
        lowcut.reset_timeBuffer()
        data = getSineWave(r,r,i2)
        j = 0
        for sample in data:
            #gitdata[j] = lowcut.process(sample)
            data[j] = highshelf.process(data[j])
            j += 1
        lvl = getAmp(data, "dB")
        fl = int((i3 - x2)/step3)
        lvls3[fl] = lvl
        i3 += step3
    plt.xscale('log')
    count = np.append(np.append(count1, count2), count3)
    lvls = np.append(np.append(lvls1, lvls2), lvls3)
    plt.plot(count, lvls)
    plt.axvline(x=cutoff, color='red')
    plt.axvline(x=cutoff_hs, color='yellow')
    plt.ylim(-30, 5)
    plt.xlim(0., count.max())
    plt.grid(True)
    plt.show()

#filterTest()

#r,d = readFile("OHTest.wav")
#r,d = readFile("testFiles/1770-2_Comp_23LKFS_2000Hz_2ch.wav")
#r,d = readFile("testFiles/1770-2_Comp_18LKFS_FrequencySweep.wav")
#r,d = readFile("testFiles/1770-2_Comp_AbsGateTest.wav")
#r,d = readFile("testFiles/1770-2_Comp_RelGateTest.wav")
#r,d = readFile("testFiles/1770-2_Conf_Stereo_VinL+R-24LKFS.wav")
#r,d = readFile("testFiles/1770-2_Conf_Mono_Voice+Music-23LKFS.wav")
#d = useAlgorithmOn(r, d, -20)
#fftPlot(r, d)
#whiteNoise = np.random.uniform(-1, 1, 50*44100)
#fftPlot(44100, whiteNoise)
#i = 0
#for sample in whiteNoise:
#    whiteNoise[i] = lowcut.process(sample)
#    i += 1
#fftPlot(44100, whiteNoise)
#newData = useAlgorithmOn(d)
#writeFile("Output.wav",d,r,br)


#lowcut = Filter()
#highshelf = Filter()
#cutoff = 38.
#cutoff_hs = 1681.
#lowcut.set_coefficients("lowcut", cutoff, 44100, (1. / 2.))
#highshelf.set_coefficients_shelf("highshelf", cutoff_hs, 44100, 4.)

