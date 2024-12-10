

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
    // write this
  

#define PC56  0x60
#define PE4  0x10
#define PB23 0x0C
void Switch_Init(void){
	
		int delay;

    SYSCTL_RCGCGPIO_R |= 0x04;  	// Enable Port C clock
		
   // while((SYSCTL_PRGPIO_R & 0x10) == 0){}; // Wait for clock
		delay = SYSCTL_RCGCGPIO_R;
    GPIO_PORTC_DEN_R |= PC56;     // Enable digital I/O for PC5 and PC6
		GPIO_PORTC_DIR_R &= ~PC56;   	// Set PC56 as inputs
    GPIO_PORTC_PUR_R |= PC56;     // Enable pull-up resistors on PC5 and PC6
		
	
	//-------------------------------- INITIALIZE LED ------------------------------
	  SYSCTL_RCGCGPIO_R |= 0x02;  	// Enable Port B clock
		delay = SYSCTL_RCGCGPIO_R;
		GPIO_PORTC_DEN_R |= PB23;     // Enable digital I/O for Pb2 and pb3
		GPIO_PORTC_DIR_R |= PB23;   	// Set Pb23 as outputs
	
	
	
	
	// ------------------------------- INITIALIZE ADC PIN -------------------------------------
	// PE4
		SYSCTL_RCGCADC_R |= 0x0001;   // 1) activate ADC0
		SYSCTL_RCGCGPIO_R |= 0x10;  	// Enable Port E clock
		while((SYSCTL_PRGPIO_R&0x10) != 0x10){};  // 3 for stabilization
		delay = SYSCTL_RCGCGPIO_R;
		GPIO_PORTE_DEN_R &= ~0x10;    // 6) disable digital I/O on PE4
		GPIO_PORTC_DIR_R &= ~PE4;   	// Set PE3 as inputs	
	  GPIO_PORTE_AFSEL_R |= 0x10;   // 5) enable alternate function on PE4
		GPIO_PORTE_AMSEL_R |= 0x10;   // 7) enable analog functionality on PE4
		ADC0_PC_R &= ~0xF;
		ADC0_PC_R |= 0x1;             // 8) configure for 125K samples/sec
		ADC0_SSPRI_R = 0x0123;        // 9) Sequencer 3 is highest priority
		ADC0_ACTSS_R &= ~0x0008;      // 10) disable sample sequencer 3
		ADC0_EMUX_R &= ~0xF000;       // 11) seq3 is software trigger
		ADC0_SSMUX3_R &= ~0x000F;
		ADC0_SSMUX3_R += 9;           // 12) set channel
		ADC0_SSCTL3_R = 0x0006;       // 13) no TS0 D0, yes IE0 END0
		ADC0_IM_R &= ~0x0008;         // 14) disable SS3 interrupts
		ADC0_ACTSS_R |= 0x0008;       // 15) enable sample sequencer 3
}
