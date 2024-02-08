import math
from scipy import signal
import numpy as np
import matplotlib.pyplot as plt
from PIL import Image


class StateVariableFilter:
    def __init__(self, cutoff, resonance, fe):
        self.fc1 = cutoff
        #self.fc2 = cutoff; # to implement a bandpass filter
        self.Q = resonance
        self.alpha1 = 2.0 * math.sin(math.pi * self.fc1 / fe)
        self.alpha2 = 1.0 / self.Q
        self.yb1 = 0.0
        self.yl1 = 0.0
        self.yl_output = 0.0
        self.yb_output = 0.0
        self.yh_output = 0.0

    def process(self, x):
        y_h = x - self.yl1 - self.alpha2 * self.yb1
        y_b = self.alpha1 * y_h + self.yb1
        y_l = self.yl1 + self.alpha1 * y_b

        self.yl_output = y_l
        self.yb_output = y_b
        self.yh_output = y_h

        # update state variables
        self.yb1 = y_b
        self.yl1 = y_l



"""
F1 = alpha1
Q1 = alpha2
usability limit -> F1 < 2 - Q1
luego alpha1 < 2 - (1/Q)
"""

for i in range(1,15000,1):
    resonance = i/100
    filter = StateVariableFilter(cutoff=5000, resonance=i, fe=44100)
    r = filter.alpha1
    q = 1 - filter.alpha1*filter.alpha2
    num = [r**2]
    den = [1, -(r**2 - q - 1), q]
    z, p, k = signal.tf2zpk(num, den)
    #if p.real[0] >= -0.5 or p.real[1] > -0.5:
     #   print("For Resonance: ", resonance, "\n", "Poles: ", p.real)
     #   break
    if filter.alpha1 < 2 - (1/filter.Q):
        print("For Resonance: ", resonance, "\n", "Poles: ", p.real)
        break

#img = Image.open('C:\\Users\\micha\\Desktop\\pole-zero.png')

"""
filter = StateVariableFilter(cutoff=5000, resonance=1.5, fe=44100)

r = filter.alpha1
q = 1 - filter.alpha1*filter.alpha2
num = [r**2]
den = [1, -(r**2 - q - 1), q]
z, p, k = signal.tf2zpk(num, den)
fig, ax = plt.subplots()
ax.scatter(np.real(p), np.imag(p), marker='x', label='Poles')
ax.scatter(np.real(z), np.imag(z), marker='o', label='Zeros')
ax.legend()
ax.set_title('Pole-zero plot')
ax.set_xlabel('Real')
ax.set_ylabel('Imaginary')
ax.grid()
plt.show()

"""
