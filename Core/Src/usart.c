#include "stm32f1xx_hal.h"
#include "usart.h"	

#if SYSTEM_SUPPORT_OS
#include "includes.h"
#endif

#if EN_USART1_RX   	
uint8_t USART_RX_BUF[USART_REC_LEN];
uint16_t USART_RX_STA=0;
uint8_t aRxBuffer[RXBUFFERSIZE];
UART_HandleTypeDef UART1_Handler;  

void uart_init(uint32_t bound)
{	
	UART1_Handler.Instance = USART1;
	UART1_Handler.Init.BaudRate = bound;
	UART1_Handler.Init.WordLength = UART_WORDLENGTH_8B;
	UART1_Handler.Init.StopBits = UART_STOPBITS_1;
	UART1_Handler.Init.Parity = UART_PARITY_NONE;
	UART1_Handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UART1_Handler.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&UART1_Handler);
	HAL_UART_Receive_IT(&UART1_Handler, (uint8_t *)aRxBuffer, RXBUFFERSIZE);
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
	GPIO_InitTypeDef GPIO_Initure;
	
	if(huart->Instance == USART1)
	{
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_USART1_CLK_ENABLE();
		__HAL_RCC_AFIO_CLK_ENABLE();
	
		GPIO_Initure.Pin = GPIO_PIN_9;
		GPIO_Initure.Mode = GPIO_MODE_AF_PP;
		GPIO_Initure.Pull = GPIO_PULLUP;
		GPIO_Initure.Speed = GPIO_SPEED_FREQ_HIGH;
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);

		GPIO_Initure.Pin = GPIO_PIN_10;
		GPIO_Initure.Mode = GPIO_MODE_AF_INPUT;
		HAL_GPIO_Init(GPIOA, &GPIO_Initure);
#if EN_USART1_RX
		HAL_NVIC_EnableIRQ(USART1_IRQn);
		HAL_NVIC_SetPriority(USART1_IRQn, 3, 3);
#endif	
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART1)
	{
		if((USART_RX_STA & 0x8000) == 0)
		{
			if(USART_RX_STA & 0x4000)
			{
				if(aRxBuffer[0] != 0x0a) USART_RX_STA = 0;
				else USART_RX_STA |= 0x8000; 
			}
			else
			{	
				if(aRxBuffer[0] == 0x0d) USART_RX_STA |= 0x4000;
				else
				{
					USART_RX_BUF[USART_RX_STA&0X3FFF] = aRxBuffer[0] ;
					USART_RX_STA++;
					if(USART_RX_STA > (USART_REC_LEN - 1)) USART_RX_STA = 0;
				}		 
			}
		}
	}
}
 
void USART1_IRQHandler(void)                	
{ 
	uint32_t timeout = 0;
#if SYSTEM_SUPPORT_OS
	OSIntEnter();    
#endif
	
	HAL_UART_IRQHandler(&UART1_Handler);
	
	timeout = 0;
    while (HAL_UART_GetState( &UART1_Handler ) != HAL_UART_STATE_READY)
	{
	 timeout++;
     if(timeout > HAL_MAX_DELAY) break;		
	}
     
	timeout = 0;
	while(HAL_UART_Receive_IT(&UART1_Handler, (uint8_t *)aRxBuffer, RXBUFFERSIZE) != HAL_OK)
	{
	 timeout++;
	 if(timeout > HAL_MAX_DELAY) break;	
	}
#if SYSTEM_SUPPORT_OS	 	// π”√OS
	OSIntExit();  											 
#endif
} 
#endif	
