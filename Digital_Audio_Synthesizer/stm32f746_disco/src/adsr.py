import math

class ADSR:
    def __init__(self, attack_time, decay_time, sustain_level, release_time):
        self.attack_time = attack_time
        self.decay_time = decay_time
        self.sustain_level = sustain_level
        self.release_time = release_time
        self.value = 0.0

    def calculate_envelope_value(self, time):
        attack_slope = 1.0 / self.attack_time
        decay_slope = (1.0 - self.sustain_level) / self.decay_time
        release_slope = -self.sustain_level / self.release_time

        if time < self.attack_time:
            self.value = time * attack_slope
        elif time < self.attack_time + self.decay_time:
            decay_time = time - self.attack_time
            self.value = 1.0 - decay_time * decay_slope
        elif time < self.attack_time + self.decay_time + self.release_time:
            release_time = time - self.attack_time - self.decay_time
            self.value = self.sustain_level + release_time * release_slope
        else:
            self.value = self.sustain_level

        self.value = max(0.0, min(1.0, self.value))

# Example usage:
envelope = ADSR(attack_time=0.1, decay_time=0.2, sustain_level=0.5, release_time=0.3)
envelope.calculate_envelope_value(0.05)
print(envelope.value)