#include "solar.h"
#include "ADC.h"

static float	wynik;
static float adc_volt_coeff = ((float)(((float)2.91) / 4095) );		
static int cnt =0;
static float sum;
static float result = 0.3f;
static float temperature;

static uint8_t read_channel[] = {3, 12};
#define CHANNEL_SOLAR 0
#define CHANNEL_TEMP 1
static uint8_t active_channel = CHANNEL_SOLAR;

void calculate_light_intensity(uint16_t val);
void calculate_temperature(uint16_t val);


void calculate_light_intensity(uint16_t val)
{
	wynik = (val*adc_volt_coeff);
	sum = sum+wynik;
	if(++cnt == 1800){
		result = sum / (float)cnt;
		sum = 0.0f;
		cnt = 0;
	}
}

void calculate_temperature(uint16_t val)
{
  temperature = 17	 + ((val*3.0*adc_volt_coeff - 2.98f) * 100);
}

void ADC0_IRQHandler()
{	
	uint16_t temp = ADC0->R[0];		// Reading the data and clearing the COCO flag
	if(active_channel == CHANNEL_SOLAR)
	{
		calculate_light_intensity(temp);
	}
	else if(active_channel == CHANNEL_TEMP)
	{
		calculate_temperature(temp);
	}
	active_channel = !active_channel;
	ADC0->SC1[0] = ADC_SC1_AIEN_MASK | ADC_SC1_ADCH(read_channel[active_channel]);
}


float solar_power()
{
	float pcent = 2.61f / 100.0f;
	return result / pcent;
}

float solar_voltage()
{
	return result;
}

float get_temperature(){
	return temperature;
}