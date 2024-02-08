#include "IIR_filter.h"
#pragma GCC optimize ("O0")



//=====================================================================
void arm_iir_init_f32( arm_iir_instance_f32 * S,  uint16_t numStages,  float32_t * pCoeffs,  float32_t * pState,  uint32_t blockSize)
{
	  S->numStages = numStages;
	  S->coef = pCoeffs;
	  //memset(pState, 0, (numStages + (blockSize - 1U)) * sizeof(float32_t)); // !!!!!!!!!!
	  S->pState = pState;
}
//=====================================================================
void IIR_calc_coeff_f32(arm_iir_instance_f32 *S, uint16_t numStages,float32_t fc1, float32_t fc2, float32_t fe, int filter_type )
{

}
//=====================================================================
int arm_iir_f32(arm_iir_instance_f32 *S, float* x, float* y)
{

    return 0;
}
//=====================================================================




