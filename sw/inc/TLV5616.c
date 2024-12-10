// TLV5616.c
// Runs on TM4C123
// Use SSI1 to send a 16-bit code to the TLV5616 and return the reply.
// Daniel Valvano
// EE445L Fall 2015
//    Jonathan W. Valvano 9/22/15

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to ARM Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

 Copyright 2014 by Jonathan W. Valvano, valvano@mail.utexas.edu
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

// SSIClk (SCLK) connected to PD0
// SSIFss (FS)   connected to PD1
// SSITx (DIN)   connected to PD3

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"

//----------------   DAC_Init     -------------------------------------------
// Initialize TLV5616 12-bit DAC
// assumes bus clock is 80 MHz
// inputs: initial voltage output (0 to 4095)
// outputs:none
void DAC_Init(uint16_t data){
    // write this
	  // Consider the following registers:
	  // SYSCTL_RCGCSSI_R, SSI1_CR1_R, SSI1_CPSR_R, SSI1_CR0_R, SSI1_DR_R, SSI1_CR1_R
		// initialize SSI1
		// will need to turn on port d clock
		SYSCTL_RCGCSSI_R |= 0x02;  // activate SSI1
		SYSCTL_RCGCGPIO_R |= 0x08; // activate port D
		while((SYSCTL_PRGPIO_R&0x08)==0){}; // allow time for clock to start

		// toggle RST low to reset; CS low so it'll listen to us
		// SSI0Fss is temporarily used as GPIO
		
		GPIO_PORTD_AFSEL_R |= 0xB;           // enable alt funct on PD 0,1,3
		GPIO_PORTD_DEN_R |= 0xB;             // enable digital I/O on PD 0,1,2
																					// configure PD 0,1,2 as SSI
		GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0xFFFF0F00)+0x00002022;
		GPIO_PORTD_AMSEL_R &= ~0xB;          // disable analog functionality on PA2,3,5
		SSI1_CR1_R &= ~SSI_CR1_SSE;           // disable SSI
		SSI1_CR1_R &= ~SSI_CR1_MS;            // master mode
																					// configure for system clock/PLL baud clock source
		SSI1_CC_R = (SSI1_CC_R&~SSI_CC_CS_M)+SSI_CC_CS_SYSPLL;
	//                                        // clock divider for 3.125 MHz SSIClk (50 MHz PIOSC/16)
	//  SSI0_CPSR_R = (SSI0_CPSR_R&~SSI_CPSR_CPSDVSR_M)+16;
																					// clock divider for 8 MHz SSIClk (80 MHz PLL/24)
																					// SysClk/(CPSDVSR*(1+SCR))
																					// 80/(10*(1+0)) = 8 MHz (slower than 4 MHz)
		SSI1_CPSR_R = (SSI1_CPSR_R&~SSI_CPSR_CPSDVSR_M)+4; // must be even number
		SSI1_CR0_R &= ~(SSI_CR0_SCR_M);      // SCR = 0 (8 Mbps data rate)
										      // SPH = 0
		SSI1_CR0_R &= ~(SSI_CR0_SPH);					//SPH = 0
		SSI1_CR0_R |=	(SSI_CR0_SPO);         // SPO = 1
																					// FRF = TI format
		SSI1_CR0_R = (SSI1_CR0_R&~SSI_CR0_FRF_M)+SSI_CR0_FRF_TI;
																					// DSS = 16-bit data
		SSI1_CR0_R = (SSI1_CR0_R&~SSI_CR0_DSS_M)+SSI_CR0_DSS_16;
		SSI1_CR1_R |= SSI_CR1_SSE;            // enable SSI
		
		while((SSI1_SR_R&SSI_SR_TNF)==0){};   // wait until transmit FIFO not full
		SSI1_DR_R = data;                        // data out

}

// --------------     DAC_Out   --------------------------------------------
// Send data to TLV5616 12-bit DAC
// inputs:  voltage output (0 to 4095)
// 
void DAC_Out(uint16_t code){
    // write this
    // Consider the following registers:
	  // SSI1_SR_R, SSI1_DR_R
		while((SSI1_SR_R&SSI_SR_TNF)==0){};   // wait until transmit FIFO not full
		SSI1_DR_R = code;                        // data out
}

// --------------     DAC_OutNonBlocking   ------------------------------------
// Send data to TLV5616 12-bit DAC without checking for room in the FIFO
// inputs:  voltage output (0 to 4095)
// 
void DAC_Out_NB(uint32_t code){
    // Consider writing this (If it is what your heart desires)
    // Consider the following registers:
	  // SSI1_SR_R, SSI1_DR_R
		SSI1_DR_R = code;                        // data out
}
