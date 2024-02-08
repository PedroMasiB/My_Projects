#include "adsr.h"
#include <stdint.h>
#include <math.h>
#pragma GCC optimize ("O0")


void adsr_init(ADSR *envelope, float attack_time, float decay_time, float sustain_level, float release_time, float SAMPLE_RATE, int mode)
{
	envelope->attack_time = attack_time;
	envelope->decay_time = decay_time;
	envelope->sustain_level = sustain_level;
	envelope->release_time = release_time;
	envelope->mode = 0;

	// Time rate
	envelope->rate = 1 / SAMPLE_RATE;

	// Time init
	envelope->time = 0.0;

	envelope->max_value = 1.0;

	envelope->state = 0;
	envelope->note_on = 0;
	envelope->note_off = 0;


	//LINEAL MODE
	if (envelope->mode==0){
		// Slopes Lineal calculations
		envelope->attack_slope = envelope->max_value / envelope->attack_time;
		envelope->decay_slope = (envelope->sustain_level - envelope->max_value) / envelope->decay_time;
		envelope->release_slope = -envelope->sustain_level / envelope->release_time;

		// Rates Lineal calculations
		envelope->attack_rate = envelope->attack_slope * envelope->rate;
		envelope->decay_rate = envelope->decay_slope * envelope->rate;
		envelope->release_rate = envelope->release_slope * envelope->rate;

		/* Para no usar los slopes
		envelope->attack_rate = (envelope->max_value / envelope->attack_time) * envelope->rate;
		envelope->decay_rate = ((envelope->sustain_level - envelope->max_value) / envelope->decay_time) * envelope->rate;
		envelope->release_rate = (-envelope->sustain_level / envelope->release_time) * envelope->rate;
		*/

	}

	// LOGARITHMIC MODE
	else if (envelope->mode==1){
		// Slopes Logarithmic calculations
		envelope->attack_slope = log(envelope->max_value) / (envelope->attack_time * (1/envelope->rate));
		envelope->decay_slope = log(envelope->sustain_level / envelope->max_value) / (envelope->decay_time * (1/envelope->rate));
		envelope->release_slope = log(envelope->sustain_level) / (envelope->release_time * (1/envelope->rate));

		// Rates Logarithmic calculations
		envelope->attack_rate = exp(envelope->attack_slope);
		envelope->decay_rate = exp(envelope->decay_slope);
		envelope->release_rate = exp(envelope->release_slope);
	}
}


void calculate_envelope_value(ADSR *envelope){
    switch(envelope->state){
    	case 0:
			envelope->value = 0.0;
			envelope->ADS_time = 0.0;
			envelope->time = 0.0;
			break;
    	case 1:
    		envelope->value += envelope->attack_rate;
    		envelope->time += envelope->rate;
    		envelope->ADS_time = envelope->time;
    		break;
    	case 2:
    		envelope->value += envelope->decay_rate;
    		envelope->time += envelope->rate;
    		envelope->ADS_time = envelope->time;
    		break;
    	case 3:
    		envelope->value = envelope->sustain_level;
    		envelope->time += envelope->rate;
    		envelope->ADS_time = envelope->time;
    		break;
    	case 4:
    		envelope->value += envelope->release_rate;
    		envelope->time += envelope->rate;
    		if(envelope->time >= envelope->ADS_time + envelope->release_time){
    			envelope->note_off = 0;
    			envelope->state = 0;
    		}
    		break;

    }
    envelope->value = fmax(0.0, fmin(1.0, envelope->value));
}



/*
we can write a function like this one to set the parameters of the ADSR, instead of writing 4 functions

void adsr_set_parametres(ADSR *envelope, int option, float vel){
    switch(option){
        case 1:
            adsr_set_attack_time(envelope, vel);
            break;
        case 2:
            adsr_set_decay_time(envelope, vel);
            break;
        case 3:
            adsr_set_sustain_level(envelope, vel);
            break;
        case 4:
            adsr_set_release_time(envelope, vel);
            break;
    }
}

*/


/* Set parametres functions*/
void adsr_set_attack_time(ADSR *envelope, float vel)
{
	float min_value = 0.005;
	float max_value = 15.0;
	float log_min_value = log10(min_value);
	float log_max_value = log10(max_value);
	float LogSliderValue = (vel / 127.0) * (log_max_value - log_min_value) + log_min_value;
    envelope->attack_time = (float) pow(10, LogSliderValue);
    /*
    if(envelope->mode == 0){
    	envelope->attack_slope = envelope->max_value / envelope->attack_time;
    	envelope->attack_rate = envelope->attack_slope * envelope->rate;
    }
    else{
    	envelope->attack_slope = log(envelope->max_value) / (envelope->attack_time * (1/envelope->rate));
    	envelope->attack_rate = exp(envelope->attack_slope);
    }
    */


}

void adsr_set_decay_time(ADSR *envelope, float vel)
{
	float min_value = 0.200;
	float max_value = 0.500;
    float rate = log(max_value / min_value) / 127.0;
    envelope->decay_time = (float) (min_value * exp(rate * vel));
    /*
    if(envelope->mode == 0){
    	envelope->decay_slope = (envelope->sustain_level - envelope->max_value) / envelope->decay_time;
    	envelope->decay_rate = envelope->decay_slope * envelope->rate;
    }
    else{
    	envelope->decay_slope = log(envelope->sustain_level / envelope->max_value) / (envelope->decay_time * (1/envelope->rate));
    	envelope->decay_rate = exp(envelope->decay_slope);
    }
    */

}

void adsr_set_sustain_level(ADSR *envelope, float vel)
{
    float min_value = 0.001;
    float max_value = 1.0;
    float rate = log(max_value / min_value) / 127.0;
    envelope->sustain_level = (float) (min_value * exp(rate * vel));
}

void adsr_set_release_time(ADSR *envelope, float vel)
{
	float min_value = 0.050;
	float max_value = 2.0;
	float log_min_value = log10(min_value);
	float log_max_value = log10(max_value);
	float LogSliderValue = (vel / 127.0) * (log_max_value - log_min_value) + log_min_value;
    envelope->release_time = (float) pow(10, LogSliderValue);
    /*
    if(envelope->mode == 0){
    	envelope->release_slope = -envelope->sustain_level / envelope->release_time;
    	envelope->release_rate = envelope->release_slope * envelope->rate;
    }
    else{
    	envelope->release_slope = log(envelope->sustain_level) / (envelope->release_time * (1/envelope->rate));
    	envelope->release_rate = exp(envelope->release_slope);
    }
    */

}


/* Note on/off functions */
void note_on(ADSR *envelope, float note)
{
    envelope->note_on = 1;
    envelope->note_off = 0;
    envelope->time = 0.0;
    envelope->ADS_time = 0.0;
    envelope->note = note;

}

void note_off(ADSR *envelope, float note)
{
	if(envelope->note == note){
		envelope->note_on = 0;
		envelope->note_off = 1;
	}

}


//  Reset function
/*
void adsr_reset(ADSR *envelope)
{
    envelope->note_on = 0;
    envelope->note_off = 0;
    envelope->time = 0.0;
    envelope->value = 0.0;
}
*/


void update_state(ADSR *envelope){
	if (envelope->time < envelope->attack_time && envelope->note_on){ // Attack State
		envelope->state = 1;
	}
	else if (envelope->time >= envelope->attack_time && envelope->time < envelope->attack_time + envelope->decay_time && envelope->note_on){ // Decay State
		envelope->state = 2;
	}
	else if (envelope->time >= envelope->attack_time + envelope->decay_time && envelope->note_on){ // Sustain state
		envelope->state = 3;
	}
	else if (envelope->note_off){ // Release state
		envelope->state = 4;
	}
	else{ // idle state
		envelope->state = 0;
	}
}

void set_rate(ADSR *envelope){

	//LINEAR MODE
	if (envelope->mode==0){
		// Slopes Lineal calculations
		envelope->attack_slope = envelope->max_value / envelope->attack_time;
		envelope->decay_slope = (  envelope->sustain_level - envelope->max_value) / envelope->decay_time;
		envelope->release_slope = -envelope->sustain_level / envelope->release_time;

		// Rates Lineal calculations
		envelope->attack_rate = envelope->attack_slope * envelope->rate;
		envelope->decay_rate = envelope->decay_slope * envelope->rate;
		envelope->release_rate = envelope->release_slope * envelope->rate;
	}

	// LOGARITHMIC MODE
	else if (envelope->mode==1){
		// Slopes Logarithmic calculations
		envelope->attack_slope = log(envelope->max_value) / (envelope->attack_time * (1/envelope->rate));
		envelope->decay_slope = log(envelope->sustain_level / envelope->max_value) / (envelope->decay_time * (1/envelope->rate));
		envelope->release_slope = log(envelope->sustain_level) / (envelope->release_time * (1/envelope->rate));

		// Rates Logarithmic calculations
		envelope->attack_rate = exp(envelope->attack_slope);
		envelope->decay_rate = exp(envelope->decay_slope);
		envelope->release_rate = exp(envelope->release_slope);
	}
}


