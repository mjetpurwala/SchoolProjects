#include <stdio.h>
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/LaunchPad.h"
#include "../inc/PLL.h"
#include "../inc/ADCSWTrigger.h"
#include "../inc/esp8266.h"
#include "../inc/MQTT.h"
#include "../inc/SSD2119.h"
#include "../inc/eDisk.h"
#include "../inc/Timer0A.h"
#include "../inc/Timer1A.h"
#include "../inc/Timer2A.h"
#include "../inc/Timer3A.h"
#include "../inc/TLV5616.h"
#include "Sound.h"
#include "Motor.h"
#include "SDCFile.h"
#include "../inc/ST7735.h"
#include "SD-LCD.h"
#include "../inc/UART.h"
#include "../inc/UART5.h"
#include "../inc/esp8266.h"


//----- Prototypes of functions in startup.s  ----------------------
//
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // Go into low power mode


// -----------------------------------------------------------------
// -------------------- MAIN LOOP ----------------------------------
//


int main(void){
DisableInterrupts();        // Disable interrupts until finished with inits
  PLL_Init(Bus80MHz);         // Bus clock at 80 MHz
	SysTick_Init();
	MotorDriver_Init();
	//Sound_Init();
//SDshenanigans(); 
	UART_Init();                // Enable Debug Serial Port
  UART5_Init(9600);               // Enable ESP8266 Serial Port
	ST7735_InitR(INITR_REDTAB);
	Reset_8266();               // Reset the WiFi module
  SetupWiFi();                // Setup communications to MQTT Broker via 8266 WiFi
	Timer2A_Init(&MQTT_to_TM4C, 400000/5, 3);         // Get data every 5ms 
  Timer3A_Init(&TM4C_to_MQTT, 80000000, 3);       // Send data back to MQTT Web App every second
	EnableInterrupts();	
	//char inFilename[] = "Sounds/blessing.txt";   // 8 characters or fewer

	
//char inFilename2[] = "Sounds/blessing.txt";   // 8 characters or fewer
//char inFilenameLCD[]="Images/tadashie.txt";
//	ST7735_FillScreen(0);
//char inFilename2[] = "Sounds/sinewave.txt";   // 8 characters or fewer
char startLDC[]="Images/tdmode.txt";
//	MotorSequenceTest();
	//MotorSequenceTest();
	//	soundconvert();
	
SDLCD(startLDC);
RightMotorHi();

	//while(1){
			
		if(1){ // let's pretend we met some condition like a command that needs to play a sound
	//	if(1){ // let's pretend we met some condition like a command that needs to play a sound
			// read the first cluster of data
			//Init_Buffer();
//			SDReadIntoBuf(inFilename2);
	//		startPlayingSound(); 
			//Init_Buffer();
	//		SDReadIntoBuf(inFilename2);
	//		startPlayingSound(); 
	//		SDReadIntoBuf(inFilenameLCD);
			
			//while (1)
			// 		SDReadInto
		}		
	//	}		

		
		/*if(1){ // let's pretend we met some condition like a command that needs to play a sound
			// read the first cluster of data
			Init_Buffer();
			SDReadIntoBuf(inFilename);
			startPlayingSound(); 
			while(getNowReading()){ // continue reading the file into the buffer
				SDReadIntoBuf(inFilename);
			}
		}	*/		
	//}
	while(1){
		// if (getReadyForMore()){
			//SDReadIntoBuf(inFilename2);
			//SDReadIntoBuf(inFilename2);
			// clearReadyForMore();
		// }

	}
	
}




