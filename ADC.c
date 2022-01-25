#include "ADC.h"



uint8_t ADC_Init(void)
{
	uint16_t kalib_temp;
	SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;          // Clock enable for ADC0 module
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;	        // Enable clock for port A
	PORTA->PCR[8] =0;//&= ~(PORT_PCR_MUX(7));		// PTA0 - analog input, channel 12
	PORTA->PCR[0] =0;
	
	ADC0->CFG1 = ADC_CFG1_ADICLK(ADICLK_BUS_2) | ADC_CFG1_ADIV(ADIV_4) | ADC_CFG1_ADLSMP_MASK;	// Converson clock BUS/2=10.49MHz, divider = 2, clock = 2.62MHz (2621440Hz), long conversion time
	ADC0->CFG2 = ADC_CFG2_ADHSC_MASK;										// Enable high-speed clock support
	ADC0->SC3  = ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3);		// Enable averaging on 32 samples, triggering on demand by default
	ADC0->SC3 |= ADC_SC3_CAL_MASK;											// Start calibration
	while(ADC0->SC3 & ADC_SC3_CAL_MASK);								// Wait for calibration completion
	
	if(ADC0->SC3 & ADC_SC3_CALF_MASK)
{
	  ADC0->SC3 |= ADC_SC3_CALF_MASK;
	  return(1);																				// Calibration error, return
	}
	
	kalib_temp = 0x00;
	kalib_temp += ADC0->CLP0;
	kalib_temp += ADC0->CLP1;
	kalib_temp += ADC0->CLP2;
	kalib_temp += ADC0->CLP3;
	kalib_temp += ADC0->CLP4;
	kalib_temp += ADC0->CLPS;
	kalib_temp += ADC0->CLPD;
	kalib_temp /= 2;
	kalib_temp |= 0x8000;                       // Set MSB
	ADC0->PG = ADC_PG_PG(kalib_temp);           // Save in plus-side gain calibration register
	ADC0->OFS = 1;													// Zero offest calibration
	ADC0->SC1[0] = ADC_SC1_ADCH(31);						// Disable ADC0
	ADC0->CFG2 |= ADC_CFG2_ADHSC_MASK;					// Fast converson mode
	ADC0->CFG1 = ADC_CFG1_ADICLK(ADICLK_BUS_2) | ADC_CFG1_ADIV(ADIV_1) | ADC_CFG1_ADLSMP_MASK | ADC_CFG1_MODE(MODE_12);	// Converson clock 10.49MHz, 12 bit resolution, long conversion time
	NVIC_ClearPendingIRQ(ADC0_IRQn);
	NVIC_EnableIRQ(ADC0_IRQn);
	
  ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(3);	

	return(0);												// Success, return

}
