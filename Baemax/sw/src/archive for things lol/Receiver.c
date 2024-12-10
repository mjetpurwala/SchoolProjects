// Receiver.c
// Lab 5
// Programs to implement receiver functionality   
// ECE445L Fall 2024
//    Jonathan W. Valvano 8/30/24

// ----------------------------------------------------------------------------
// Hardware/software assigned to receiver
//   Timer2A ADC0 samples sound
//   Fifo3 linkage from  ADC to Decoder
//   main-loop runs decoder
//   PE3 PF3, PF2, PF1, LEDs
//   PA2-PA7, SSI0, ST7735R

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/dsp.h"
#include "../inc/fifo.h"
//#include "../inc/ADCSWTrigger.h"
#include "../inc/LED.h"

// write this

uint32_t FIFO [16];
uint32_t fs1,fs2;

void torecieve(void){
int static i=0;

uint32_t x_t;
x_t= ADC0_InSeq3();	
FIFO[i]=	x_t;
	
DFT(i,x_t);

//LED_BlueOn();
 if(i == 15){
        fs1 = Mag1();
        fs2 = Mag2();
        if(fs1 > 0x00100000){
            LED_GreenOn();
            LED_BlueOff();
        } else if (fs2 > 0x00100000){
            LED_BlueOn();
            LED_GreenOff();
        } else {
            LED_GreenOff();
            LED_BlueOff();
        }
    }
 
		i++;
		if(i==16){i=0;}
		
}





