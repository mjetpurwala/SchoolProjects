// Lab3Main.c
// Runs on LM4F120/TM4C123
// Uses ST7735.c to display the clock
// patterns to the LCD.
//    16-bit color, 128 wide by 160 high LCD
// Mark McDermott, Daniel Valvano and Jonathan Valvano
// Lab solution, do not post
// August 3,2024

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2024

 Copyright 2024 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// Specify your hardware connections, feel free to change
// PD0 is squarewave output to speaker
// PE0 is mode select
// PE1 is left
// PE2 is right 
// PE3 is up
// PE4 is down
// if alarm is sounding, any button will quiet the alarm

#include <stdio.h>
#include <stdint.h>
#include "../inc/ST7735.h"
#include "../inc/PLL.h"
#include "../inc/tm4c123gh6pm.h"
#include "../inc/Timer0A.h"
#include "../inc/Timer1A.h"
#include "../inc/Timer2A.h"
#include "../inc/Sine.h"
#include "Lab3.h"
#include "Display.h"
#include "Menu.h"
#include "SetTime.h"
#include "Timer.h"

// ---------- Prototypes   -------------------------
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void WaitForInterrupt(void);  // low power mode




void Port_Init(){uint32_t volatile delay;
	
	
	/*************** Port C & F: Input Switches ***************/
	
	/* Mapping:
	Button 1: PC4
	Button 2: PC7
	Button 3: PC6
	Button 4: PC5
	Button 5: PF4
	Button 6: PF3
	*/

	SYSCTL_RCGCGPIO_R |= 0x24; // activate port C & port F
	delay = SYSCTL_RCGCGPIO_R;  // wait for clock
	GPIO_PORTC_DEN_R |= 0xF0;  // enable digital I/O for PC4-7
  GPIO_PORTC_DIR_R &= ~0xF0;  // set PC4-7 as inputs
	GPIO_PORTF_DEN_R |= 0x18;  // enable digital I/O for PF3-4
  GPIO_PORTF_DIR_R &= ~0x18;  // set PF3-4 as inputs
	

	//pull down resistors 
	GPIO_PORTC_PDR_R |= 0xF0;
	GPIO_PORTF_PDR_R |= 0x18;
	
	
	/*************** Port B: Speaker Output ***************/
	SYSCTL_RCGCGPIO_R |= 0x02; // activate port B
	delay = SYSCTL_RCGCGPIO_R;  // wait for clock
	GPIO_PORTB_DEN_R |= 0x02;  // enable digital I/O for PB1
  GPIO_PORTB_DIR_R |= 0x02;  // set PB1 as outputs
	
	// the one PB0 for the lcd
	GPIO_PORTB_DEN_R |= 0x01;  // enable digital I/O for PB1
  GPIO_PORTB_DIR_R |= 0x01;  // set PB1 as outputs
	
	//need port a 
	SYSCTL_RCGCGPIO_R |= 0x01; // activate port A
	delay = SYSCTL_RCGCGPIO_R;  // wait for clock
	GPIO_PORTA_DEN_R |= 0xFC;
  GPIO_PORTA_DIR_R |= 0xFC;  // set PA7-2 as outputs
	
	
	
	
	//GPIO_PORTB_PCTL_R&=~ 0x00FFFFFF;
	//GPIO_PORTB_DR8R_R |= 0x02; // allow TM4C123 to drive 8mA current
}

void SoundOut(void){
	DisableInterrupts();
	GPIO_PORTB_DATA_R ^= 0x02;
	EnableInterrupts();
}

int main(void){
  DisableInterrupts();
	PLL_Init(Bus80MHz);    // bus clock at 80 MHz
	Timer0A_Init(&(Timer),80000000,0);
	Port_Init();
	Output_Init();
	analogInit();
  EnableInterrupts();
  while(1){
		/**************** Toggle Clock Display Mode ****************/
		if((GPIO_PORTC_DATA_R&0x80)==0x80){
			//while((GPIO_PORTC_DATA_R&0x10)==0x10){}
			Clock_Delay1ms(100);
			if(clockflag==1){ digitalInit();}
			else { analogInit();}
		}	
		/********************** Entering Menu **********************/
		if((GPIO_PORTC_DATA_R&0x10)==0x10){
			//while((GPIO_PORTC_DATA_R&0x10)==0x10){}
			Clock_Delay1ms(100);
			menu();
		}
		/******************** Turning Off Alarm ********************/
		if((alarmSounding == 1) && ((GPIO_PORTF_DATA_R&0x08)==0x08)){
			Clock_Delay1ms(100);
			alarmSounding = 0;
			Timer2A_Stop();			
		}		
	}	
}

