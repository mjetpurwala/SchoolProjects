// ----------------------------------------------------------------------------
// 
// File name:     Lab5.c
//
// ----------------------------------------------------------------------------
//
// Description:   This lab is designed to be used on the new Lab Board 
// Author:        Mark McDermott and Jon Valvano
// Orig gen date: August 4, 2024
// 
// Goal of this lab: Audio Communication
//

// ----------------------------------------------------------------------------
// Hardware/software assigned to common
// main initialization initializes all modules
//   PD2, Timer5A, ADC1, UART0 implements TExaSdisplay
// ----------------------------------------------------------------------------
// Hardware/software assigned to transmitter
//   UART0, possible source of input data (conflicts with TExaSdisplay)
//   PC7-PC4 Port_C_Init, possible source of input data
//   Timer0A periodic interrupt to generate input
//   Fifo1 linkage from Input to Encoder
//   Timer1A periodic interrupt create a sequence of frequencies
//   Fifo2 linkage from  Encoder to DAC
//   SSI1 PD3 PD1 PD0 TLV5616 DAC output
//   SysTick ISR output to DAC
// ----------------------------------------------------------------------------
// Hardware/software assigned to reciever
//   Timer2A ADC0 samples sound
//   Fifo3 linkage from  ADC to Decoder
//   main-loop runs decoder
//   PE3 PF3, PF2, PF1, LEDs
//   PA2-PA7, SSI0, ST7735R

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/Texas.h"
#include "../inc/PLL.h"
#include "../inc/ST7735.h"
#include "Receiver.h"
#include "Transmitter.h"
#include "../inc/dsp.h"
#include "Switch.h"
#include "../inc/TLV5616.h"
#include "../inc/SysTickInts.h"
#include "../inc/ADCSWTrigger.h"
//#include "../inc/Timer3A.h"
#include "../inc/Timer0A.h"
//#include "../inc/Timer4A.h"
#include "../inc/LED.h"
#include "led_matrix.h"



void DisableInterrupts(void);           // Disable interrupts
void EnableInterrupts(void);            // Enable interrupts






int main(void){
  DisableInterrupts();
  PLL_Init(Bus80MHz); 
	Switch_Init();
	Transmitter_Init();
//	Transmitter_Init();
	// bus clock at 80 MHz
  // write this
	//ADC0_InitSWTriggerSeq3_Ch9();
	//Timer3A_Init(&(torecieve),12500,0);
	Timer0A_Init(&Charliplex, 40000,0);
	//Timer4A_Init(&switch_image, 80000000,0);
	DFT_Init();
	LED_Init();
	LED_Matrix_Init();
  EnableInterrupts();
	while(1){
	//uint32_t result;
      // write this
		if((GPIO_PORTC_DATA_R&0x20) == 0x20){			// Button 1: PC5
			while((GPIO_PORTC_DATA_R&0x20) == 0x20){}
			songchoice=1;
			Music_PlaySong();
			//GPIO_PORTB_DATA_R |= 0x04; //not working idk why, moving on to decoding 
		}
		else if((GPIO_PORTC_DATA_R&0x40) == 0x40){				// Button 2: PC6
			while((GPIO_PORTC_DATA_R&0x40) == 0x40){}
			songchoice=2;
			Music_PlaySong();
		}

		//result = ADC0_InSeq3();
		//Music_Reset();
  }

	
}


