import scipy.io.wavfile as wave
import numpy as np
import matplotlib.pyplot as plt

class Helper:
    inputData = []

    def readFile(self, name):
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
        self.inputData = np.copy(data)
        plt.figure(1)
        if (data.ndim == 1):
            plt.plot(self.inputData)
            plt.grid(True)
            plt.ylim(-1,1)
            plt.xlim(0)
        elif (data.ndim == 2):
            dataSwap = np.swapaxes(self.inputData, 0, 1)
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

    def writeFile(self, name, data, rate, bitrate=16):
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
        diffData = np.subtract(self.inputData, data)
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
        diffData = self.inputData - data
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

    def getSineWave(self, rate, length, freq):
        T = 1 / rate
        x = np.linspace(0., length * T, length)
        data = np.sin(freq * 2. * np.pi * x)
        return data

    def getSamplesPerMs(self, ms, rate):
        return int(rate / 1000 * ms)
        #return ms

    def getRMS2(self, avCo, rms, sample):
        return (1 - avCo) * rms + avCo * sample**2

    def getLvl(self, data, unit="lin"):
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

    def getAmp(self, data, unit="lin"):
        half = int(data.size / 2)
        amp = max(data[half:])
        if unit == "dB":
            if amp == 0:
                return -100.
            else:
                return 20 * np.log10(amp)
        else:
            return amp

    def getTimeConstant(self, ms, rate):              # errechnet Zeit-Konstante für ms abhängig von Sample Abstand
        if(ms > 0):
            return 1 - np.exp(-2.2*(1/rate)/(ms/1000))
        else:
            return 1
