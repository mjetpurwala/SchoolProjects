
#include <stdio.h>
#include <stdint.h>
#include "../inc/ST7735.h"
#include "../inc/Timer2A.h"
#include "Display.h"
#include "Menu.h"
#include "Alarm.h"
#include "Stopwatch.h"
#include "Lab3.h"


int hours,minutes,seconds; 
int stopmin,stopsec,stopmil;
int volatile alarmSounding;
int cooldown[5] = {0, 0, 0, 0, 0};

void Timer(void){
	/********************** Increment Time **********************/
	seconds++;
	if(seconds==60){
	seconds=0;
	minutes++;	
	}	
	if(minutes==60){
	minutes=0;
		hours++;
	}
	if(hours==24){
	hours=0;
	}
	/******************* Update Clock Display *******************/
	if(menuflag!=1){
		if(clockflag==1){		
			updateDisplayA(seconds);
		}
		else{
			updateDisplayD(hours, minutes, seconds);
		}
	}
	/********************* Check for Alarm *********************/
	for(int i=0; i < 5; i++){
		if((alarmSet[i] == 1) && (alarmHours[i] == hours) && (alarmMinutes[i] == minutes) && (cooldown[i] == 0)){
			alarmSounding = 1;
			cooldown[i] = 60;
			Timer2A_Init(SoundOut, 60000, 2);
		}
	}
	/********************* Relax Cooldowns *********************/
	for(int i=0; i < 5; i++){
		if(cooldown[i] > 0){
			cooldown[i]--;
		}
	}
}

void Clock_Delay(uint32_t ulCount){
  while(ulCount){
    ulCount--;
  }
}

void Stopwatch(void){
stopmil++;
if(stopmil==100){
	stopmil=0;
	stopsec++;	
	}	
	if(stopsec==60){
	stopmin=0;
		stopmin++;
	}
	if(stopmin==60){
	stopmin=0;
	}
}















// ------------Clock_Delay1ms------------
// Simple delay function which delays about n milliseconds.
// Inputs: n, number of msec to wait
// Outputs: none
void Clock_Delay1ms(uint32_t n){
  while(n){
    Clock_Delay(23746);  // 1 msec, tuned at 80 MHz, originally part of LCD module
    n--;
  }
}
