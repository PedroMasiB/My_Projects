import ctypes
import numpy as np
import matplotlib.pyplot as plt


# Load the shared library
lib = ctypes.CDLL('./oscillator.so')

# Define the Oscillator struct in Python using ctypes
class Oscillator(ctypes.Structure):
    _fields_ = [("amplitude", ctypes.c_double),
                ("frequency", ctypes.c_double),
                ("sample_rate", ctypes.c_double),
                ("phase", ctypes.c_double)]

def get_sine_wave(oscillator, num_samples):
    # Define the argument and return types of the C function
    lib.sine_wave.argtypes = [ctypes.POINTER(Oscillator)]
    lib.sine_wave.restype = ctypes.c_double

    # Allocate an array to store the output samples
    samples = (ctypes.c_double * num_samples)()

    # Call the C function to generate the waveform
    for i in range(num_samples):
        samples[i] = lib.sine_wave(ctypes.byref(oscillator))

    return samples

# Define parameters for the waveform
amplitude = 1.0
frequency = 440.0
sample_rate = 44100
duration = 1.0
num_samples = int(sample_rate * duration)

# Create an Oscillator object
osc = Oscillator(amplitude, frequency, sample_rate, 0.0)

# Generate the waveform
waveform = get_sine_wave(osc, num_samples)

# Generate the time axis
time = np.linspace(0, duration, num_samples)

# Plot the waveform
plt.plot(time, waveform)
plt.xlabel('Time (seconds)')
plt.ylabel('Amplitude')
plt.show()
