import numpy as np

class Filter:
    b0 = 1.
    b1 = 0.
    b2 = 0.
    a1 = 0.
    a2 = 0.
    z_1 = 0.
    z_2 = 0.
    
    def reset_coefficients(self):
        self.b0 = 1.
        self.b1 = 0.
        self.b2 = 0.
        self.a1 = 0.
        self.a2 = 0.
        self.z_1 = 0.
        self.z_2 = 0.
    
    def reset_timeBuffer(self):
        self.z_1 = 0.
        self.z_2 = 0.
    
    def process(self, sample):
        mid = sample - self.a1 * self.z_1 - self.a2 * self.z_2
        out = self.b0 * mid + self.b1 * self.z_1 + self.b2 * self.z_2

        self.z_2 = self.z_1
        self.z_1 = mid

        return out

    def set_coefficients_hp(self, k):
        self.b0 = 1 / (k + 1)
        self.b1 = -self.b0
        self.a1 = (k - 1) / (k + 1)
        self.b2 = 0
        self.a2 = 0

    def set_coefficients(self, type, cut_off, s_rate):
        k = np.tan(np.pi * cut_off / s_rate)
        if type == "highpass" or type == "lowcut":
            self.set_coefficients_hp(k)
