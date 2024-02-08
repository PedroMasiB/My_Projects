/*
 * FIR_filterc.c
 *
 *  Created on: 13 sept. 2016
 *      Author: kerhoas
 */

#include <FIR_filter.h>


/*  void arm_fir_init_f32(
  arm_fir_instance_f32 * S,
  uint16_t numTaps,
  float32_t * pCoeffs,
  float32_t * pState,
  uint32_t blockSize);
*/
//=======================================================================================

void FIR_calc_coeff_f32(arm_fir_instance_f32 *S, uint16_t numTaps,float32_t fc1, float32_t fc2, float32_t fe, int filter_type )
{


}

//=======================================================================================
void FIR_custom_init_f32( arm_fir_instance_f32 *S, uint16_t numTaps,float32_t fc1, float32_t fc2, float32_t fe,  float32_t *pCoeffs, float32_t *pState )
{

}

//=======================================================================================

void FIR_init_f32( arm_fir_instance_f32 *S, uint16_t numTaps, float32_t *pCoeffs, float32_t *pState )
{
  uint16_t i;

  S->numTaps = numTaps;
  S->pCoeffs = pCoeffs;
  S->pState = pState;
  for (i = 0; i < S->numTaps; i++)
  {
    S->pState[i] = 0;
  }
}

//=======================================================================================
void FIR_filt_f32(arm_fir_instance_f32 *S, float32_t *pSrc, float32_t *pDst) {

	uint16_t i;
	float32_t acc;

    acc = 0;										// Clear the accumulator/output before filtering
    S->pState[0] = *pSrc;					 // Place new input sample as first element in the filter state array
    //Direct form filter each sample using a sum of products
		for (i = 0; i < S->numTaps; i++)
    {
       acc += S->pState[i]*S->pCoeffs[i];
    }
	*pDst = acc;

	// Vieillissement des échantillons d'entrée
    for (i = S->numTaps-1; i > 0; i--)
    {
      S->pState[i] = S->pState[i-1];
    }
}

//=======================================================================================

void FIR_filt_f32_circular(arm_fir_instance_f32 *S, float32_t *pSrc, float32_t *pDst) {

	int16_t i;
	float32_t acc;
	static int16_t k=0;	// younger sample index

    acc = 0;										// Clear the accumulator/output before filtering
    S->pState[k] = *pSrc;					 // Place new input sample as first element in the filter state array
    //Direct form filter each sample using a sum of products
		for (i = 0; i < S->numTaps; i++)
    {
       acc += S->pState[k++]*S->pCoeffs[i];
       if (k >= S->numTaps) k = 0;

    }
	*pDst = acc;

	k--;
	if (k <0) k = S->numTaps-1;
}

//=======================================================================================

void FIR_init_q15(arm_fir_instance_q15 *S, uint16_t numTaps, q15_t * pCoeffs, q15_t * pState)
{
	  uint16_t i;
	  S->numTaps = numTaps;
	  S->pCoeffs = pCoeffs;
	  S->pState = pState;
	  for (i = 0; i < S->numTaps; i++)
	  {
	    S->pState[i] = 0;
	  }
}

//=======================================================================================

void FIR_filt_q15(arm_fir_instance_q15 *S, q15_t *pSrc, q15_t *pDst) {

	uint16_t i;
	int32_t acc;

    acc = 0;										// Clear the accumulator/output before filtering
    S->pState[0] = *pSrc;					 // Place new input sample as first element in the filter state array
    //Direct form filter each sample using a sum of products
		for (i = 0; i < S->numTaps; i++)
    {
       acc += (int32_t) S->pState[i]*S->pCoeffs[i];
    }
	*pDst = (int16_t) (acc >> 16);

	// Vieillissement des échantillons d'entrée
    for (i = S->numTaps-1; i > 0; i--)
    {
      S->pState[i] = S->pState[i-1];
    }
}

//=======================================================================================

void FIR_filt_q15_circular(arm_fir_instance_q15 *S, q15_t *pSrc, q15_t *pDst) {

	uint16_t i;
	int32_t acc;
	static int16_t k=0;	// younger sample index

    acc = 0;										// Clear the accumulator/output before filtering
    S->pState[k] = *pSrc;					 // Place new input sample as first element in the filter state array
    //Direct form filter each sample using a sum of products
		for (i = 0; i < S->numTaps; i++)
    {
       acc += (int32_t) S->pState[k++]*S->pCoeffs[i];
       if (k >= S->numTaps) k = 0;
    }
	*pDst = (int16_t) (acc >> 16);

	//k++;
	//if (k >= S->numTaps) k = 0;
	k--;
	if (k <0) k = S->numTaps-1;
}

//=======================================================================================

