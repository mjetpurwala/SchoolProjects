


// hi world this is team baymax
#include <stdio.h>
#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/LaunchPad.h"
#include "../inc/ST7735.h"
#include "../inc/PLL.h"
#include "../inc/CortexM.h"
#include "../inc/SSD1306.h"
#include "../inc/ADCSWTrigger.h"
#include "../inc/esp8266.h"
#include "../inc/MQTT.h"
#include "../inc/SSD2119.h"



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
  
  //Output_Init();              // Initialize ST7735 LC
	
	

  EnableInterrupts();
	  LCD_Init();
	
  //SSD1306_SetPlot(T0,Tm,-100,100,SSD1306_WHITE);
  LCD_printf("and when i say gay i mean stupid and bad!!");

}
