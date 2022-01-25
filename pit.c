 
#include "MKL05Z4.h"                    
#include "pit.h"												
#include "tracker.h"

void PIT_IRQHandler(void);

static const IRQn_Type myPIT_IRQn = 22;  


void PIT_IRQHandler() {
	// check to see which channel triggered interrupt 
	if (PIT->CHANNEL[0].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 0
		PIT->CHANNEL[0].TFLG &= PIT_TFLG_TIF_MASK;
		// Do ISR work
		
		tracker_operate();
		
	} else if (PIT->CHANNEL[1].TFLG & PIT_TFLG_TIF_MASK) {
		// clear status flag for timer channel 1
		PIT->CHANNEL[1].TFLG &= PIT_TFLG_TIF_MASK;
	}

	NVIC_ClearPendingIRQ(myPIT_IRQn);
	
}


void pitInitialize(unsigned period) {


	SIM->SCGC6 |= SIM_SCGC6_PIT_MASK; 
	
	// Enable module, freeze timers in debug mode
	PIT->MCR &= ~PIT_MCR_MDIS_MASK; 
	PIT->MCR |= PIT_MCR_FRZ_MASK;  
	
	
	PIT->CHANNEL[0].LDVAL = PIT_LDVAL_TSV(period);

	// No chaining
	PIT->CHANNEL[0].TCTRL &= PIT_TCTRL_CHN_MASK;
	
	// Generate interrupts
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TIE_MASK;

	/* Enable Interrupts */
	NVIC_SetPriority(myPIT_IRQn, 3); 

	NVIC_ClearPendingIRQ(myPIT_IRQn); 
	NVIC_EnableIRQ(myPIT_IRQn);	

}


void startPIT(void) {
	PIT->CHANNEL[0].TCTRL |= PIT_TCTRL_TEN_MASK;
}

void stopPIT(void) {
	PIT->CHANNEL[0].TCTRL &= ~PIT_TCTRL_TEN_MASK;
}


