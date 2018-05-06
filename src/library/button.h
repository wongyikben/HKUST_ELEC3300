#ifndef __BUTTON_H
#define __BUTTON_H

#include "stm32f10x_gpio.h"

typedef void button_listener(void);

typedef struct{
	GPIO_TypeDef* port;
	u16 pin;
	u8 state;
}BUTTON;

typedef enum{
	BUTTON1,
	BUTTON2,
	BUTTON3,
	BUTTON4
}BUTTON_ID;

void button_init(void);
u8 read_button(BUTTON_ID id);

#endif
