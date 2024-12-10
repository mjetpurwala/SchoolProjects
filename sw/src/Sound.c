// Sound.c

//Names : Eva Nance and Rathna Sivakumar

/* What will the sound look like?
	Well, I'm assuming that the main will constantly check if some requirement was met.
	If requirement 1 is met, sound 1 will play,
	If requirement 2 is met, sound 2 will play,
	And so on.
	So there should be a function that the main can call with the sound number as its parameter.
	playsound(soundeffect sound) is that function.
	This function should read the desired sound data from the SD Card, process it, and then play it.
	At the moment, it reads from ROM, not the SD Card.
*/
#include <stdint.h>
#include "Sound.h"
#include "../inc/TLV5616.h"
#include "../inc/Timer1A.h"
#include "../inc/tm4c123gh6pm.h"
#include "SDCFile.h"


#define soundExampleLength 11382

void soundconvert(void){
	for(int i=0;i<soundExampleLength; i++){
		//soundExample[i]= soundExample8bit[i] * 16; 
	}
}

CircularBuffer parser; 

void initBuffer() {
		for (int i = 0; i < BUFFER_SIZE; i++){
			parser.buffer[i] = 0;
		}
    parser.head = 0;
    parser.tail = 0;
    parser.count = 0;
}

void Init_Buffer(){
	initBuffer();
}

int isBufferFull() {
    return parser.count == BUFFER_SIZE;
}

int isBufferOverHalfFull() {
    return parser.count > BUFFER_SIZE / 2;
}

int addToBuffer(uint32_t value) {
    if (isBufferFull()) {
        return 0; // buffer full
    }
		
    parser.buffer[parser.head] = value;         // Add value to the head
    parser.head = (parser.head + 1) % BUFFER_SIZE; // Advance head
    parser.count++;
    return 1; // success!
}

int isBufferEmpty() {
    return parser.count == 0;
}

int readFromBuffer() {
    if (isBufferEmpty()) {
        return ~0;
    }
    uint32_t value = parser.buffer[parser.tail];       // Read value from the tail
    parser.tail = (parser.tail + 1) % BUFFER_SIZE; // Advance tail
    parser.count--;
    return value;
}


void Timer1A_Start(void){
	NVIC_DIS0_R = 0<<21;        // turn off disable
	NVIC_EN0_R = 1<<21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x00000001;    // 10) enable TIMER1A
}

uint8_t readyForMore;

uint8_t getReadyForMore(void){
	return readyForMore;
}

void clearReadyForMore(void){
	readyForMore = 0;
}

void outputToDAC(){
//	static uint16_t dum = 0;
//	dum += 0x10;
//	DAC_Out((dum)&0xFFF);
//	return;
	uint32_t output = readFromBuffer();
	
	if (output == (~0)){
		Timer1A_Stop(); // empty so we done	
	}
	else{
	  DAC_Out(output);
		if(isBufferEmpty() && getNowReading()){
			readyForMore = 1;
		}
	}
}

void startPlayingSound(void){
	Timer1A_Start();
}

// initializes but stops timer for sound (so that it doesnt immediately start playing)
void Sound_Init(void){
	DAC_Init(0);
	Timer1A_Init(&outputToDAC, 80000, 2); // 11.025 kHz
	Timer1A_Stop();
	readyForMore = 0;
}

