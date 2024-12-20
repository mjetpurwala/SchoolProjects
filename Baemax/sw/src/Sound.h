// Sound.h
// Runs on TM4C123 
// Prototypes for basic functions to play sounds from the
// original Space Invaders.
// Jonathan Valvano
// 1/14/21
#ifndef _Sound_h
#define _Sound_h
#include <stdint.h>

// Header files contain the prototypes for public functions 
// this file explains what the module does
void Sound_Init(void);
//******* Sound_Start ************
// This function does not output to the DAC. 
// Rather, it sets a pointer and counter, and then enables the timer interrupt.
// It starts the sound, and the timer ISR does the output
// feel free to change the parameters
// Input: pt is a pointer to an array of DAC outputs
//        count is the length of the array
// Output: none
// special cases: as you wish to implement
void Sound_Play(const uint8_t *pt, uint32_t count);

#define BUFFER_SIZE 250 

typedef struct {
    uint32_t buffer[BUFFER_SIZE]; // Array to store elements
    uint32_t head;                // Write index
    uint32_t tail;                // Read index
    uint32_t count;               // Number of elements in the buffer
} CircularBuffer;


void soundconvert(void);
void startPlayingSound(void);
int addToBuffer(uint32_t value);
void Init_Buffer(void);
char* GetInFileName(void);
uint8_t getReadyForMore(void);
void clearReadyForMore(void);



#endif


