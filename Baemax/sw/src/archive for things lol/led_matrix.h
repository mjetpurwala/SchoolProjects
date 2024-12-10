// File **********Transmitter.h***********
// Solution to Lab9 (not to be shown to students)
// Programs to implement transmitter functionality   
// EE445L Spring 2021
//    Jonathan W. Valvano 4/4/21
// 2-bit input, positive logic switches, positive logic software
#ifndef __LED_MATRIX_H__
#define __LED_MATRIX_H__



#include <stdint.h>


//------------LED_Init------------
// Initialize GPIO Port F for negative logic switches on PF0 and
// PF4 as the Launchpad is wired.  Weak internal pull-up
// resistors are enabled, and the NMI functionality on PF0 is disabled.
// LEDs on PF3,2,1 are enabled
// Input: none
// Output: none

void LED_Matrix_Init(void);
	
void LED_Init(void);


void LED_On(uint32_t pin);

void Charliplex (void);
void switch_image(void);

#endif 

