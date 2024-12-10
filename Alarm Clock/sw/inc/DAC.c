/**
 * @file DAC.c
 * @author your name (you@domain.com)
 * @brief Low level driver for the DAC. Documentation comments are specifically
 *        for the TLV5618 12-bit SPI DAC. Modify for your particular DAC.
 * @version 0.3.0
 * @date 2023-02-14
 *
 * @copyright Copyright (c) 2023
 * @note Reference datasheet:
 *      https://www.ti.com/lit/ds/symlink/tlv5618a.pdf?ts=1676400608127&ref_url=https%253A%252F%252Fwww.google.com%252F
 */

#include "./DAC.h"
#include "./tm4c123gh6pm.h"

//#define SSI1_CR0_R              (*((volatile uint32_t *)0x40009000))
//#define SSI_CR0_SCR_M           0x0000FF00  // SSI Serial Clock Rate

void DAC_Init() {
    /**
     * Unified_Port_Init in Lab5.c calls Port_D_Init, which initializes the Port
     * D GPIOs for the appropriate alternate functionality (SSI).
     *
     * According to Table 15-1. SSI Signals in the datasheet, this corresponds
     * to SSI1. The corresponding Valvanoware register defines are at L302 and
     * L2670 in inc/tm4c123gh6pm.h. Use this in combination with the datasheet
     * or any existing code to write your driver! An example of how to
     * initialize SSI is found in L741 in inc/ST7735.c.
     */
    // 1. Enable clocks for SSI1 and Port D
    SYSCTL_RCGCSSI_R |= SYSCTL_RCGCSSI_R1;  // SSI1
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R3; // Port D
    
    // 2. Configure PD0-PD3 for SSI1
    GPIO_PORTD_AFSEL_R |= 0x0F;  // Alternate function
    GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R & 0xFFFF0000) + 0x00002222; // Assign SSI
    GPIO_PORTD_DEN_R |= 0x0F;    // Enable digital I/O PD0-3
    GPIO_PORTD_AMSEL_R &= ~0x0F; // Disable analog
    
    // 3. SSI1 configuration
    SSI1_CR1_R = 0x00000000;     // Disable SSI1, master mode
    SSI1_CC_R = 0x0;             // Use system clock
    SSI1_CPSR_R = 0x02;          // Clock prescale divisor
    SSI1_CR0_R = 0x0007;         // 16-bit data, SPI mode 0
    SSI1_CR1_R |= 0x00000002;    // Enable SSI1
}

void DAC_Output(uint16_t data) {
    // An example of how to send data via SSI is found in L534 of inc/ST7735.c.
    // Remember that 4 out of the 16 bits is for DAC operation. The last 12 bits
    // are for data. Read the datasheet! 
    // Ensure data is 12 bits by masking with 0x0FFF (0000 1111 1111 1111)
    data &= 0x0FFF;
    
    // Wait until the transmit FIFO is not full
    while((SSI1_SR_R & SSI_SR_TNF) == 0){};  
    
    // Send the 12-bit data to the DAC via SSI1
    SSI1_DR_R = data;
}
