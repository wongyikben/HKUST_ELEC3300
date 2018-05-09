#ifndef __FLASHSAVE_H
#define __FLASHSAVE_H
#include "stm32f10x.h"
#include "stm32f10x_flash.h"

#define arraySize 1920
#define colSize 	(arraySize/320)

void writeRow(u8* array, u16 y);
void writeRow2(u8* array,u16 y);
void writenum(u8 num);

u8 readImage(u16 x,u16 y);
u8 readImage2(u16 x, u16 y);
u16 readnum(void);

#endif
