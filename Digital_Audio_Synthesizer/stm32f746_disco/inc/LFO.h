#ifndef LFO_H_
#define LFO_H_


#include "oscillator.h"
#include "IIR.h"
#include <math.h>

//#include "adsr.h"

#define N 1024
#define FE 44100.0



typedef struct {
    float freq;
    float amp;
    float sample_rate;
    float output;
    float time;
    float phase;
    int on;
} LFO;

void LFO_init(LFO *lfo, float freq, float amp, float sample_rate);
void LFO_process(LFO *lfo, state_variable_filter_t *f);
void set_LFO_amplitude(LFO *lfo, float vel);
void set_frequency(LFO *lfo, float vel);
void stop_LFO(LFO *lfo);


#endif /* LFO_H_ */
