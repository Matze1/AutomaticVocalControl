import scipy.io.wavfile as wave
import scipy.optimize as optmz
import numpy as np

def addUp(x):
    return x[0] + x[1] - x[2]

x0 = np.array([12.0, 199.0, 0.0])

res = optmz.minimize(addUp, x0, bounds=((-1,70),(1,300),(-90,90)), options={'disp': True})

print(res.x)
