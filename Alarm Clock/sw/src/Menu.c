#include <stdio.h>
#include <stdint.h>
#include "../inc/ST7735.h"
#include "../inc/tm4c123gh6pm.h"
#include "Display.h"
#include "Timer.h"
#include "SetTime.h"
#include "Alarm.h"
#include "Stopwatch.h"
//#include "SetTimer.h" //uncomment these when ready
//#include "Stopwatch.h"

int menuflag;
void menu(void){
	menuflag=1;
	ST7735_FillScreen(0);

	uint16_t brown  = ST7735_Color565(190,115,50);

	while(1){
		/**************** Display Menu Options ****************/
		ST7735_DrawString(0,0, "Choose your option!", ST7735_RED);
		ST7735_DrawString(0,3, "1: Exit", brown);
		ST7735_DrawString(0,6, "2: Set Time", brown);
		ST7735_DrawString(0,9, "3: Set Alarm", brown);
		ST7735_DrawString(0,12, "4: Stopwatch", brown);	
		/**************** Option 1: Exit ****************/					// --MAPPINGS--
		if((GPIO_PORTC_DATA_R&0x10) == 0x10){												// Button 1: PC4
			//while((GPIO_PORTC_DATA_R&0x10) == 0x10){}
			Clock_Delay1ms(100);
			if(clockflag==1){analogInit();}
			else{digitalInit();}
				menuflag=0;
			break;
		}	
		/**************** Option 2: Set Time ****************/
		if((GPIO_PORTC_DATA_R&0x80) == 0x80){												// Button 2: PC7
			Clock_Delay1ms(100);
			SetTimeMenu();
		}	
		/**************** Option 3: Set Alarm ****************/
		if((GPIO_PORTC_DATA_R&0x40) == 0x40){												// Button 3: PC6
			Clock_Delay1ms(100);	
			AlarmMenu();
		}
		/**************** Option 4: StopWatch ****************/
		if((GPIO_PORTC_DATA_R&0x20) == 0x20){												// Button 4: PC5
				Clock_Delay1ms(100);
			StopwatchMenu();
		}
	}
}
