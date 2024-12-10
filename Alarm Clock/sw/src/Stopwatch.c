#include <stdio.h>
#include <stdint.h>
#include "../inc/ST7735.h"
#include "../inc/tm4c123gh6pm.h"
#include "../inc/Timer1A.h"
#include "Display.h"
#include "Timer.h"
#include "SetTime.h"
#include "Alarm.h"


int startflag;


void StopwatchMenu(void){
	ST7735_FillScreen(0);

	ST7735_DrawString(0,0, "hey hey hey", ST7735_RED);
	ST7735_DrawString(0,1, "let's stop and watch!", ST7735_RED);

	ST7735_DrawString(0,3, "1: START/STOP", ST7735_WHITE);
	ST7735_DrawString(0,4, "2: RESET", ST7735_WHITE);
	ST7735_DrawString(0,12, "3: Exit to Menu", ST7735_YELLOW); // now you save first, then exit :3
	
	int minute1=0;
	int minute2=0;
	int second1=0;
	int second2=0;
	int msecond1=0;
	int msecond2=0;
	int startflag=0;
	ST7735_DrawChar(1, 80,minute1 +'0', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(11, 80,minute2+'0', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(21, 80,':', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(31, 80,second1+'0', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(41, 80,second2+'0', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(51, 80,':', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(61, 80,msecond1+'0', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(71, 80,msecond2+'0', ST7735_CYAN, ST7735_BLACK, 2);
	while(1){
		if((startflag==0)&&((GPIO_PORTC_DATA_R&0x10)==0x10)){ //start!
					Clock_Delay1ms(100);
				Timer1A_Init(&(Stopwatch),800000,1);
					startflag=1;	
		}
		//isolating each digit
		if(startflag==1){
		minute1 = stopmin/10;
		minute2	= stopmin%10;
		second1 = stopsec/10;
		second2 = stopsec%10;
		msecond1= stopmil/10;
		msecond2= stopmil%10;
		ST7735_DrawChar(1, 80,minute1 +'0', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(11, 80,minute2+'0', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(21, 80,':', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(31, 80,second1+'0', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(41, 80,second2+'0', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(51, 80,':', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(61, 80,msecond1+'0', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(71, 80,msecond2+'0', ST7735_CYAN, ST7735_BLACK, 2);	
		}
		if((startflag==1)&&((GPIO_PORTC_DATA_R&0x10)==0x10)){ //start,stop
					Clock_Delay1ms(100);
					startflag=0;	
					Timer1A_Stop();
		}
		if((GPIO_PORTC_DATA_R&0x80)==0x80){ //RESET BUTTON
				Clock_Delay1ms(100);
				Timer1A_Stop();
			ST7735_DrawChar(1, 80,'0', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(11, 80,'0', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(21, 80,':', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(31, 80,'0', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(41, 80,'0', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(51, 80,':', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(61, 80,'0', ST7735_CYAN, ST7735_BLACK, 2);
		ST7735_DrawChar(71, 80,'0', ST7735_CYAN, ST7735_BLACK, 2);
			//	ST7735_DrawString(0,14, "RESET REGISTER", ST7735_YELLOW); // now you save first, then exit :3
				stopmin=0;
				stopsec=0;
				stopmil=0;
				
		}
		
		if((GPIO_PORTC_DATA_R&0x40)==0x40){ //exit button
				Clock_Delay1ms(100);
				ST7735_FillScreen(0);
				minute1=0;
				minute2=0;
				second1=0;
				second2=0;
				msecond1=0;
				msecond2=0;
				stopmin=0;
				stopsec=0;
				stopmil=0;
				return;}
		
		
		
		
	}

	
	
	
	






}