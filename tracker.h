/**
 * @file tracker.h
 * @author Mateusz Edwardsson
 * @date Dec 2021
 * @brief Solar tracker
 * @ver 0.1
 */



#ifndef SOLAR_TRACKER_H
#define SOLAR_TRACKER_H

#define MANUAL_BTN_CLOCKWISE 6
#define MANUAL_BTN_COUNTERCLOCKWISE 7

#define NIGHT_ON_THRESHOLD 0.1f
#define NIGHT_OFF_THRESHOLD 0.15f

typedef enum {
	MANUAL, AUTO
} Tracker_Mode;

typedef struct{
	int best_step;
	float min_val;
	float max_val;
	float avg_val;
} Scan_Result;


void tracker_init(void);

void tracker_operate(void);


#endif
