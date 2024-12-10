// DMATimer.c
// Runs on LM4F120/TM4C123
// Periodic timer triggered DMA transfer
// source: One large byte-aligned memory
//   BlockSize is number of bytes in a block (<=1024)
//   NumBlocks is the number of blocks
//   Memory buffer size is BlockSize*NumBlocks
// destination: Port
//   initialized in main program

// Uses Timer5A to trigger the DMA,
//   read from a large memory Buffer, and
//   then write to an 8-bit PORT
// There is a Timer5A interrupt after the buffer has been transferred
//   In ping-pong mode the ISR sets up the next buffer

// Usage
// 0) configure a GPIO port as output, can use bit-specific addressing
// 1) call DMA_Init once, setting the output period (12.ns) and port address
// 2) call DMA_Transfer to initiate DMA specifying
//    memory buffer address, byte aligned
//    block size and number of blocks
// Jonathan Valvano
// May 2, 2015

/* This example accompanies the book
   "Embedded Systems: Real Time Operating Systems for ARM Cortex M Microcontrollers",
   ISBN: 978-1466468863, Jonathan Valvano, copyright (c) 2015
   Section 6.4.5, Program 6.1

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

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/DMATimerWrite.h"

// functions defined in startup.s
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
int32_t StartCritical(void);  // previous I bit, disable interrupts
void EndCritical(int32_t sr); // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

// The control table used by the uDMA controller.  This table must be aligned to a 1024 byte boundary.
// really two 128-word tables: primary and alternate
// there are 32 channels, we are using channel 8, encoding 3
// each channel has 4 words in the primary table, and another 4 words in the second
uint32_t ucControlTable[256] __attribute__ ((aligned(1024)));
// Timer5A uses uDMA channel 8 encoding 3
#define CH8 (8*4)
#define CH8ALT (8*4+128)

#define BIT8 0x00000100
uint8_t *SourcePt;                 // last address of the source buffer, incremented by 1
volatile uint32_t *DestinationPt;  // fixed address, Port to output
uint32_t BlockSize;                // number of bytes to transmit in each block
uint32_t NumBlocks;                // number of blocks to transmit
uint32_t NumberOfBuffersSent=0;
enum ch8status Status = UNINITIALIZED;

// ***************** Timer5A_Init ****************
// Activate Timer5A trigger DMA periodically
// Inputs:  period in 12.5nsec
// Outputs: none
void Timer4A_Init(uint16_t period){
  SYSCTL_RCGCTIMER_R |= 0x10;      // 0) activate timer5
 // while((SYSCTL_PRTIMER_R&SYSCTL_PRTIMER_R5) == 0){};
  TIMER4_CTL_R = 0x00000000;     // 1) disable timer5A during setup
  TIMER4_CFG_R = 0x00000000;       // 2) configure for 16-bit timer mode
  TIMER4_TAMR_R = 0x00000002;      // 3) configure for periodic mode, default down-count settings
  TIMER4_TAILR_R = period-1;       // 4) reload value
  TIMER4_TAPR_R = 0;               // 5) 12.5ns timer5A
  TIMER4_ICR_R = 0x00000001;       // 6) clear timer5A timeout flag
  TIMER4_IMR_R |= 0x00000001;      // 7) arm timeout interrupt
  NVIC_PRI17_R = (NVIC_PRI17_R&0xFF00FFFF)|0x00000040; // priority 2 
  NVIC_EN2_R = 0x00000040;      // 9) enable interrupt 70 in NVIC
  // vector number 108, interrupt number 92
/*  TIMER5_CTL_R |= 0x00000001;      // 10) enable timer5A when DMA transfer initiates*/
// interrupts enabled in the main program after all devices initialized
}

// ************DMA_Init*****************
// Main program must initialize used for destination
// Source address will be set in DMA_Transfer
// DMA triggered by timer 5A every period bus cycles
// This function needs to be called once before requesting a transfer
// The destination address is fixed, which can be a bit specific addressing
// The source address is incremented each byte
// Inputs:  period in 12.5nsec
//          destination address of output port
// Outputs: none
void DMA_Init(uint16_t period, uint32_t *destination){
  int i;
  DestinationPt = destination;

  for(i=0; i<256; i++){
    ucControlTable[i] = 0;
  }
  SYSCTL_RCGCDMA_R = 0x01;    // µDMA Module Run Mode Clock Gating Control
                              // allow time to finish
  while((SYSCTL_PRDMA_R&SYSCTL_PRDMA_R0) == 0){};
  UDMA_CFG_R = 0x01;          // MASTEN Controller Master Enable
  UDMA_CTLBASE_R = (uint32_t)ucControlTable;
  UDMA_CHMAP1_R = (UDMA_CHMAP1_R&0xFFFFFFF0)|0x00000003;  // timer5A, channel 8, encoding 3
  UDMA_PRIOSET_R |= BIT8;     // use high priority
  UDMA_ALTCLR_R = BIT8;       // use primary control initially
  UDMA_USEBURSTCLR_R = BIT8;  // responds to both burst and single requests
  UDMA_REQMASKCLR_R = BIT8;   // allow the µDMA controller to recognize requests for this channel
  Timer4A_Init(period);
  Status = IDLE;
}

// private function used to reprogram primary channel control structure
void static setRegular(void){
  ucControlTable[CH8]   = (uint32_t)SourcePt+BlockSize-1;  // last address of memory
  ucControlTable[CH8+1] = (uint32_t)DestinationPt;         // fixed address (when DSTINC is 0x3)
  ucControlTable[CH8+2] = 0xC0000003+((BlockSize-1)<<4);   // DMA Channel Control Word (DMACHCTL)
/* DMACHCTL          Bits    Value Description
   DSTINC            31:30   3     8-bit destination address no increment
   DSTSIZE           29:28   0     8-bit destination data size
   SRCINC            27:26   0     1 byte source address increment
   SRCSIZE           25:24   0     8-bit source data size
   reserved          23:18   0     Reserved
   ARBSIZE           17:14   0     Arbitrates after 1 transfer
   XFERSIZE          13:4  count-1 Transfer count items
   NXTUSEBURST       3       0     N/A for this transfer type
   XFERMODE          2:0     011   Use ping-pong transfer mode
  */
//  UDMA_ENASET_R |= BIT8;  // µDMA Channel 8 is enabled.
  // bit 8 in UDMA_ENASET_R become clear when done
  // bits 2:0 ucControlTable[CH8+2] become clear when done
  SourcePt = SourcePt+BlockSize;
}
// private function used to reprogram alternate channel control structure
void static setAlternate(void){
  ucControlTable[CH8ALT]   = (uint32_t)SourcePt+BlockSize-1;  // last address of memory
  ucControlTable[CH8ALT+1] = (uint32_t)DestinationPt;         // fixed address (when DSTINC is 0x3)
  ucControlTable[CH8ALT+2] = 0xC0000003+((BlockSize-1)<<4);   // DMA Channel Control Word (DMACHCTL)
/* DMACHCTL          Bits    Value Description
   DSTINC            31:30   11    no destination address increment
   DSTSIZE           29:28   00    8-bit destination data size
   SRCINC            27:26   00    8-bit source address increment
   SRCSIZE           25:24   00    8-bit source data size
   reserved          23:18   0     Reserved
   ARBSIZE           17:14   0     Arbitrates after 1 transfer
   XFERSIZE          13:4  count-1 Transfer count items
   NXTUSEBURST       3       0     N/A for this transfer type
   XFERMODE          2:0     011   Use ping-pong transfer mode
  */
  SourcePt = SourcePt+BlockSize;
}
// ************DMA_Transfer*****************
// Called to transfer bytes from source to destination
// The source address is incremented each byte, destination address is fixed
// Inputs:  source is a pointer to the memory buffer containing 8-bit data,
//          blocksize is the size of each block (max is 2048 bytes) (must be even)
//          numblocks is the number of blocks
// Outputs: none
// This routine does not wait for completion
void DMA_Transfer(uint8_t *source, uint32_t blocksize,
                   uint32_t numblocks){
  DisableInterrupts();    // some or all of this initialization is a critical section
  Status = BUSY;
  BlockSize = blocksize/2;// number of bytes to transmit
  NumBlocks = 2*numblocks;
  NumberOfBuffersSent = 0;
  SourcePt = source;      // address of source buffer
  setRegular();
  setAlternate();
  NVIC_EN2_R = 0x10000000;         // 9) enable interrupt 92 in NVIC
  // vector number 108, interrupt number 92
  TIMER4_CTL_R = 0x00000001;    // 10) enable TIMER4A
  UDMA_ENASET_R |= BIT8;  // µDMA Channel 8 is enabled
  EnableInterrupts();     // re-enable interrupts and be ready for ping-pong interrupt
}

// ************DMA_Stop*****************
// Stop the transfer bytes from source to destination
// Inputs:  none
// Outputs: none
void DMA_Stop(void){
  UDMA_ENACLR_R = BIT8;        // µDMA Channel 8 is disabled
  NVIC_DIS2_R = 0x10000000;    // 9) disable interrupt 92 in NVIC
  TIMER4_CTL_R &= ~0x00000001; // 10) disable timer5A
  Status = IDLE;
}

void Timer4A_Handler(void){ // interrupts after each block is transferred
  TIMER4_ICR_R = TIMER_ICR_TATOCINT; // acknowledge timer4A timeout
/*  NumberOfBuffersSent++;
  if(NumberOfBuffersSent < NumBlocks){
    if((ucControlTable[CH8+2]&0x0007)==0){     // regular buffer complete
      setRegular();                            // rebuild channel control structure
    }
    if((ucControlTable[CH8ALT+2]&0x0007)==0){  // Alternate buffer complete
      setAlternate();                          // rebuild channel control structure
    }
  }else{
    DMA_Stop();
  }*/
  if((ucControlTable[CH8+2]&0x0007) == 0){     // regular buffer complete
    NumberOfBuffersSent++;                     // increment counter
    if(NumberOfBuffersSent < (NumBlocks - 1)){ // still more data to send
      setRegular();                            // rebuild channel control structure
    }
  }
  if((ucControlTable[CH8ALT+2]&0x0007) == 0){  // alternate buffer complete
    NumberOfBuffersSent++;                     // increment counter
    if(NumberOfBuffersSent < (NumBlocks - 1)){ // still more data to send
      setAlternate();                          // rebuild channel control structure
    }
  }
  if(((ucControlTable[CH8+2]&0x0007) == 0) &&
     ((ucControlTable[CH8ALT+2]&0x0007) == 0)){// both buffers complete and not re-enabled above
    DMA_Stop();
  }
}

// ************DMA_Status*****************
// Return the status of the DMA Channel 8 module in an
// enumerated type.
// Inputs:  none
// Outputs: enumerated type status of the DMA Channel 8
enum ch8status DMA_Status(void){
  return Status;
}











// DMAtestmain.c
// Runs on LM4F120/TM4C123
// Uses Timer4A to trigger the DMA,
//   read from a large memory Buffer, and
//   then write to an 8-bit PORT
// There is a Timer5A interrupt after the buffer has been transferred
//   In ping-pong mode the ISR sets up the next buffer
// Jonathan Valvano
// May 3, 2015


// this is the start of the main test for the DMA from valv war just putting it in the driver inc file 




// Blink the on-board LED.
#define GPIO_PORTE0 (*((volatile uint32_t *)0x40024004))
#define PF0       (*((volatile uint32_t *)0x40025004))
#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
#define LEDS      (*((volatile uint32_t *)0x40025038))
#define RED       0x02
#define BLUE      0x04
#define GREEN     0x08
#define PF4       (*((volatile uint32_t *)0x40025040))
#define PF5       (*((volatile uint32_t *)0x40025080))
#define PF6       (*((volatile uint32_t *)0x40025100))
#define PF7       (*((volatile uint32_t *)0x40025200))
const int32_t COLORWHEEL[8] = {RED, RED+GREEN, GREEN, GREEN+BLUE, BLUE, BLUE+RED, RED+GREEN+BLUE, 0};
#define NUMLEDS  50        // number of LEDs in the string
#define SIZE 3*8*4*NUMLEDS // size of color encoding buffer
#define PINGPONGS 3        // number of DMA ping-pong transfers needed to send the whole encoding buffer ((SIZE/PINGPONGS/2)<=1024 and an integer)
// Each LED has three 8-bit color values stored in the order:
// [Green]  [Red]  [Blue]
// With each bit of each byte of each color of each LED stored
// in four bytes.  These four bytes encode a 0 as:
// {0x01, 0x00, 0x00, 0x00}
// and a 1 as:
// {0x01, 0x01, 0x00, 0x00}
// Note: These values are for LED string on GPIO pin 0.
// In other words, there are NUMLEDS LEDs per string of lights,
// 3 colors (green, red, blue) per LED, 8 bits per color, and
// 4 bytes per bit of color.
uint8_t Buffer[SIZE];      // the color encoding buffer
uint32_t Cursor;           // pointer into Buffer (0 to SIZE-1)

// delay function from sysctl.c
// which delays 3*ulCount cycles
//#ifdef __TI_COMPILER_VERSION__
  //Code Composer Studio Code
  void Delayrgb(uint32_t ulCount){
    __asm (  "loop:    subs    r0, #1\n"
      "    bne     loop\n");
}

//------------sendreset------------
// Send a reset command to the WS2812 LED driver through PE0.
// This function uses a blind wait, so it takes at least 50
// usec to complete.
// Input: none
// Output: none
// Assumes: 80 MHz system clock, PE0 initialized
void sendreset(void){
  GPIO_PORTE0 = 0x00;              // hold data low
  Delayrgb(1333);                     // delay ~4,000 cycles (50 usec)
}

//------------clearbuffer------------
// Clear the entire RAM buffer and restart the cursor to
// the beginning of the row of LEDs.  To actually update the
// physical LEDs, initiate a DMA transfer from 'Buffer' to
// GPIO_PORTE0 (0x40024004).
// Input: none
// Output: none
void clearbuffer(void){
  int i;
  Cursor = 0;                      // restart at the beginning of the buffer
  for(i=0; i<SIZE; i=i+4){
    Buffer[i] = 1;                 // clear all color values for all LEDs by encoding 0's
    Buffer[i+1] = 0;
    Buffer[i+2] = 0;
    Buffer[i+3] = 0;
  }
}

//------------addcolor------------
// Private helper function that adds a color to the buffer.
// Configure the next LED with the desired color values and
// move to the next LED automatically.  This updates the RAM
// buffer and increments the cursor.  To actually update the
// physical LEDs, initiate a DMA transfer from 'Buffer' to
// GPIO_PORTE0 (0x40024004).  A color value of zero means
// that color is not illuminated.
// Input: red   8-bit red color value
//        green 8-bit green color value
//        blue  8-bit blue color value
// Output: none
void addcolor(uint8_t red, uint8_t green, uint8_t blue){
  int i;
  for(i=0x80; i>0x00; i=i>>1){     // store the green byte first
    if(green&i){
      Buffer[Cursor] = 1;
      Buffer[Cursor+1] = 1;
      Buffer[Cursor+2] = 0;
      Buffer[Cursor+3] = 0;
    } else{
      Buffer[Cursor] = 1;
      Buffer[Cursor+1] = 0;
      Buffer[Cursor+2] = 0;
      Buffer[Cursor+3] = 0;
    }
    Cursor = Cursor + 4;
  }
  for(i=0x80; i>0x00; i=i>>1){     // store the red byte second
    if(red&i){
      Buffer[Cursor] = 1;
      Buffer[Cursor+1] = 1;
      Buffer[Cursor+2] = 0;
      Buffer[Cursor+3] = 0;
    } else{
      Buffer[Cursor] = 1;
      Buffer[Cursor+1] = 0;
      Buffer[Cursor+2] = 0;
      Buffer[Cursor+3] = 0;
    }
    Cursor = Cursor + 4;
  }
  for(i=0x80; i>0x00; i=i>>1){     // store the blue byte third
    if(blue&i){
      Buffer[Cursor] = 1;
      Buffer[Cursor+1] = 1;
      Buffer[Cursor+2] = 0;
      Buffer[Cursor+3] = 0;
    } else{
      Buffer[Cursor] = 1;
      Buffer[Cursor+1] = 0;
      Buffer[Cursor+2] = 0;
      Buffer[Cursor+3] = 0;
    }
    Cursor = Cursor + 4;
  }
  if(Cursor >= SIZE){              // check if at the end of the buffer
    Cursor = 0;                    // wrap back to the beginning
  }
}

//------------setcursor------------
// Point the cursor at the desired LED index.  This LED will
// be the next one configured by the next call to addcolor().
// If the new cursor parameter is greater than the total
// number of LEDs, it will wrap around.  For example,
// setcursor(11) with a row of 10 LEDs will wrap around to the
// second (index=1) LED.
// Input: newCursor 8-bit new cursor index
// Output: none
void setcursor(uint8_t newCursor){
  newCursor = newCursor%(SIZE/3/8/4);
  Cursor = newCursor*3*8*4;
}

int somergbtest(void){
  uint8_t phase;                   // phase value of the rainbow LED [0 to 8] or [0 to 44]
  uint8_t red, green, blue;        // value of each color
  int8_t ired, igreen, iblue;      // amount to increment each color
  int i, t;
  
                                   // activate clock for Port E
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
                                   // allow time for clock to stabilize
  while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R4) == 0){};
  GPIO_PORTE_DIR_R |= 0x01;        // make PE0 out
  GPIO_PORTE_AFSEL_R &= ~0x01;     // disable alt funct on PE0
  GPIO_PORTE_DR8R_R |= 0x01;       // enable 8 mA drive on PE0
  GPIO_PORTE_DEN_R |= 0x01;        // enable digital I/O on PE0
                                   // configure PE0 as GPIO
  GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFFFFFF0)+0x00000000;
  GPIO_PORTE_AMSEL_R &= ~0x01;     // disable analog functionality on PE0
  sendreset();
  SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R5; // enable Port F clock
                                   // allow time for clock to stabilize
  while((SYSCTL_PRGPIO_R&SYSCTL_PRGPIO_R5) == 0){};
                                   // unlock GPIO Port F Commit Register
  GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
  GPIO_PORTF_CR_R = 0x01;          // enable commit for PF0
  GPIO_PORTF_DIR_R = 0x0E;         // make PF3-1 output (PF3-1 built-in LEDs)
  GPIO_PORTF_AFSEL_R &= ~0xFF;     // disable alt funct on PF7-0
  GPIO_PORTF_PUR_R |= 0x11;        // enable weak pull-up on PF0 and PF4
  GPIO_PORTF_DEN_R |= 0xFF;        // enable digital I/O on PF7-0
  GPIO_PORTF_PCTL_R = 0;           // all pins are regular I/O
  GPIO_PORTF_AMSEL_R = 0;          // disable analog functionality on Port F
  LEDS = 0;                        // turn all LEDs off
  DMA_Init(32, (uint32_t *)&GPIO_PORTE0); // initialize DMA channel 8 for Timer5A transfer, every 24 cycles (0.4 usec)
  EnableInterrupts();              // Timer5A interrupt on completion
  phase = 0;
  setcursor(0);
  while(1){
/* Beginning of 10 LED test */
/*    if(phase < 3){
      red = 255 - 85*phase;
      green = 85*phase;
      blue = 0;
      ired = -85;
      igreen = 85;
      iblue = 0;
    } else if(phase < 6){
      red = 0;
      green = 255 - 85*(phase - 3);
      blue = 85*(phase - 3);
      ired = 0;
      igreen = -85;
      iblue = 85;
    } else{
      red = 85*(phase - 6);
      green = 0;
      blue = 255 - 85*(phase - 6);
      ired = 85;
      igreen = 0;
      iblue = -85;
    }
    phase = phase + 1;
    if(phase > 8){
      phase = 0;
    }
    for(i=0; i<9; i=i+1){
      addcolor(red, green, blue);
      red = red + ired;
      green = green + igreen;
      blue = blue + iblue;
      if((red == 255) && (ired > 0)){
        igreen = ired;
        ired = -1*ired;
      }
      if((red == 0) && (ired < 0)){
        ired = 0;
      }
      if((green == 255) && (igreen > 0)){
        iblue = igreen;
        igreen = -1*igreen;
      }
      if((green == 0) && (igreen < 0)){
        igreen = 0;
      }
      if((blue == 255) && (iblue > 0)){
        ired = iblue;
        iblue = -1*iblue;
      }
      if((blue == 0) && (iblue < 0)){
        iblue = 0;
      }
    }
    addcolor(31*phase, 31*phase, 31*phase);*/
/* End of of 10 LED test */
/* Beginning of 50 LED test */
    addcolor(5*phase, 5*phase, 5*phase);
    addcolor(0, 0, 0);
    addcolor(0, 0, 0);
    addcolor(0, 0, 0);
    addcolor(0, 0, 0);
    if(phase < 15){
      red = 240 - 16*phase;
      green = 16*phase;
      blue = 0;
      ired = -16;
      igreen = 16;
      iblue = 0;
    } else if(phase < 30){
      red = 0;
      green = 240 - 16*(phase - 15);
      blue = 16*(phase - 15);
      ired = 0;
      igreen = -16;
      iblue = 16;
    } else{
      red = 16*(phase - 30);
      green = 0;
      blue = 240 - 16*(phase - 30);
      ired = 16;
      igreen = 0;
      iblue = -16;
    }
    phase = phase + 1;
    if(phase > 44){
      phase = 0;
    }
    for(i=0; i<45; i=i+1){
      addcolor(red, green, blue);
      red = red + ired;
      green = green + igreen;
      blue = blue + iblue;
      if((red == 240) && (ired > 0)){
        igreen = ired;
        ired = -1*ired;
      }
      if((red == 0) && (ired < 0)){
        ired = 0;
      }
      if((green == 240) && (igreen > 0)){
        iblue = igreen;
        igreen = -1*igreen;
      }
      if((green == 0) && (igreen < 0)){
        igreen = 0;
      }
      if((blue == 240) && (iblue > 0)){
        ired = iblue;
        iblue = -1*iblue;
      }
      if((blue == 0) && (iblue < 0)){
        iblue = 0;
      }
    }
/* End of 50 LED test */
    sendreset();         // send a reset
    DMA_Transfer((uint8_t *)Buffer, SIZE/PINGPONGS, PINGPONGS);
    LEDS = COLORWHEEL[t&0x07];
    t = t + 1;
    Delayrgb(2000000);
    while(DMA_Status() != IDLE){// takes SIZE*0.4 microseconds to complete
    }
  }
}
