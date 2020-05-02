#ifndef __APP_CFG_H__
#define __APP_CFG_H__


#include "usart.h"




void vStartStreamBufferInterruptInit( void );

void user_UART_IRQHandler(UART_HandleTypeDef *huart);

HAL_StatusTypeDef GPU_tx_and_rx_hand(void         *pbuf,
                                   CPU_SIZE_T    buf_len);



#endif
