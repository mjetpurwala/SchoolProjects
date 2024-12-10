// LED.c
// Runs on TM4C123
// Provide functions that initialize a GPIO as an input pin and
// allow reading of two negative logic switches on PF0 and PF4
// Output to LEDs
// Use bit-banded I/O.
// Daniel and Jonathan Valvano
// Feb 23, 2015
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

/* This example accompanies the book
   "Embedded Systems: Introduction to ARM Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014
   Section 4.2    Program 4.2

  "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014
   Example 2.3, Program 2.9, Figure 2.36

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
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


#define GPIO_LOCK_KEY           0x4C4F434B  // Unlocks the GPIO_CR register
#define PF0                     (*((volatile uint32_t *)0x40025004))
#define PF4                     (*((volatile uint32_t *)0x40025040))
#define SWITCHES                (*((volatile uint32_t *)0x40025044))
#define SW1       0x10                      // on the left side of the Launchpad board
#define SW2       0x01                      // on the right side of the Launchpad board
#define PF1                     (*((volatile uint32_t *)0x40025008))
#define PF2                     (*((volatile uint32_t *)0x40025010))
#define PF3                     (*((volatile uint32_t *)0x40025020))
	
#define Pin0 0x1
#define Pin1 0x2
#define Pin2 0x4
#define Pin3 0x8

#define IMAGES_LEN 2
#define PINS 12

uint32_t images[2][12] = {{1,0,1,0,1,0,1,1,1,0,1,0}, 
													{1,0,1,0,1,0,1,1,1,0,0,0}};

uint32_t pinH[12] = {1,2,3,0,1,2,2,3,3,0,1,0};

uint32_t cur_image = 0;

//------------LED_Init------------
// Initialize GPIO Port F for negative logic switches on PF0 and
// PF4 as the Launchpad is wired.  Weak internal pull-up
// resistors are enabled, and the NMI functionality on PF0 is disabled.
// LEDs on PF3,2,1 are enabled
// Input: none
// Output: none
void LED_Matrix_Init(void){
	SYSCTL_RCGCGPIO_R |= 0x08;  	// Enable Port D clock
	
  while((SYSCTL_PRGPIO_R & 0x08) == 0){}; // Wait for clock
	GPIO_PORTD_DEN_R |= 0xF;     // Enable digital I/O 
	GPIO_PORTD_DIR_R |= 0xF;   	// Set PD123 as outputs
}

void LED_Matrix_On(uint32_t pin){
	GPIO_PORTD_DATA_R |= 0x1 << pin;
}

void LED_Matrix_Off(uint32_t pin){
	GPIO_PORTD_DATA_R &= ~(0x1 << pin);
}

void LED_Matrix_Off_All(void){
	GPIO_PORTD_DATA_R = 0x0;
}

void Clock_Delay(uint32_t ulCount){
  while(ulCount){
    ulCount--;
  }
}

void Clock_Delay1ms(uint32_t n){
  while(n){
    Clock_Delay(23746);  // 1 msec, tuned at 80 MHz, originally part of LCD module
    n--;
  }
}

//called every 1s
void switch_image(void){
	cur_image = (cur_image+1)%IMAGES_LEN;
}

//called every 50ms
void Charliplex (void){
	static int cur_pin = 0;
	LED_Matrix_Off_All();
	if(images[cur_image][cur_pin] == 1){
		LED_Matrix_On(pinH[cur_pin]);
	}
	cur_pin = (cur_pin+1)%PINS;
}


