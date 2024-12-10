#include <stdio.h>
#include <stdint.h>
#include "../inc/ST7735.h"
#include "../inc/tm4c123gh6pm.h"
#include "Timer.h"

void SetTimeMenu(void){

	/**************** Display User Guide ****************/
	ST7735_FillScreen(0);

	ST7735_DrawString(0,0, "What time would you", ST7735_RED);
	ST7735_DrawString(0,1, "like to set?", ST7735_RED);

	ST7735_DrawString(0,2, "1: Set Hours", ST7735_WHITE);
	ST7735_DrawString(0,4, "2: Set Minutes", ST7735_WHITE);
	ST7735_DrawString(0,6, "3: Set the Time!", ST7735_WHITE); // hewo, I made the button mappings more intuitive!
	ST7735_DrawString(0,12, "4: Exit to Menu", ST7735_YELLOW); // now you save first, then exit :3

	/**************** Initialize Time Display ****************/
	char hour1='0';
	char hour2='0';
	char minute1='0';
	char minute2='0';
	int hourflag =0;
	int minuteflag=0;
	
	while(1){ //(GPIO_PORTC_DATA_R&0x40)!=0x40
	ST7735_DrawChar(1, 80,hour1, ST7735_CYAN, ST7735_BLACK, 4);
	ST7735_DrawChar(31, 80,hour2, ST7735_CYAN, ST7735_BLACK, 4);
	ST7735_DrawChar(51, 80,':', ST7735_CYAN, ST7735_BLACK, 4);
	ST7735_DrawChar(71, 80,minute1, ST7735_CYAN, ST7735_BLACK, 4);
	ST7735_DrawChar(101, 80,minute2, ST7735_CYAN, ST7735_BLACK, 4);
		
		/**************** Option 1: Set Hours ****************/
		if((GPIO_PORTC_DATA_R&0x10)==0x10){
			Clock_Delay1ms(100);
			if(hourflag==23){hourflag=0;} 
			if(hour2=='9'){
				hour2='0';
				hour1++;
				hour2--;
			}
			if(hour1=='2' && hour2=='3'){
				hour1='0';
				hour2='0';
				hour2--;
				}
		hour2++;
		hourflag++;
		}
		
		/**************** Option 2: Set Minutes ****************/
		if((GPIO_PORTC_DATA_R&0x80)==0x80){
			//while((GPIO_PORTC_DATA_R&0x80) == 0x80){}
			Clock_Delay1ms(100);
			if(minuteflag==59){minuteflag=0;}

				if(minute2=='9'){
				minute2='0';
				minute1++;
				minute2--;
				if(minute1=='6'){
				minute1='0';
				minute2='0';
				minute2--;
				}	
			}		
		minute2++;
		minuteflag++;
		}
	
		/**************** Option 3: Set the Time! ****************/
		if((GPIO_PORTC_DATA_R&0x40)==0x40){
				//while((GPIO_PORTC_DATA_R&0x40) == 0x40){}
			Clock_Delay1ms(100);
			ST7735_DrawString(0,14, "TIME SUCCESSFULLY SET!", ST7735_RED);
			hours=hourflag;
			minutes=minuteflag;
			seconds=0;
		}
		
		/**************** Option 4: Return to Menu ****************/
		if((GPIO_PORTC_DATA_R&0x20)==0x20){
				//while((GPIO_PORTC_DATA_R&0x20) == 0x20){}
			Clock_Delay1ms(100);
			ST7735_FillScreen(0);

			return;
		}
	}
}


