#include "uart.h"

USART_TypeDef* com_usart[2] = {USART1, USART3};
uc32 com_tx_port_clk[2] = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOC};
uc32 com_rx_port_clk[2] = {RCC_APB2Periph_GPIOA, RCC_APB2Periph_GPIOC};
uc32 com_usart_clk[2] = {RCC_APB2Periph_USART1, RCC_APB1Periph_USART3};
GPIO_TypeDef* com_tx_port[2] = {GPIOA, GPIOC};
GPIO_TypeDef* com_rx_port[2] = {GPIOA, GPIOC};
uc16 com_tx_pin[2] = {GPIO_Pin_9, GPIO_Pin_10};
uc16 com_rx_pin[2] = {GPIO_Pin_10, GPIO_Pin_11};
uc16 com_irq[2] = {USART1_IRQn, USART3_IRQn};

on_receive_listener *uart_rx_listener[2];
u8 has_uart_rx_listener[2] = {0	};

u16 x_index=0;
u16 y_index=0;
u16 x_position = 0;
u16 y_position = 0;
vec3 position;
u8 LED_state=0;
u8 check = 0; // first bit for target mode, second bit for position mode,thrid bit for ready state,
// fourth bit for next move,last two bit for position
u8 target_x[500];
u8 target_y[500];

void uart_init(COM_TypeDef COM, u32 br) {
	RCC_APB2PeriphClockCmd(com_rx_port_clk[COM]|com_rx_port_clk[COM]|RCC_APB2Periph_AFIO, ENABLE);
	if (COM == UART1)
		RCC_APB2PeriphClockCmd(com_usart_clk[COM], ENABLE);
	else
		RCC_APB1PeriphClockCmd(com_usart_clk[COM], ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = com_tx_pin[COM];
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(com_tx_port[COM], &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Pin = com_rx_pin[COM];
	GPIO_Init(com_rx_port[COM], &GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_PartialRemap_USART3,ENABLE);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = br;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(com_usart[COM], &USART_InitStructure);
	USART_ITConfig(com_usart[COM], USART_IT_RXNE, ENABLE);
	USART_Cmd(com_usart[COM], ENABLE);
}

void uart_tx_byte(COM_TypeDef COM, char data) {
	while (USART_GetFlagStatus(com_usart[COM], USART_FLAG_TC) == RESET);
	USART_SendData(com_usart[COM], (u16)data);
}

uint8_t uart_rx_byte(COM_TypeDef COM) {
	while (USART_GetFlagStatus(com_usart[COM], USART_FLAG_RXNE) == RESET); 
	return (uint8_t)USART_ReceiveData(com_usart[COM]); 
}

void uart_tx(COM_TypeDef COM, const char *tx_buf, ...) {
	va_list arglist;
	char buf[255], *fp;
	
	va_start(arglist, tx_buf);
	vsprintf(buf, tx_buf, arglist);
	va_end(arglist);
	
	fp = buf;
	while (*fp)
		uart_tx_byte(COM, *fp++);
}

void uart_interrupt(COM_TypeDef COM) {
	NVIC_InitTypeDef NVIC_InitStructure;

	#ifdef VECT_TAB_RAM
	NVIC_SetVectorTable(NVIC_VectTab_RAM,0x0);
	#else
	NVIC_SetVectorTable(NVIC_VectTab_FLASH,0x0);
	#endif
	
	NVIC_InitStructure.NVIC_IRQChannel = com_irq[COM];
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	USART_ITConfig(com_usart[COM], USART_IT_RXNE, ENABLE);
}

void uart_interrupt_init(COM_TypeDef COM, on_receive_listener *listener) {
	uart_rx_listener[COM] = listener;
	has_uart_rx_listener[COM] = 1;
	uart_interrupt(COM);
}

void USART1_IRQHandler(void) {
	if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
		if (has_uart_rx_listener[UART1])
			(*uart_rx_listener[UART1])(USART_ReceiveData(USART1));
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

void USART3_IRQHandler(void) {
	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) {
		if (has_uart_rx_listener[UART3])
			(*uart_rx_listener[UART3])(USART_ReceiveData(USART3));
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	}
}

void uart_receive(const uint8_t byte){
		if(byte & 0x80){ //control signal
			if(byte == UART_READY){ // ready signal
				check = CHECK_READY; // ready
			}else if(byte == UART_TAR_BEGIN){ //begin of target mode
				check = CHECK_TAR;
				x_index=0;
				y_index=0;
			}else if(byte == UART_TAR_END){ //end of target mode
				check = CHECK_RESET;
				// Please Run your path finding program
			}else if(byte == UART_POS_BEGIN){ //begin of position mode
				check = CHECK_POS;
				x_position = 0;
				y_position = 0;
			}else if(byte == UART_POS_END){ //end of position mode
				check = CHECK_RESET;
				position.n[0]=x_position;
				position.n[1]=y_position;
			}else if(byte == UART_NEXT){
				check = CHECK_NEXT;
			}else if((byte&0xF0) == UART_LED){ // LED mode
				LED_state = byte&0x0F;
			}else if(byte == UART_ERROR){ // ERROR
				// ERROR Handler
			}else{
				uart_tx_byte(UART1,UART_ERROR);
			}			
		}
		else{
			if(check & CHECK_TAR){// put the byte into target array
				if(byte & 0x40){ //[01xxxxxx] = the x position  [00xxxxxx] = the y position 
					target_x[x_index] = byte & 0x3F;
					++x_index;
				}else{
					target_y[y_index] = byte & 0x3F;
					++y_index;
				}
			}else if(check & CHECK_POS){
				if((check & 0x03) == 0){ // position update
					u16 temp = byte;
					temp = temp<<7;
					x_position+=temp;
					check++;
				}else if((check & 0x03) == 1){
					u16 temp = byte;
					x_position+=temp;
					check++;
				}else if((check & 0x03) == 2){
					u16 temp = byte;
					temp = temp<<7;
					check++;
					y_position+=temp;
				}else if((check & 0x03) == 3){
					u16 temp = byte;
					y_position+=temp;
					check=0;
				}
			
			
			
			}
		}
}

void send_position(u16 x,u16 y){
	uart_tx_byte(UART1,0xA0);
	uart_tx_byte(UART1,x>>7);
	uart_tx_byte(UART1,x & 0x7F);
	uart_tx_byte(UART1,y>>7);
	uart_tx_byte(UART1,y & 0x7F);
	uart_tx_byte(UART1,0xA1);
}

void send_ready(void){
	check = 0;
	uart_tx_byte(UART1,UART_READY);
}

void send_target(u8* x,u8* y,u16 size){
	uart_tx_byte(UART1,UART_TAR_BEGIN);
	for(int i=0;i<size;i++){
		// max 2^6
		uart_tx_byte(UART1,x[i]|0x40);
		uart_tx_byte(UART1,y[i]);
	}
	uart_tx_byte(UART1,UART_TAR_END);
}

void send_next(void){uart_tx_byte(UART1,UART_NEXT);}

void send_LED(u8 LED_mode){
	u8 temp = UART_LED;
	temp += LED_mode;
	uart_tx_byte(UART1,temp);
 }

 u8 getcheck(void){return check;}
 
 u16 getSize(void){
	if(x_index==y_index){return x_index;}
	return 0;
 }
 
u8* get_target_x(void){return target_x;}
u8* get_target_y(void){return target_y;}
u8	get_LED(void){return LED_state;}
vec3 get_position(void){return position;}
u8 get_ready(void){return check& CHECK_READY;}
u8 get_nextmove(void){return check&CHECK_NEXT;}
// please use this function to reset all the state
void reset_ready (void){check=0;}
