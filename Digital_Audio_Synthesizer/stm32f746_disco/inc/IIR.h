#ifndef IIR_H_
#define IIR_H_

#include "oscillator.h"
//#include "LFO.h"
#include <math.h>

#define FE 44100.0
#define N 1024



typedef struct {
  float fc1; // Filter cutoff frequency
  //float fc2;
  float Q; // Filter resonance
  float alpha1, alpha2; // Filter coefficients
  float min_Q; // to update the minimun value of Q everytime we change the fc1
  float yb1, yl1; // Filter state variables
  float yl_output, yb_output, yh_output; // Filter outputs
  float alpha1_phase;
} state_variable_filter_t;

void SVF_init(state_variable_filter_t *f, float cutoff, float resonance, float fe);
void SVF_process(state_variable_filter_t *f, float x);
void set_cutoff(state_variable_filter_t *f, float vel);
void set_Q(state_variable_filter_t *f, float vel);
//void set_fc1(state_variable_filter *f, LFO *lfo, ADSR *envelope);

//void set_cutoff_LFO(state_variable_filter_t *f, float vel);





#endif /* IIR_H_ */


