#include "tracker.h"
#include "stepper.h"
#include "solar.h"
#include "lcd1602.h"
#include "pit.h"

static Tracker_Mode tracker_mode;
static Scan_Result result;
static unsigned step_no;
static float init_val = 0;
static int night = 0;

static uint32_t t=0;
static int cnt = 0;

void tracker_manual_operation(void);
void tracker_auto_operation(void);
void tracker_scan(void);
void move_stepper(StepperDirection_Type dir);
void tracker_reset_position(void);


void move_stepper(StepperDirection_Type dir)
{
	if(dir == COUNTERCLOCKWISE && step_no >= 9) return;
	if(dir == CLOCKWISE && step_no <= 0) return;
	step_no += (dir == CLOCKWISE ? -1 : 1);
	stepper_move(dir);
}

void tracker_reset_position()
{
	if(step_no == 6) return;
	if(step_no < 6){
		for(unsigned i=0; i<(6-step_no); i++){
			stepper_move(CLOCKWISE);
			step_no++;
		}
	}else {
		for(unsigned i=0; i<(step_no-6); i++){
			stepper_move(COUNTERCLOCKWISE);
			step_no--;
		}
	}
	LCD1602_ClearAll();
}


void tracker_init(void)
{
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	
	for(unsigned char i=0; i<2; ++i){
		 PORTB->PCR[6+i] |= PORT_PCR_MUX(1);
		PORTB->PCR[6 + i] |= PORT_PCR_PE_MASK | PORT_PCR_PS_MASK; 
	}
	tracker_mode = ((PTB->PDIR & (1<<(MANUAL_BTN_CLOCKWISE))) == 0  && (PTB->PDIR & (1<<(MANUAL_BTN_COUNTERCLOCKWISE))) == 0)
		? MANUAL : AUTO;
	if(tracker_mode == AUTO)
	{
		tracker_scan();
	}
}


void tracker_manual_operation(void)
{
	int clockwise = (PTB->PDIR & (1<<(MANUAL_BTN_CLOCKWISE))) == 0;
	int counterclockwise = (PTB->PDIR & (1<<(MANUAL_BTN_COUNTERCLOCKWISE))) == 0;
	if(clockwise && counterclockwise) return;
	if(clockwise){
		stepper_move(CLOCKWISE);
		return;
	}
	if(counterclockwise){
		stepper_move(COUNTERCLOCKWISE);
		return;
	}

}

void tracker_scan()
{
	LCD1602_SetCursor(1,0);
	LCD1602_Print("Scanning...");
	for(int i=0;i<5;++i) stepper_move(CLOCKWISE);
	
	int best_step=0;
	float max_val = 0, min_val = 0, total = 0;
	for(int step=0; step<10; ++step)
	{
		float val = solar_voltage();
		total += val;
		if(step != 0)
		{
			if(val > max_val)
			{
				max_val = val;
				best_step = step;
			}
			if(val < min_val)
			{
				min_val = val;
			}
		}
		else
		{
			max_val = val;
			min_val = val;
		}
		stepper_move(COUNTERCLOCKWISE);
	}
	result.best_step = best_step;
  result.min_val = min_val;
	result.max_val = max_val;
	result.avg_val = total / 10.0f;
	
	for(int i=0; i<(9-best_step); ++i) stepper_move(CLOCKWISE);
	step_no = result.best_step;
}

void tracker_auto_operation()
{
	float new_val = solar_voltage();
	
	if(night && new_val > NIGHT_OFF_THRESHOLD){
		night = 0;
		tracker_reset_position();
		tracker_scan();
		return;
	}
	if(new_val <= NIGHT_ON_THRESHOLD){
		night = 1;
		return;
	}
	
	init_val = init_val > 0 || init_val < 0 ? init_val : result.max_val;
	
	if(init_val - new_val  >= 0.15|| init_val - new_val <= -0.2){
		move_stepper(CLOCKWISE);
		float cw_val = solar_voltage();
		
		move_stepper(COUNTERCLOCKWISE);
		move_stepper(COUNTERCLOCKWISE);
		float ccw_val = solar_voltage();
		
		if(new_val > cw_val && new_val > ccw_val){
			move_stepper(CLOCKWISE);
			}
		else if(cw_val>ccw_val){
			move_stepper(CLOCKWISE);
			move_stepper(CLOCKWISE);
			float next_val = 0;
			float curr_val = 0;
			while(next_val >= curr_val){
				curr_val = solar_voltage();
				move_stepper(CLOCKWISE);
				next_val = solar_voltage();
				if(step_no == 0){ break;}
			}
			move_stepper(COUNTERCLOCKWISE);
			init_val = curr_val;
		}
		else{
			float next_val = 0;
			float curr_val = 0;
			while(next_val >= curr_val){
				curr_val = solar_voltage();
				move_stepper(COUNTERCLOCKWISE);
				next_val = solar_voltage();
				if(step_no == 9){ break;}
			}
			move_stepper(CLOCKWISE);
			init_val = curr_val;
		}
	}
}

void tracker_operate(void)
{
	char display[]={0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20};

	stopPIT();
	if(tracker_mode == MANUAL)
	{
		tracker_manual_operation();
	}
	else if(tracker_mode == AUTO)
	{
		tracker_auto_operation();
		sprintf(display, "Mode: %s", night ? "Night    " : "Operating");
		LCD1602_SetCursor(0, 1);
		LCD1602_Print(display);
	}
	sprintf(display,"U=%.2fV  %.1f%% ",solar_voltage(), solar_power());
	LCD1602_SetCursor(0,0);
	LCD1602_Print(display);
	
	cnt++;
	if(cnt == 50){
			sprintf(display,"T=%.2fC\n",get_temperature());
			for(t=0;display[t]!=0;t++)
			{
				while(!(UART0->S1 & UART0_S1_TDRE_MASK));
				UART0->D = display[t];
			}
			cnt = 0;
		}
  
	startPIT();
}
