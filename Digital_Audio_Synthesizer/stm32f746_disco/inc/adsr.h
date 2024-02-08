#include <math.h>


/*
#define ATTACK_DURATION  1000.0
#define RELEASE_DURATION 1000.0
enum state_t  { INIT, NOTE_OFF, ATTACK, DECAY, SUSTAIN, RELEASE };

//int adsr(struct adsr_TypeStruct* );
*/

typedef struct
{
    float attack_time;
    float decay_time;
    float sustain_level;
    float release_time;

    float value;
    float time;
    int state;
    float rate;


    int note_on;
    int note_off;

    float attack_slope;
    float decay_slope;
    float release_slope;

    float attack_rate;
    float decay_rate;
    float release_rate;

    float ADS_time;

    int mode; // lineal o exponencial

    float max_value;

    float note;
} ADSR;



void adsr_init(ADSR *envelope, float attack_time, float decay_time, float sustain_level, float release_time, float SAMPLE_RATE, int mode);
void calculate_envelope_value(ADSR *envelope);
void adsr_set_attack_time(ADSR *envelope, float vel);
void adsr_set_decay_time(ADSR *envelope, float vel);
void adsr_set_sustain_level(ADSR *envelope, float vel);
void adsr_set_release_time(ADSR *envelope, float vel);
void note_on(ADSR *envelope, float note);
void note_off(ADSR *envelope, float note);
void adsr_reset(ADSR *envelope);
void adsr_set_parametres(ADSR *envelope, int option, float vel);
void calculate_value(ADSR *envelope);
void update_state(ADSR *envelope);
void set_rate(ADSR *envelope);






