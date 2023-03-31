#ifndef __USART_H
#define __USART_H
#include "stm32f4xx_hal.h"

#define USART_REC_LEN  			200
#define EN_USART1_RX 			1
	  	
extern uint8_t  USART_RX_BUF[USART_REC_LEN];
extern uint16_t USART_RX_STA;
extern UART_HandleTypeDef UART1_Handler;

#define RXBUFFERSIZE   1 
extern uint8_t aRxBuffer[RXBUFFERSIZE];

void uart_init(uint32_t bound);
#endif


