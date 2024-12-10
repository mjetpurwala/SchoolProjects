#ifndef __Display_h
#define __Display_h
extern int clockflag;
void analogInit(void);
void digitalInit(void);
//void secondHand(int seconds);
void updateDisplayA(int seconds);
void updateDisplayD(int hours, int minutes, int seconds);

#endif /* __Display_h */




