#ifndef __Timer_h
#define __Timer_h

#include <stdint.h>

extern int hours, minutes, seconds; 
extern int stopmin,stopsec,stopmil;
extern volatile int alarmSounding;

void Timer(void);
void Stopwatch(void);
void Clock_Delay1ms(uint32_t n);
	
#endif /* __Timer_h */

