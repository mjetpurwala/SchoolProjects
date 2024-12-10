// File **********Transmitter.c***********
// Lab 5
// Programs to implement transmitter functionality   
// EE445L Spring 2021
//    Jonathan W. Valvano 4/4/21
// Hardware/software assigned to transmitter
//   UART0, possible source of input data (conflicts with TExaSdisplay)
//   PC7-PC4 Port_C_Init, possible source of input data
//   Timer0A periodic interrupt to generate input
//   FreqFifo linkage from Encoder to SendData
//   Timer1A periodic interrupt create a sequence of frequencies
//   SSI1 PD3 PD1 PD0 TLV5616 DAC output
//   SysTick ISR output to DAC

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/SysTickInts.h"
#include "../inc/Timer0A.h"
#include "../inc/Timer1A.h"
#include "../inc/fifo.h"
#include "../inc/tlv5616.h"
#include "../inc/Timer1A.h"
#include "../inc/Timer2A.h"
#include "Switch.h"
int songchoice;

  struct note{
	uint32_t pitch;
	uint32_t duration;
};typedef const struct note note_t;
note_t *notePtr; // Index tracker for notes
note_t Song[2] = {{3188,80000000}, {0,0}};
note_t Song1[2] = {{3188,80000000}, {0,0}};
//1193


//--------------Timer Func-----------------
		// set the frequency (pitch) (SysTick)
void func2(void){
	
//	if((GPIO_PORTC_DATA_R&0x20) == 0x20){												// Button 1: PC5

	// note change detected:
		notePtr++; // increment noteptr
		if((*notePtr).duration == 0){
			TIMER2_CTL_R = 0x00000000;
			TIMER1_CTL_R = 0x00000000;
		}else{
			TIMER1_TAILR_R =(*notePtr).pitch;// set systick with new pitch
			TIMER2_TAILR_R = (*notePtr).duration;
		}
}
	//}

void Transmitter_Init(void){
	
	if(songchoice==1){notePtr = Song;songchoice=0;}
	else{notePtr = Song1;songchoice=0;}
	DAC_Init(0);
	 }

   //-------------------Music_PlaySong--------------------------
// Play song, while button pushed or until end
void Music_PlaySong(void){
	
	
//if((GPIO_PORTC_DATA_R&0x20) == 0x20){												// Button 1: PC5
	
	Transmitter_Init();
	Timer2A_Init(func2, (*notePtr).duration, 1);
	Timer1A_Init(func1, (*notePtr).pitch, 2);

//	Timer2A_Stop();
//	Timer1A_Stop();
	
	//}
		// start going thru the song
		// at every note change, reset systick's frequency
		// every note change is determined by note duration

}

void Music_Reset(void){
		TIMER2_CTL_R = 0x00000000;
		TIMER1_CTL_R = 0x00000000;
		TIMER1_TAILR_R =0;
		TIMER2_TAILR_R = 0;
}

