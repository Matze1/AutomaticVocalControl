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

def useAlgorithmOn(data):
    gain = 0.8
    i = 0
    for sample in data:
        data[i] = sample * gain
        i += 1
    return data

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

#rmsSquare = (1 - avCo) * rmsSquare + avCo * sample**2  # aktueller rms Wert wird berechnet
#ampSquare = rmsSquare

def getTimeConstant(ms, rate):              # errechnet Zeit-Konstante für ms abhängig von Sample Abstand
    if(ms > 0):
        return 1 - np.exp(-2.2*(1/rate)/(ms/1000))
    else:
        return 1

#r,d = readFile("OHTest.wav")
#r,d = readFile("cello.wav")
#fftPlot(r, d)
#whiteNoise = np.random.uniform(-1, 1, 50*44100)
#fftPlot(44100, whiteNoise)
lowcut = Filter()
cutoff = 150
lowcut.set_coefficients("lowcut", cutoff, 44100)
#i = 0
#for sample in whiteNoise:
#    whiteNoise[i] = lowcut.process(sample)
#    i += 1
x = 2000.
r = 44100
start = 0
i = start;
step = 10
lvls = np.arange(x/step)
count = np.arange(x/step) * step
while i < x:
    lowcut.reset_timeBuffer()
    data = getSineWave(r,r,i)
    j = 0
    for sample in data:
        data[j] = lowcut.process(sample)
        j += 1
    lvl = getLvl(data, "dB")
    fl = int(i/step)
    lvls[fl] = lvl
    i += step
plt.xscale('log')
plt.plot(count, lvls)
plt.axvline(x=cutoff, color='red')
plt.ylim(-60, 0)
plt.xlim(0., count.max())
plt.grid(True)
plt.show()
#fftPlot(44100, whiteNoise)
#newData = useAlgorithmOn(d)
#writeFile("Output.wav",newData,r,br)
