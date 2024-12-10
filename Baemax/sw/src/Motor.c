
#include "../inc/tm4c123gh6pm.h"
#include "../inc/CortexM.h"
#include <stdint.h>

/************ MotorDriver_Init ************/
/*
Pin Mappings: 
PWMA -> PF2
PWMB -> PF3
AIN1 -> PA2
AIN2 -> PA5
BIN1 -> PF0
BIN2 -> PF1
STBY -> PD6
*/
void MotorDriver_Init(void){
	// Turn on Clock and PWM Module 1
	SYSCTL_RCGCGPIO_R |= 0x29; 											// check to make sure this is Ports A, D, and F!!!!!
	SYSCTL_RCGCPWM_R |= 0x02;
	while ((SYSCTL_PRGPIO_R & 0x29) == 0) {}
		
	// Set PA2 and PA5 for AIN1 and AIN2 
	GPIO_PORTA_DIR_R |= 0x24;
	GPIO_PORTA_DEN_R |= 0x24;
		
	GPIO_PORTF_LOCK_R = 0x4C4F434B;  // Unlock Port F
	GPIO_PORTF_CR_R |= 0x01;         // Allow changes to PF0
	//GPIO_PORTF_AMSEL_R &= ~0x01; // Disable analog mode
	//GPIO_PORTF_AFSEL_R &= ~0x01; // Disable alternate functions
	//GPIO_PORTF_PCTL_R &= ~0x0000000F; // Configure as GPIO
	GPIO_PORTF_PUR_R |= 0x01;  // Enable pull-up for PF0
		
	// Set PF0 and PF1 for BIN1 and BIN2
	GPIO_PORTF_DIR_R |= 0x03;
	GPIO_PORTF_DEN_R |= 0x03; 

	// Set PD6 as output for STBY
	GPIO_PORTD_DIR_R |= 0x40;
	GPIO_PORTD_DEN_R |= 0x40;
	
	// Set PF2 and PF3 for PWMA and PWMB
	GPIO_PORTF_AFSEL_R |= 0x0C;     			// enable alt funct on PF3 PF2
  GPIO_PORTF_PCTL_R = (GPIO_PORTF_PCTL_R&0xFFFFF00FF)|0x00005500;
    // PF2 is M1PWM6
    // PF3 is M1PWM7
  GPIO_PORTF_DIR_R |= 0x0C;             // PF3 PF2 output
  GPIO_PORTF_DEN_R |= 0x0C;             // enable digital I/O on PF3 PF2
  GPIO_PORTF_DR8R_R |= 0x0C;            // high current output on PF3 PF2
    // bit 20=1 for USEPWMDIV
    // bits 19-17 = 0 for divide by 2
    // bits 19-17 = n for divide by 2*(2^n)
    // bits 19-17 = 3 for divide by 16
    // bits 19-17 = 5 for divide by 64
  SYSCTL_RCC_R = 0x00120000 |           // 3) use PWM divider 
      (SYSCTL_RCC_R & (~0x000E0000));   //    configure for /4 divider
  PWM1_3_CTL_R = 0;                     // 4) re-loading down-counting mode
	
  PWM1_3_GENA_R = 0xC8;                 // low on LOAD, high on CMPA down
  // PF2 goes low on LOAD
  // PF2 goes high on CMPA down
  PWM1_3_LOAD_R = 40000 - 1;    				// 5) cycles needed to count down to 0
  PWM1_3_CMPA_R = 0;                  	// 6) count value when output rises
    
  PWM1_3_GENB_R = 0xC08;               	// low on LOAD, high on CMPB down
  // PF3 goes low on LOAD
  // PF3 goes high on CMPB down  
  PWM1_3_CMPB_R = 0;                   	// 6) count value when output rises
  PWM1_3_CTL_R |= 0x00000001;           // 7) start PWM1
  PWM1_ENABLE_R |= 0x000000C0;          // enable PF2 is M1PWM6 and PF3 is M1PWM7

}

// Initialize SysTick with busy wait running at bus clock.
void SysTick_Init(void){
    // ECE319K students write this function
		NVIC_ST_CTRL_R = 0; // 1) disable SysTick during setup 
		NVIC_ST_RELOAD_R = 0x00FFFFFF; // 2) maximum reload value 
		NVIC_ST_CURRENT_R = 0; // 3) any write to CURRENT clears it 
		NVIC_ST_CTRL_R = 0x00000005; // 4) enable SysTick with core clock

}
// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)
void SysTick_Wait(uint32_t delay){
  // ECE319K students write this function
	NVIC_ST_RELOAD_R = delay-1;  // number of counts
  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears
  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for COUNT flag
  }

}
// 1000us equals 1ms
void SysTick_Wait1ms(uint32_t delay){
    // ECE319K students write this function
		for(uint32_t i=0; i<delay; i++){
		SysTick_Wait(80000); // wait 1 ms
		}
}

void MotorStop(char select){
	if(select == 'A'){
		PWM1_3_CMPA_R = 0;
	}else if(select == 'B'){
		PWM1_3_CMPB_R = 0;
	}else{
		PWM1_3_CMPA_R = 0;
		PWM1_3_CMPB_R = 0;
	}
	SysTick_Wait1ms(1000);       
}

void MotorForward(char select, uint32_t duty, uint32_t delay){
	if(select == 'A'){
		GPIO_PORTA_DATA_R |= 0x04;  	// Set PA2 (AIN1) high
		GPIO_PORTA_DATA_R &= ~0x20; 	// Set PA5 (AIN2) low
		PWM1_3_CMPA_R = duty;      		// Set PWMA duty cycle
	}else if(select == 'B'){
		GPIO_PORTF_DATA_R |= 0x01;  	// Set PF0 (BIN1) high
    GPIO_PORTF_DATA_R &= ~0x02; 	// Set PF1 (BIN2) low
    PWM1_3_CMPB_R = duty;      		// Set PWMB duty cycle
	}else{
		GPIO_PORTA_DATA_R |= 0x04;  	// Set PA2 (AIN1) high
		GPIO_PORTA_DATA_R &= ~0x20; 	// Set PA5 (AIN2) low
		PWM1_3_CMPA_R = duty;      		// Set PWMA duty cycle
		GPIO_PORTF_DATA_R |= 0x01;  	// Set PF0 (BIN1) high
    GPIO_PORTF_DATA_R &= ~0x02; 	// Set PF1 (BIN2) low
    PWM1_3_CMPB_R = duty;      		// Set PWMB duty cycle
	}
	SysTick_Wait1ms(delay);
	MotorStop(select);
}

void MotorBackward(char select, uint32_t duty, uint32_t delay){
	if(select == 'A'){
		GPIO_PORTA_DATA_R &= ~0x04; 	// Set PA2 (AIN1) low
    GPIO_PORTA_DATA_R |= 0x20;  	// Set PA3 (AIN2) high
    PWM1_3_CMPA_R = duty;       	// Set PWMA duty cycle
	}else if(select == 'B'){
		GPIO_PORTF_DATA_R &= ~0x01; 	// Set PF0 (BIN1) low
    GPIO_PORTF_DATA_R |= 0x02;  	// Set PF1 (BIN2) high
    PWM1_3_CMPB_R = duty;       	// Set PWMB duty cycle
	}else{
		GPIO_PORTA_DATA_R &= ~0x04; 	// Set PA2 (AIN1) low
    GPIO_PORTA_DATA_R |= 0x20;  	// Set PA3 (AIN2) high
    PWM1_3_CMPA_R = duty;       	// Set PWMA duty cycle
		GPIO_PORTF_DATA_R &= ~0x01; 	// Set PF0 (BIN1) low
    GPIO_PORTF_DATA_R |= 0x02;  	// Set PF1 (BIN2) high
    PWM1_3_CMPB_R = duty;       	// Set PWMB duty cycle
	}
	SysTick_Wait1ms(delay);
	MotorStop(select);
}

void MotorSequenceTest(void) {
	// Activate motor driver
	GPIO_PORTD_DATA_R |= 0x40; 				// Setting STBY to high to turn on motor driver

	// Motor A Test
	MotorForward('A', 12000, 2000);		// Test 1: Direction: Forward, Duty Cycle: 75%, Duration: 2s
	MotorBackward('A', 8000, 2000);		// Test 2: Direction: Backward, Duty Cycle: 50%, Duration: 2s

	// Motor B Test
	MotorForward('B', 12000, 2000);		// Test 3: Direction: Forward, Duty Cycle: 75%, Duration: 2s
  MotorBackward('B', 8000, 2000);		// Test 4: Direction: Backward, Duty Cycle: 50%, Duration: 2s

	// Deactivate motor driver
  GPIO_PORTD_DATA_R &= ~0x40; 			// Seting STBY to low to turn off motor driver
}

void RightMotorHi(void){
	GPIO_PORTD_DATA_R |= 0x40;
	MotorForward('A', 30000, 1000);
	MotorBackward('A', 30000, 1000);
	GPIO_PORTD_DATA_R &= ~0x40;
}


void FlurryFight(void){
	GPIO_PORTD_DATA_R |= 0x40;
	MotorForward('C', 36000, 2000);
	// calibrate MotorBackward if needed
	GPIO_PORTD_DATA_R &= ~0x40;
}

void RightPunch(void){
	GPIO_PORTD_DATA_R |= 0x40;
	MotorForward('A', 36000, 2000);
	MotorBackward('A', 3600, 1000);
	GPIO_PORTD_DATA_R &= ~0x40;
}

void LeftPunch(void){
	GPIO_PORTD_DATA_R |= 0x40;
	MotorForward('B', 36000, 2000);
	MotorBackward('B', 3600, 1000);
	GPIO_PORTD_DATA_R &= ~0x40;
}

void Hug4U(void){
	GPIO_PORTD_DATA_R |= 0x40;
	MotorForward('C', 25000, 1000);
	MotorBackward('C', 25000, 1000);
	GPIO_PORTD_DATA_R &= ~0x40;
}



