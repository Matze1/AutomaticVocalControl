import scipy.io.wavfile as wave
import numpy as np
import matplotlib.pyplot as plt

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

def useAlgorithmOn(data):
    gain = 0.8
    i = 0
    for sample in data:
        data[i] = sample * gain
        i += 1
    return data

#rmsSquare = (1 - avCo) * rmsSquare + avCo * sample**2  # aktueller rms Wert wird berechnet
#ampSquare = rmsSquare

def getTimeConstant(ms, rate):              # errechnet Zeit-Konstante für ms abhängig von Sample Abstand
    if(ms > 0):
        return 1 - np.exp(-2.2*(1/rate)/(ms/1000))
    else:
        return 1

r,d = readFile("OHTest.wav")
#r,d = readFile("cello.wav")
newData = useAlgorithmOn(d)
writeFile("Output.wav",newData,r,br)
