/**
* @file main.c
* @author Mateusz Edwardsson
* @date Dec 2021
* @brief Solar tracker
* @ver 0.1
*/

#include <MKL05Z4.h>

#include "solar.h"
#include "stepper.h"
#include "tracker.h"
#include "lcd1602.h"
#include "pit.h"
#include "ADC.h"
#include "uart0.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main()
{
	LCD1602_Init();
	LCD1602_Backlight(1);
  ADC_Init();
	stepper_init();
  tracker_init();	
	UART0_Init();	
	pitInitialize(20901);
	
	startPIT();

	
	while(1);
}
