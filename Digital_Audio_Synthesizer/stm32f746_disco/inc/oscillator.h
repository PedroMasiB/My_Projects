#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include <stdlib.h>

typedef struct {
  float amplitude;
  float frequency; // f0
  float phase;
  float sample_rate; // fs
  float sample;

  int mode;
} Oscillator;

void OSC_init(Oscillator *osc, float amplitude, float frequency, float phase, float sample_rate, float sample, int mode);
void set_amplitude(Oscillator *osc, float amplitude);
int sine_wave(Oscillator *osc);
void sawtooth_wave(Oscillator *osc);
void square_wave(Oscillator *osc);
void set_mode(Oscillator *osc);



#endif /* OSCILLATOR_H */





