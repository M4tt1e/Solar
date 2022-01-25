#include "stepper.h"

#define NOP_COUNT 40000

static PinStruct_Type COILS[COIL_NUM] = {
	{PTB, PORTB, SIM_SCGC5_PORTB_MASK, 8},
	{PTB, PORTB, SIM_SCGC5_PORTB_MASK, 9},
	{PTB, PORTB, SIM_SCGC5_PORTB_MASK, 10},
	{PTB, PORTB, SIM_SCGC5_PORTB_MASK, 11}
};


void stepper_init()
{
	for(int i=0; i<COIL_NUM; i++){
		SIM->SCGC5 |= COILS[i].clk_mask;              /* connect CLOCK to port */
	  COILS[i].port->PCR[COILS[i].pin] = PORT_PCR_MUX(1UL); /* set MUX to GPIO */
	  COILS[i].gpio->PDDR |= SET_BIT(COILS[i].pin);            /* set as OUTPUT   */
	  COILS[i].gpio->PSOR |= SET_BIT(COILS[i].pin); 
	}
}

void stepper_move(StepperDirection_Type direction)
{
	int start[] = {0, 3};
	int end[] = {4, -1};
	int step[] = {1, -1};

	for(int u=0; u<=52; u++){
		for(int i=start[direction]; i!=end[direction]; i+=step[direction]){
			for(int j=0; j<4; j++){
				if(i != j){
					COILS[j].gpio->PCOR |= SET_BIT(COILS[j].pin);
				}
			}
			COILS[i].gpio->PSOR |= SET_BIT(COILS[i].pin);
			for(uint32_t d=0;d<(NOP_COUNT);d++)__NOP();
		}
	}
}
