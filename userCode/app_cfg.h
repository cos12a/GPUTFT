#ifndef __APP_CFG_H__
#define __APP_CFG_H__


#include "usart.h"




void vStartStreamBufferInterruptInit( void );

void user_UART_IRQHandler(UART_HandleTypeDef *huart);



#endif
