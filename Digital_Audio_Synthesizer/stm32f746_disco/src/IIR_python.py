import numpy as np
import matplotlib.pyplot as plt

from scipy import signal

fc = 5000
fs = 44100
Q = 2

alpha1 = 2*np.sin(np.pi*fc/fs)
alpha2 = 1/Q

b = [alpha1**2]
print(b)
a = [1, (alpha1**2+alpha1*alpha2-2), (1-alpha1*alpha2)]
print(a)
f, h = signal.freqz(b, a, fs=fs)
plt.semilogy(f, abs(h))
plt.axvline(fc,c="r")
plt.grid()
plt.xlabel("Frequency [Hz]")
plt.ylabel("Modulus")
plt.xlim([0, fs/2])
plt.show()