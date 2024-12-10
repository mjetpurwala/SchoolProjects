#include <stdio.h>
#include <stdint.h>
#include "../inc/ST7735.h"
#include "../inc/Sine.h"
#include "Timer.h"

int oldsecx,oldsecy;
int oldminx,oldminy; 
int oldhourx,oldhoury; 
int clockflag;
void ssgDigit(int16_t alignx, int16_t aligny, int digit){
	/* Each segment should be 8 pixels long, 2 pixels thick, and have 1 pixel spacing */
	switch(digit){
		case 0:
			// a b c d e f
			ST7735_FillRect(alignx + 3, aligny + 0, 8, 2, 0x8ed8); // a
			ST7735_FillRect(alignx + 12, aligny + 3, 2, 8, 0x8ed8); // b
			ST7735_FillRect(alignx + 12, aligny + 15, 2, 8, 0x8ed8); // c
			ST7735_FillRect(alignx + 3, aligny + 24, 8, 2, 0x8ed8); // d
			ST7735_FillRect(alignx + 0, aligny + 15, 2, 8, 0x8ed8); // e
			ST7735_FillRect(alignx + 0, aligny + 3, 2, 8, 0x8ed8); // f
			break;
		case 1:
			// b c
			ST7735_FillRect(alignx + 12, aligny + 3, 2, 8, 0x8ed8); // b
			ST7735_FillRect(alignx + 12, aligny + 15, 2, 8, 0x8ed8); // c
			break;
		case 2:
			// a b d e g
			ST7735_FillRect(alignx + 3, aligny + 0, 8, 2, 0x8ed8); // a
			ST7735_FillRect(alignx + 12, aligny + 3, 2, 8, 0x8ed8); // b
			ST7735_FillRect(alignx + 3, aligny + 24, 8, 2, 0x8ed8); // d
			ST7735_FillRect(alignx + 0, aligny + 15, 2, 8, 0x8ed8); // e
			ST7735_FillRect(alignx + 3, aligny + 12, 8, 2, 0x8ed8); // g
			break;
		case 3:
			// a b c d g
			ST7735_FillRect(alignx + 3, aligny + 0, 8, 2, 0x8ed8); // a
			ST7735_FillRect(alignx + 12, aligny + 3, 2, 8, 0x8ed8); // b
			ST7735_FillRect(alignx + 12, aligny + 15, 2, 8, 0x8ed8); // c
			ST7735_FillRect(alignx + 3, aligny + 24, 8, 2, 0x8ed8); // d
			ST7735_FillRect(alignx + 3, aligny + 12, 8, 2, 0x8ed8); // g
			break;
		case 4:
			// b c f g
			ST7735_FillRect(alignx + 12, aligny + 3, 2, 8, 0x8ed8); // b
			ST7735_FillRect(alignx + 12, aligny + 15, 2, 8, 0x8ed8); // c
			ST7735_FillRect(alignx + 0, aligny + 3, 2, 8, 0x8ed8); // f
			ST7735_FillRect(alignx + 3, aligny + 12, 8, 2, 0x8ed8); // g
			break;
		case 5:
			// a c d f g
			ST7735_FillRect(alignx + 3, aligny + 0, 8, 2, 0x8ed8); // a
			ST7735_FillRect(alignx + 12, aligny + 15, 2, 8, 0x8ed8); // c
			ST7735_FillRect(alignx + 3, aligny + 24, 8, 2, 0x8ed8); // d
			ST7735_FillRect(alignx + 0, aligny + 3, 2, 8, 0x8ed8); // f
			ST7735_FillRect(alignx + 3, aligny + 12, 8, 2, 0x8ed8); // g
			break;
		case 6:
			// a c d e f g
			ST7735_FillRect(alignx + 3, aligny + 0, 8, 2, 0x8ed8); // a
			ST7735_FillRect(alignx + 12, aligny + 15, 2, 8, 0x8ed8); // c
			ST7735_FillRect(alignx + 3, aligny + 24, 8, 2, 0x8ed8); // d
			ST7735_FillRect(alignx + 0, aligny + 15, 2, 8, 0x8ed8); // e
			ST7735_FillRect(alignx + 0, aligny + 3, 2, 8, 0x8ed8); // f
			ST7735_FillRect(alignx + 3, aligny + 12, 8, 2, 0x8ed8); // g
			break;
		case 7:
			// a b c
			ST7735_FillRect(alignx + 3, aligny + 0, 8, 2, 0x8ed8); // a
			ST7735_FillRect(alignx + 12, aligny + 3, 2, 8, 0x8ed8); // b
			ST7735_FillRect(alignx + 12, aligny + 15, 2, 8, 0x8ed8); // c
			break;
		case 8:
			// a b c d e f g
			ST7735_FillRect(alignx + 3, aligny + 0, 8, 2, 0x8ed8); // a
			ST7735_FillRect(alignx + 12, aligny + 3, 2, 8, 0x8ed8); // b
			ST7735_FillRect(alignx + 12, aligny + 15, 2, 8, 0x8ed8); // c
			ST7735_FillRect(alignx + 3, aligny + 24, 8, 2, 0x8ed8); // d
			ST7735_FillRect(alignx + 0, aligny + 15, 2, 8, 0x8ed8); // e
			ST7735_FillRect(alignx + 0, aligny + 3, 2, 8, 0x8ed8); // f
			ST7735_FillRect(alignx + 3, aligny + 12, 8, 2, 0x8ed8); // g
			break;
		case 9:
			// a b c f g
			ST7735_FillRect(alignx + 3, aligny + 0, 8, 2, 0x8ed8); // a
			ST7735_FillRect(alignx + 12, aligny + 3, 2, 8, 0x8ed8); // b
			ST7735_FillRect(alignx + 12, aligny + 15, 2, 8, 0x8ed8); // c
			ST7735_FillRect(alignx + 0, aligny + 3, 2, 8, 0x8ed8); // f
			ST7735_FillRect(alignx + 3, aligny + 12, 8, 2, 0x8ed8); // g
			break;
		default:
			break;
	}
}

void analogInit(){
ST7735_FillScreen(0);
clockflag=1;
	int16_t y[64]={NULL};
	int16_t x[64] = {NULL};

	for(int i=0; i<=64; i++){
		y[i] = Sin(i);
		x[i] = Sin(64-i); 
			ST7735_DrawPixel(64+x[i]/2,64+(-1*y[i])/2,ST7735_WHITE);
			ST7735_DrawPixel(64+x[i]/2,64+(y[i])/2,ST7735_WHITE);
			ST7735_DrawPixel(64+(-1*x[i])/2,64+(-1*y[i])/2,ST7735_WHITE);
			ST7735_DrawPixel(64+(-1*x[i])/2,64+(y[i])/2,ST7735_WHITE);
}
	

//hard coding each number
	ST7735_DrawChar(60, 7, '1', ST7735_CYAN, ST7735_BLACK, 1);
	ST7735_DrawChar(65, 7, '2', ST7735_CYAN, ST7735_BLACK, 1);


	ST7735_DrawChar(90, 20, '1', ST7735_CYAN, ST7735_BLACK, 1);


	ST7735_DrawChar(105, 39, '2', ST7735_CYAN, ST7735_BLACK, 1);

	ST7735_DrawChar(105, 87, '4', ST7735_CYAN, ST7735_BLACK, 1);


	ST7735_DrawChar(33, 20, '1', ST7735_CYAN, ST7735_BLACK, 1);
	ST7735_DrawChar(38, 20, '1', ST7735_CYAN, ST7735_BLACK, 1);


	ST7735_DrawChar(37, 105, '7', ST7735_CYAN, ST7735_BLACK, 1);

	ST7735_DrawChar(88, 105, '5', ST7735_CYAN, ST7735_BLACK, 1);


	ST7735_DrawChar(16, 39, '1', ST7735_CYAN, ST7735_BLACK, 1);
	ST7735_DrawChar(21, 39, '0', ST7735_CYAN, ST7735_BLACK, 1);

	ST7735_DrawChar(19, 87, '8', ST7735_CYAN, ST7735_BLACK, 1);


	ST7735_DrawChar(62, 115, '6', ST7735_CYAN, ST7735_BLACK, 1);


	ST7735_DrawChar(115, 62, '3', ST7735_CYAN, ST7735_BLACK, 1);
	ST7735_DrawChar(10, 62, '9', ST7735_CYAN, ST7735_BLACK, 1);

	
}

void digitalInit(){
	clockflag=0;
	ST7735_FillScreen(0);
	// Colon 1
	ST7735_DrawChar(33, 20, ':', 0x8ed8, 0x0000, 4);
	// Colon 2
	ST7735_DrawChar(76, 20, ':', 0x8ed8, 0x0000, 4);
	//Hours
	ssgDigit(6, 20, hours/10);
	ssgDigit(23, 20, hours%10);
	// Minutes
	ssgDigit(49, 20, minutes/10);
	ssgDigit(66, 20, minutes%10);
	// Seconds
	ssgDigit(92, 20, seconds/10);
	ssgDigit(109, 20, seconds%10);
}

void updateDisplayA(int seconds){
//ST7735_FillScreen(0);
//analogInit();

    ST7735_Line(64, 64, 64 + (oldsecx*5/18), 64 + (oldsecy*5/18), ST7735_BLACK);
    ST7735_Line(64, 64, 64 + (oldminx*5/18), 64 + (oldminy*5/18), ST7735_BLACK);
    ST7735_Line(64, 64, 64 + (oldhourx*5/18), 64 + (oldhoury*5/18), ST7735_BLACK);

	uint16_t brown  = ST7735_Color565(190,115,50);

		
		int32_t angle = ((seconds*255)/60)-64 ;

    int32_t unscaledy = Sin(angle);
    int32_t unscaledx = Sin(64 + angle);  // Shift by p/2 for cosine

    //unscaledy = (32 * unscaledy / 127);  // Scale for display size
   // unscaledx = (32 * unscaledx / 127);

    ST7735_Line(64, 64, 64 + (unscaledx*5/18), 64 + (unscaledy*5/18), ST7735_RED);
		oldsecx=unscaledx;
		oldsecy=unscaledy;
		
		
		
		int32_t anglemin = ((minutes*255)/60)-64 ;

    int32_t unscaledminy = Sin(anglemin);
    int32_t unscaledminx = Sin(64 + anglemin);  // Shift by p/2 for cosine

    ST7735_Line(64, 64, 64 + (unscaledminx*5/18), 64 + (unscaledminy*5/18), ST7735_GREEN);
		oldminx=unscaledminx;
		oldminy=unscaledminy;
		
			int32_t anglehour = ((hours*255)/12)-64 ;

    int32_t unscaledhoury = Sin(anglehour);
    int32_t unscaledhourx = Sin(64 + anglehour);  // Shift by p/2 for cosine

    ST7735_Line(64, 64, 64 + (unscaledhourx*5/18), 64 + (unscaledhoury*5/18), brown);
		oldhourx=unscaledhourx;
		oldhoury=unscaledhoury;

//ST7735_Line(64,64,64+(unscaledx),64+(unscaledy),ST7735_RED);
}


int oldhour1, oldhour2, oldmin1, oldmin2, oldsec1, oldsec2;
void updateDisplayD(int hours, int minutes, int seconds){
	// Calculate new time
	int hour1 = hours / 10;
	int hour2 = hours % 10;
	int min1 = minutes / 10;
	int min2 = minutes % 10;
	int sec1 = seconds / 10;
	int sec2 = seconds % 10;
	
	// Hours
	if(hour1 != oldhour1){
		ST7735_FillRect(6, 20, 14, 26, 0x0000);
		ssgDigit(6, 20, hour1);
	}
	if(hour2 != oldhour2){
		ST7735_FillRect(23, 20, 14, 26, 0x0000);
		ssgDigit(23, 20, hour2);
	}
	// Minutes
	if(min1 != oldmin1){
		ST7735_FillRect(49, 20, 14, 26, 0x0000);
		ssgDigit(49, 20, min1);
	}
	if(min2 != oldmin2){
		ST7735_FillRect(66, 20, 14, 26, 0x0000);
		ssgDigit(66, 20, min2);
	}
	// Seconds
	if(sec1 != oldsec1){
		ST7735_FillRect(92, 20, 14, 26, 0x0000);
		ssgDigit(92, 20, sec1);
	}
	if(sec2 != oldsec2){
		ST7735_FillRect(109, 20, 14, 26, 0x0000);
		ssgDigit(109, 20, sec2);
	}
	
	// Save current time
	oldhour1 = hour1;
	oldhour2 = hour2;
	oldmin1 = min1;
	oldmin2 = min2;
	oldsec1 = sec1;
	oldsec2 = sec2;
}
