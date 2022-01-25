
#include <MKL05Z4.h>


#ifndef STEPPER_H
#define STEPPER_H

#define SET_BIT(x) ((uint32_t)(1UL << (x)))

#define COIL_NUM 4


typedef enum {
	COUNTERCLOCKWISE, CLOCKWISE
} StepperDirection_Type;

typedef struct {
	GPIO_Type 	*gpio;        /* GPIO base pointer */
	PORT_Type 	*port;        /* PORT base pointer */
	uint32_t		clk_mask;     /* Mask for SCGC5 register */
	uint8_t  		pin;          /* Number of PIN */
} PinStruct_Type;


void stepper_init(void);

void stepper_move(StepperDirection_Type direction);


#endif
