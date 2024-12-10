#ifndef _SCDFILE
#define _SCDFILE

#include <stdint.h>
#include <stdio.h>


void diskError(char *errtype, int32_t code, int32_t block);

void SimpleUnformattedTest(void);

void FileSystemTest(void);

void SDshenanigans(void);

void SDReadIntoBuf(char* inFilename);

void Parser500(void);

int getNowReading(void);

void SDLCD(char* inFilenameLCD);

void SDLCD_Transparent(char* inFilenameLCD, uint16_t width, uint16_t height);

#endif 