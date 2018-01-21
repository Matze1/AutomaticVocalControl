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
    
    def print_coefficients(self):
        print ('b0 = ' + repr(self.b0))
        print ('b1 = ' + repr(self.b1))
        print ('b2 = ' + repr(self.b2))
        print ('a1 = ' + repr(self.a1))
        print ('a2 = ' + repr(self.a2))
    
    def reset_timeBuffer(self):
        self.z_1 = 0.
        self.z_2 = 0.
    
    def process(self, sample):
        mid = sample - self.a1 * self.z_1 - self.a2 * self.z_2
        out = self.b0 * mid + self.b1 * self.z_1 + self.b2 * self.z_2

        self.z_2 = self.z_1
        self.z_1 = mid

        return out

    def set_coefficients_d(self, b_0, b_1, b_2, a_1, a_2):
        self.b0 = b_0
        self.b1 = b_1
        self.b2 = b_2
        self.a1 = a_1
        self.a2 = a_2

    def set_coefficients(self, type, cut_off, s_rate):
        k = np.tan(np.pi * cut_off / s_rate)
        if type == "highpass" or type == "lowcut":
            self.b0 = 1. / (k + 1.)
            self.b1 = -self.b0
        if type == "highcut" or type == "lowpass":
            self.b0 = k / (k + 1.)
            self.b1 = self.b0
        if type == "allpass":
            self.b0 = (k - 1.) / (k + 1.)
            self.b1 = 1.
        self.a1 = (k - 1.) / (k + 1.)
        self.b2 = 0.
        self.a2 = 0.
    
    def set_coefficients(self, type, cut_off, s_rate, q):
        k = np.tan(np.pi * cut_off / s_rate)
        kk = k * k
        x = kk * q
        y = k + q
        z = x + y
        zm = x - k + q
        zb = 2 * q * (kk - 1) / z
        if type == "highpass" or type == "lowcut":
            self.b0 = q / z
            self.b1 = -2. * self.b0
            self.b2 = self.b0
        if type == "highcut" or type == "lowpass":
            self.b0 = x / z
            self.b1 = 2. * self.b0
            self.b2 = self.b0
        if type == "bandpass":
            self.b0 = k / z
            self.b1 = 0.
            self.b2 = -self.b0
        if type == "bandreject":
            self.b0 = q * (1 + kk) / z
            self.b1 = zb
            self.b2 = self.b0
        if type == "allpass":
            self.b0 = zm / z
            self.b1 = zb
            self.b2 = 1.
        self.a1 = zb
        self.a2 = zm / z

    def set_coefficients_shelf(self, type, cut_off, s_rate, gain):
        k = np.tan(np.pi * cut_off / s_rate)
        v0 = 10**(gain/20.)
        kk = k * k
        w2k = np.sqrt(2.) * k
        w2v0k = np.sqrt(2. * v0) * k
        if gain == 0.:
            self.reset_coefficients()
        if type == "lowshelf":
            if gain > 0.:
                denom = 1. +  w2k + kk
                v0kk = v0 * kk
                self.b0 = (1. + w2v0k + v0kk) / denom
                self.b1 = 2. * (v0kk - 1.) / denom
                self.b2 = (1. - w2v0k + v0kk) / denom
                self.a1 = 2. * (kk - 1.) / denom
                self.a2 = (1. - w2k + kk) / denom
            else:
                denom = v0 + w2v0k + kk
                self.b0 = v0 * (1. + w2k + kk) / denom
                self.b1 = 2. * v0 * (kk - 1.) / denom
                self.b2 = v0 * (1. - w2k + kk) / denom
                self.a1 = 2. * (kk - v0) / denom
                self.a2 = (v0 - w2v0k + kk) / denom
        if type == "highshelf":
            if gain > 0.:
                denom = 1. +  w2k + kk
                self.b0 = (v0 + w2v0k + kk) / denom
                self.b1 = 2. * (kk - v0) / denom
                self.b2 = (v0 - w2v0k + kk) / denom
                self.a1 = 2. * (kk - 1.) / denom
                self.a2 = (1. - w2k + kk) / denom
            else:
                denom = 1. + w2v0k + v0 * kk
                self.b0 = v0 * (1. + w2k + kk) / denom
                self.b1 = 2. * v0 * (kk - 1.) / denom
                self.b2 = v0 * (1. - w2k + kk) / denom
                self.a1 = 2 * (v0 * kk - 1.) / denom
                self.a2 = (1. - w2v0k + v0 * kk) / denom
        #print ("wrong type! please use -highshelf- or -lowshelf-.")

