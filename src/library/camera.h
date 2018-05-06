

#ifndef __OV7725_H
#define __OV7725_H

#include "stm32f10x.h"
#include "ticks.h"
#include "ov7725define.h"
#include "tft.h"
#include "uart.h"
#include "flash.h"

//sccb
uint8_t cameraSccbInit(ImageType outputType);
uint8_t sccbWriteByte(uint16_t address,uint8_t data);
uint8_t sccbReadByte(uint8_t* buffer, uint16_t length, uint8_t address);
//ov7725
uint8_t cameraInit(ImageType outputType);
uint8_t cameraReceiveFrame(void);
uint8_t cameraCaptureFrame(void);
//flipping RGB to BGR or BGR to RGB
uint16_t flipRB(uint16_t input);
//changing 8bit grey scale to 16bit grey scale
uint16_t greyScale8bitTo16bit(uint8_t);
//camera testing function with tft display
void cameraTestTftDisplay(void);

void cameraHighResolution(void);

void positionInit(void);

void storeImage(void);

void uartImage(void);

void TestFunction(void);

void PositionUpdate(void);

vec3 WhiteBoardToCamera(vec3 v);

vec3 CameraToWhiteBoard(vec3 v);

void flashDetection(u8 number,u8 LED,u8 cor);

vec3 getCorner(u8 cor);

u16 get_x_length(void);

u16 get_y_length(void);
// -image colour, resolution setting-
//sccbWriteByte(COM7,);

// -image size setting-
//sccbWriteByte(HSTART,);
//sccbWriteByte(HSIZE,);
//sccbWriteByte(VSTRT,);
//sccbWriteByte(VSIZE,);
//sccbWriteByte(HREF,);
//sccbWriteByte(HOutSize,);
//sccbWriteByte(VOutSize,);
//sccbWriteByte(EXHCH,);

// -digital effect-
//sccbWriteByte(SDE,);

// -YUV setting-
//sccbWriteByte(UFix,);
//sccbWriteByte(VFix,);

// -Brightness-
//sccbWriteByte(BRIGHT,);

// -Contrast-
//sccbWriteByte(CNST,);

#endif //__OV7725_H
