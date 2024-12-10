// ----------------------------------------------------------------------------
//
// File name: MQTT.c
//
// Description: This code is used to bridge the TM4C123 board and the MQTT Web Application
//              via the ESP8266 WiFi board

// Authors:       Mark McDermott
// Orig gen date: June 3, 2023
// Last update:   July 21, 2023
//
// ----------------------------------------------------------------------------


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "PLL.h"
#include "../src/SDCFile.h"
#include "UART.h"
#include "UART5.h"
#include "UART2.h"
#include "esp8266.h"
#include "../src/Motor.h"
//#include "../src_TM4C/MotorControl.h"
//#include "../src_TM4C/Tachometer.h"

#define DEBUG1                // First level of Debug
//#undef  DEBUG1                // Comment out to enable Debug1

#define UART5_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define UART5_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define UART5_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define UART5_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define UART5_CTL_UARTEN         0x00000001  // UART Enable

#define     UART2_FR_TXFF            0x00000020  // UART Transmit FIFO Full
#define     UART2_FR_RXFE            0x00000010  // UART Receive FIFO Empty
#define     UART2_LCRH_WLEN_8        0x00000060  // 8 bit word length
#define     UART2_LCRH_FEN           0x00000010  // UART Enable FIFOs
#define     UART2_CTL_UARTEN         0x00000001  // UART Enable

// ----   Function Prototypes not defined in header files  ------
// 
void EnableInterrupts(void);    // Defined in startup.s

// ----------   VARIABLES  ----------------------------

//extern uint32_t         Kp1; //Extern for any variables we need to set/get for MQTT code
//extern uint32_t         Kp2;
//extern uint32_t         Ki1;
//extern uint32_t         Ki2;
//extern uint32_t         Xstar;
uint32_t example_left_motor_speed;

//Buffers for send / recv
char                    input_char;
char                    b2w_buf[64];

char                    w2b_buf[128];
static uint32_t         bufpos          = 0;



// --------------------------     W2B Parser      ------------------------------
//
// This parser decodes and executes commands from the MQTT Web Appication 
//
void Parser(void) {
  //Perform operations on the w2b buffer to set variables as appropriate
	//atoi() and strtok() may be useful
	
	char first_token[8];
	strcpy(first_token, strtok(w2b_buf, ","));
	
	#ifdef DEBUG1
		UART_OutString("Token 1: ");
		UART_OutString(first_token);
		UART_OutString("\r\n");
	#endif
}
  
// -----------------------  TM4C_to_MQTT Web App -----------------------------
// This routine publishes clock data to the
// MQTT Web Application via the MQTT Broker
// The data is sent using CSV format:
//
// ----------------------------------------------------------------------------
//    
//    Convert this routine to use a FIFO
//
// 
void TM4C_to_MQTT(void){
	
	
  //TODO: Add other commands here
	/*
  sprintf(b2w_buf, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\n", MC_GetLeftU(),MC_GetRightU(),
	MC_GetLeftE(), MC_GetRightE(), GetLeftRPM(), GetRightRPM(), MC_GetKp1(), MC_GetKp2(),
	MC_GetKi1(), MC_GetKi2(), MC_GetDesiredSpeed());  // Msg is comma seperated, and must end with '\n' 
*/
	
	sprintf(b2w_buf, "1,2,3,4,5,6,7,8,9,10,11,\n");
	//b2w_buf = "1,2,3,4,5,6,7,8,9,10,11\n";
  
  if((UART5_FR_R & UART5_FR_TXFF) ==0 ){
		UART5_OutString(b2w_buf); 
	}     

  #ifdef DEBUG1
   UART_OutString("B2W: ");
   UART_OutString(b2w_buf);         
   UART_OutString("\r\n"); 
  #endif 
}
 
// -------------------------   MQTT_to_TM4C  -----------------------------------
// This routine receives the command data from the MQTT Web App and parses the
// data and feeds the commands to the TM4C.
// -----------------------------------------------------------------------------
//
//    Convert this routine to use a FIFO
//
// 
void MQTT_to_TM4C(void) {
    
  if ((UART5_FR_R & UART5_FR_RXFE) ==0 ) {  // Check to see if a there is data in the RXD buffer
		
			input_char =(UART5_DR_R & 0xFF);      // Read the data from the UART
		
			#ifdef DEBUG1
					UART_OutChar(input_char);         // Debug only
				#endif  
		
			/*if (input_char !='\n'){
				if(input_char=='A'){
									
				}
			
				}
		*/
        
      if (input_char != '\n'){              // If the incoming character is not a newline then process byte             
        
				w2b_buf[bufpos] = input_char;       // The buffer position in the array get assigned to the current read
        bufpos++;                           // Once that has happend the buffer advances,doing this over and over
                                            // again until the end of package marker is read.      
      } else if (input_char == '\n'){
				w2b_buf[bufpos] = input_char;
				switch(w2b_buf[0]){
					case('A') : 
						//multipunch
						ST7735_FillScreen(ST7735_BLACK);
						SDLCD("Images/mpo.txt");
						FlurryFight();
						break;
					case('B') : 
						//left punch
						ST7735_FillScreen(ST7735_BLACK);
						//SDLCD("Images/hit.txt");
						SDLCD("Images/hitoutp.txt");
						LeftPunch();

						break;
					case('C') : 
						//right punch
						//ST7735_FillScreen(ST7735_BLACK);
					//	SDLCD("Images/hit.txt");
						SDLCD("Images/hitoutp.txt");
						RightPunch();
						break;
					case('D') :
						//happy
					//	ST7735_FillScreen(ST7735_BLACK);
						SDLCD("Images/lolli.txt");
						ST7735_DrawString(0,0,"You've been good",ST7735_RED);
						ST7735_DrawString(0,12,"Here is a lollipop",ST7735_RED);
						Hug4U();
						break;
					case('E') : 
						//sad
						ST7735_FillScreen(ST7735_BLACK);
						SDLCD("Images/heart.txt");
						ST7735_DrawString(0,0,"",ST7735_RED);
						ST7735_DrawString(0,12,"Would you like a hug?",ST7735_RED);
						Hug4U();
						break;
					case('F') :
						//angry						
						ST7735_FillScreen(ST7735_BLACK);
						SDLCD("Images/wtrfall.txt");
						ST7735_DrawString(0,0,"It'll be okay princess <3",ST7735_RED);
						ST7735_DrawString(0,0,"Look at this waterfall...",ST7735_RED);
						Hug4U();
						break;
				case('G') :
						//surprised
					//	ST7735_FillScreen(ST7735_BLACK);
				//		SDLCD("Images/ekg.txt");
					//	ST7735_DrawString(0,0,"Your neurotransmitter levels are elevated.",ST7735_RED);
					//	ST7735_DrawString(0,0,"Your\nneurotransmitter\nlevels are elevated.",ST7735_RED);
						break;
					case('H') :
						//time
						break;
					case('I') :
						//weather
						break;
					case('J') :
						//RETURN, any type from any menu
						SDLCD("Images/tdmode.txt");
						break;
				//	default:
						//	SDLCD("Images/tdmode.txt");
					//		break;
				}
        bufpos = 0;                      // Reset for next string	
      }   
			
  } 
	
  
}   // End of routine

void UART2_Handler(void){
	while((UART2_FR_R & UART2_FR_RXFE) ==0){
		input_char =(UART2_DR_R & 0xFF);      // Read the data from the UART
		
			#ifdef DEBUG1
					UART_OutChar(input_char);         // Debug only
				#endif  
	}
}
