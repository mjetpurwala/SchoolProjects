#include <stdio.h>
#include <stdint.h>
#include "../inc/ST7735.h"
#include "../inc/tm4c123gh6pm.h"
#include "Timer.h"

void PlotDot(int slot){
	ST7735_DrawSmallCircle(3, 10 + slot*20, ST7735_WHITE);
}

void EraseDot(int slot){
		if(slot == 0){
			ST7735_DrawSmallCircle(3, 90, ST7735_BLACK);
		}else{
			ST7735_DrawSmallCircle(3, 10 + (slot-1)*20, ST7735_BLACK);
		}
}

char hour1[5] = {'0', '0', '0', '0', '0'};
char hour2[5] = {'0', '0', '0', '0', '0'};
char minute1[5] = {'0', '0', '0', '0', '0'};
char minute2[5] = {'0', '0', '0', '0', '0'};
int hourflag[5] = {0, 0, 0, 0, 0};
int minuteflag[5] = {0, 0, 0, 0, 0};
int alarmHours[5] = {0, 0, 0, 0, 0}; 
int alarmMinutes[5] = {0, 0, 0, 0, 0};
volatile int alarmSet[5] = {0, 0, 0, 0, 0};

char alarm1[6] = {'0', '0', ':', '0', '0', '\0'};
char alarm2[6] = {'0', '0', ':', '0', '0', '\0'};
char alarm3[6] = {'0', '0', ':', '0', '0', '\0'};
char alarm4[6] = {'0', '0', ':', '0', '0', '\0'};
char alarm5[6] = {'0', '0', ':', '0', '0', '\0'};

void SetPrintedAlarm(void){
	//Set Alarm 1
	alarm1[0] = hour1[0];
	alarm1[1] = hour2[0];
	alarm1[3] = minute1[0];
	alarm1[4] = minute2[0];
	
	alarm2[0] = hour1[1];
	alarm2[1] = hour2[1];
	alarm2[3] = minute1[1];
	alarm2[4] = minute2[1];
	
	alarm3[0] = hour1[2];
	alarm3[1] = hour2[2];
	alarm3[3] = minute1[2];
	alarm3[4] = minute2[2];
	
	alarm4[0] = hour1[3];
	alarm4[1] = hour2[3];
	alarm4[3] = minute1[3];
	alarm4[4] = minute2[3];
	
	alarm5[0] = hour1[4];
	alarm5[1] = hour2[4];
	alarm5[3] = minute1[4];
	alarm5[4] = minute2[4];
}

void SetAlarm(int slot){
	ST7735_FillScreen(0);

	ST7735_DrawString(0,0, "What time would you", ST7735_RED);
	ST7735_DrawString(0,1, "like to set?", ST7735_RED);

	ST7735_DrawString(0,2, "1: Set Hours", ST7735_WHITE);
	ST7735_DrawString(0,4, "2: Set Minutes", ST7735_WHITE);
	ST7735_DrawString(0,6, "3: Turn ON/OFF Alarm!", ST7735_WHITE); // hewo, I made the button mappings more intuitive!
	ST7735_DrawString(0,12, "4: Exit to Menu", ST7735_YELLOW); // now you save first, then exit :3
	

	/**************** Initialize Time Display ****************/
	
	while(1){ //(GPIO_PORTC_DATA_R&0x40)!=0x40
		
		
		ST7735_DrawChar(1, 80, hour1[slot], ST7735_CYAN, ST7735_BLACK, 4);
		ST7735_DrawChar(31, 80, hour2[slot], ST7735_CYAN, ST7735_BLACK, 4);
		ST7735_DrawChar(51, 80,':', ST7735_CYAN, ST7735_BLACK, 4);
		ST7735_DrawChar(71, 80, minute1[slot], ST7735_CYAN, ST7735_BLACK, 4);
		ST7735_DrawChar(101, 80, minute2[slot], ST7735_CYAN, ST7735_BLACK, 4);
	
		/**************** Option 1: Set Hours ****************/
		if((GPIO_PORTC_DATA_R&0x10)==0x10){
			//while((GPIO_PORTC_DATA_R&0x10) == 0x10){}
			Clock_Delay1ms(100);
			if(hourflag[slot]==23){hourflag[slot]=0;}
			if(hour2[slot]=='9'){
				hour2[slot]='0';
				hour1[slot]++;
				hour2[slot]--;
			}
			if(hour1[slot]=='2' && hour2[slot]=='3'){ // Should probably change this to military time so no need for AM/PM
				hour1[slot]='0';
				hour2[slot]='0';
				hour2[slot]--;
				}
		hour2[slot]++;
		hourflag[slot]++;
		}
		
		/**************** Option 2: Set Minutes ****************/
		if((GPIO_PORTC_DATA_R&0x80)==0x80){
			//while((GPIO_PORTC_DATA_R&0x80) == 0x80){}
			Clock_Delay1ms(100);
			if(minuteflag[slot]==59){minuteflag[slot]=0;}

				if(minute2[slot]=='9'){
				minute2[slot]='0';
				minute1[slot]++;
				minute2[slot]--;
				if(minute1[slot]=='6'){
				minute1[slot]='0';
				minute2[slot]='0';
				minute2[slot]--;
				}	
			}		
		minute2[slot]++;
		minuteflag[slot]++;
		}
	
		/**************** Option 3: Turn ON/OFF Alarm! ****************/
		if((GPIO_PORTC_DATA_R&0x40)==0x40){
				//while((GPIO_PORTC_DATA_R&0x40) == 0x40){}
			Clock_Delay1ms(100);
			if(alarmSet[slot]==0){
				ST7735_DrawString(0,14, "ALARM ON!", ST7735_RED);
				alarmSet[slot]=1;
				alarmHours[slot]=hourflag[slot];
				alarmMinutes[slot]=minuteflag[slot];
			}else{
				ST7735_DrawString(0,14, "ALARM OFF!", ST7735_RED);
				alarmSet[slot]=0;
			}
		}
		
		/**************** Option 4: Return to Menu ****************/
		if((GPIO_PORTC_DATA_R&0x20)==0x20){
				//while((GPIO_PORTC_DATA_R&0x20) == 0x20){}
			Clock_Delay1ms(100);
			SetPrintedAlarm();
			
			ST7735_FillScreen(0);
			
			ST7735_DrawString(0,0, "--------------------", ST7735_CYAN);
			ST7735_DrawString(2,1, "| Alarm 1 - ", ST7735_CYAN);
			ST7735_DrawString(14,1, alarm1, ST7735_CYAN);
			ST7735_DrawString(0,2, "--------------------", ST7735_CYAN);
			ST7735_DrawString(2,3, "| Alarm 2 - ", ST7735_CYAN);
			ST7735_DrawString(14,3, alarm2, ST7735_CYAN);
			ST7735_DrawString(0,4, "--------------------", ST7735_CYAN);
			ST7735_DrawString(2,5, "| Alarm 3 - ", ST7735_CYAN);
			ST7735_DrawString(14,5, alarm3, ST7735_CYAN);
			ST7735_DrawString(0,6, "--------------------", ST7735_CYAN);
			ST7735_DrawString(2,7, "| Alarm 4 - ", ST7735_CYAN);
			ST7735_DrawString(14,7, alarm4, ST7735_CYAN);
			ST7735_DrawString(0,8, "--------------------", ST7735_CYAN);
			ST7735_DrawString(2,9, "| Alarm 5 - ", ST7735_CYAN);
			ST7735_DrawString(14,9, alarm5, ST7735_CYAN);
			ST7735_DrawString(0,10, "--------------------", ST7735_CYAN);
			ST7735_DrawString(0,11, "1: Select Alarm Slot", ST7735_WHITE);
			ST7735_DrawString(0,13, "2: Confirm Selection", ST7735_WHITE); 
			ST7735_DrawString(0,15, "3: Exit to Menu", ST7735_YELLOW);
			
			PlotDot(slot);
			return;
		}
	}
}

int slot;
void AlarmMenu(void){
	
	/**************** Display User Guide ****************/
	ST7735_FillScreen(0);
	
	ST7735_DrawString(0,0, "--------------------", ST7735_CYAN);
	ST7735_DrawString(2,1, "| Alarm 1 - ", ST7735_CYAN);
	ST7735_DrawString(14,1, alarm1, ST7735_CYAN);
	ST7735_DrawString(0,2, "--------------------", ST7735_CYAN);
	ST7735_DrawString(2,3, "| Alarm 2 - ", ST7735_CYAN);
	ST7735_DrawString(14,3, alarm2, ST7735_CYAN);
	ST7735_DrawString(0,4, "--------------------", ST7735_CYAN);
	ST7735_DrawString(2,5, "| Alarm 3 - ", ST7735_CYAN);
	ST7735_DrawString(14,5, alarm3, ST7735_CYAN);
	ST7735_DrawString(0,6, "--------------------", ST7735_CYAN);
	ST7735_DrawString(2,7, "| Alarm 4 - ", ST7735_CYAN);
	ST7735_DrawString(14,7, alarm4, ST7735_CYAN);
	ST7735_DrawString(0,8, "--------------------", ST7735_CYAN);
	ST7735_DrawString(2,9, "| Alarm 5 - ", ST7735_CYAN);
	ST7735_DrawString(14,9, alarm5, ST7735_CYAN);
	ST7735_DrawString(0,10, "--------------------", ST7735_CYAN);
	ST7735_DrawString(0,11, "1: Select Alarm Slot", ST7735_WHITE);
	ST7735_DrawString(0,13, "2: Confirm Selection", ST7735_WHITE); 
	ST7735_DrawString(0,15, "3: Exit to Menu", ST7735_YELLOW);
	
	/*************** Initialize Selection Dot **************/
	PlotDot(slot);
	
	while(1){
		/************** Option 1: Select Alarm Slot **************/	// --MAPPINGS--
		if((GPIO_PORTC_DATA_R&0x10) == 0x10){												// Button 1: PC4
			Clock_Delay1ms(200); // Feel free to adjust this delay as needed
				slot++;
				if(slot == 5){
					slot = 0;
				}
				PlotDot(slot);
				EraseDot(slot);
		}
		
		/************** Option 2: Confirm Selection **************/
		if((GPIO_PORTC_DATA_R&0x80) == 0x80){												// Button 2: PC7
			Clock_Delay1ms(100);
			SetAlarm(slot);
		}
		
		/******************** Option 3: Exit *********************/
		if((GPIO_PORTC_DATA_R&0x40) == 0x40){												// Button 3: PC6
			Clock_Delay1ms(100);
				ST7735_FillScreen(0);
				break;
		}
	}
	
}
