#include "button.h"

static BUTTON buttons[4];

GPIO_TypeDef* button_ports[4] = {GPIOB, GPIOB, GPIOB,GPIOC};
uc16 button_pins[4] = {GPIO_Pin_6, GPIO_Pin_7,GPIO_Pin_4,GPIO_Pin_7};

/**
  * @brief  Initialization of button GPIO pins
  * @param  None
  * @retval None
  */
void button_init() {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_4;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	for (u8 i=0; i<4; i++) {
		buttons[i].port = button_ports[i];
		buttons[i].pin = button_pins[i];
		buttons[i].state = 1;
	}
}

/**
  * @brief  Read current state of a button
  * @param  id: button to read from
  * @retval 1 if not pressed; 0 if pressed
  */
u8 read_button(BUTTON_ID id) {
	if(buttons[id].state!=GPIO_ReadInputDataBit(buttons[id].port, buttons[id].pin)){
		buttons[id].state=GPIO_ReadInputDataBit(buttons[id].port, buttons[id].pin);
		return (~buttons[id].state)&0x01;
	}
	return 0;
}

