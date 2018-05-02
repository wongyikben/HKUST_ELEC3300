#ifndef __UART_H
#define __UART_H

#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include <stdio.h>
#include "../../vec.h"
#include <stdarg.h>

typedef enum 
{
	UART_NULL = -1,		//disabled
    UART1 = 0,   		//usart1
    UART3 = 1,				//usart3
} COM_TypeDef;

#define UART_READY 					0xFF
#define UART_POS_BEGIN 			0xA0
#define UART_POS_END				0xA1
#define UART_TAR_BEGIN 			0xF0
#define UART_TAR_END				0xF1
#define UART_LED						0xB0
#define UART_ERROR					0xC0
#define UART_NEXT						0xE0

#define CHECK_TAR						0x80
#define CHECK_READY					0x20
#define CHECK_RESET					0x00
#define CHECK_POS						0x40
#define CHECK_NEXT       		0x10


typedef void on_receive_listener(const uint8_t byte);

void uart_init(COM_TypeDef COM, u32 br);
void uart_tx_byte(COM_TypeDef COM, const char data);
void uart_tx(COM_TypeDef COM, const char * tx_buf, ...);
void uart_interrupt(COM_TypeDef COM);
uint8_t uart_rx_byte(COM_TypeDef COM);
void uart_interrupt_init(COM_TypeDef COM, on_receive_listener *listener);


void uart_receive(const uint8_t byte);

void send_position(u16 x,u16 y);
void send_ready(void);
void send_target(u8* x,u8* y,u16 size);
void send_LED(u8 LED_mode);
void send_next(void);

u8 getcheck(void);
u16 getSize(void);
u8* get_target_x(void);
u8* get_target_y(void);
u8	get_LED(void);
vec3 get_position(void);
u8 get_ready(void);
u8 get_nextmove(void);
void reset_ready(void);

#endif
