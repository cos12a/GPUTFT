
/*
 * FreeRTOS Kernel V10.3.1
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*
 * A simple example that shows a stream buffer being used to pass data from an
 * interrupt to a task.
 *
 * There are two strings, pcStringToSend and pcStringToReceive, where
 * pcStringToReceive is a substring of pcStringToSend.  The interrupt sends
 * a few bytes of pcStringToSend to a stream buffer ever few times that it
 * executes.  A task reads the bytes from the stream buffer, looking for the
 * substring, and flagging an error if the received data is invalid.
 */



/* Standard includes. */
#include <stdint.h>
#include <string.h>

#include "stdio.h"
#include "string.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "cmsis_os.h"


/* Demo app includes. */
#include "StreamBufferInterrupt.h"

#include "semphr.h"    

#include "app_cfg.h"

#include "usart.h"
#include "lib_def.h"
#include  "lib_mem.h"
#include  "lib_math.h"
#include  "lib_str.h"

#include "shell.h"
#include "sh_shell.h"



/* Definitions for USART1_Send */
extern osSemaphoreId_t USART1_SendHandle;
extern osSemaphoreId_t USART1_RxHandle;     // USART1_RxHandle Semaphore

#define sbiSTREAM_BUFFER_LENGTH_BYTES		    ( ( size_t ) 64 )
#define sbiSTREAM_BUFFER_TRIGGER_LEVEL_10	    ( ( BaseType_t ) 10 )
#define sbiSTREAM_BUFFER_TRIGGER_LEVEL_2	    ( ( BaseType_t ) 2 )


#define GPU_TFT_USART_PORT                          huart1

#define RX1_BUFFER_LENGTH_BYTES                 32u
#define TX1_BUFFER_LENGTH_BYTES                 32u

//static StreamBufferHandle_t Rx1StreamBuffer = NULL;

static const char GPU_SNED_TEST[] = "\r\nGPU Seria prot test success.\r\n";

static uint8_t Rx1_Buffer[ RX1_BUFFER_LENGTH_BYTES ];
static uint8_t Tx1_Buffer[ TX1_BUFFER_LENGTH_BYTES ];


void GPU_Serial_init(void)
{


	/* Create the stream buffer that sends data from the interrupt to the
	task, and create the task. */
//	Rx1StreamBuffer = xStreamBufferCreate( /* The buffer length in bytes. */
//										 sbiSTREAM_BUFFER_LENGTH_BYTES,
//										 /* The stream buffer's trigger level. */
//										 sbiSTREAM_BUFFER_TRIGGER_LEVEL_2 );

    GPU_TFT_send_byte((void *)GPU_SNED_TEST, sizeof(GPU_SNED_TEST));



}



//static void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//	UNUSED(huart);
//}


// USART1 Rx完成中断回调函数(GPU串口)

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if ( huart->Instance == USART1 ){
        osSemaphoreRelease(USART1_RxHandle);        // 释放接收完成信号量
    }
}



// 发送完成中断回调函数

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        osSemaphoreRelease(USART1_SendHandle);
    }

}

HAL_StatusTypeDef GPU_TFT_send_byte( void *pbuf,	uint16_t buf_len)
{

	HAL_StatusTypeDef sendStatus;

    osSemaphoreAcquire(USART1_SendHandle, osWaitForever );
	sendStatus =  HAL_UART_Transmit_DMA(&GPU_TFT_USART_PORT, (uint8_t *)pbuf, buf_len);
	return( (HAL_StatusTypeDef) sendStatus );

}



// 将GPU数据发送出去,并接收回来判断是否OK.如果超时则再发送一次,
// 如果同一数据三次发送失败则返回发送Error

#define GPU_RETURN_OK   "OK"

HAL_StatusTypeDef GPU_tx_and_rx_hand(void         *pbuf,
                                   uint16_t    buf_len)
{

    osStatus_t stat;
    uint8_t sendCnt = 0u;
    HAL_StatusTypeDef reStat = HAL_OK;
    const TickType_t xBlockTime = pdMS_TO_TICKS( 500ul );       //等待500ms

    for (sendCnt =0; sendCnt <= 3u; sendCnt ++) {
        
        GPU_TFT_send_byte(pbuf, buf_len);           /// 发送数据
        
        Mem_Set((void     *)&Rx1_Buffer[0],                           /* Clr cur working dir path.                            */
                            (CPU_INT08U) 0x00u,
                            sizeof (Rx1_Buffer));
    
        reStat = HAL_UART_Receive_IT (&GPU_TFT_USART_PORT, Rx1_Buffer, 2u);         //开始接收数据
        stat = osSemaphoreAcquire(USART1_RxHandle, xBlockTime );         // 等待接收信号量接收状态500mS延时.

        if ( osOK == stat) {        
            if (( Mem_Cmp(GPU_RETURN_OK, Rx1_Buffer, 2) == DEF_YES)) {
                return ( HAL_OK );          // 正常退出.
             }
        } else {                        // 清除本次接收,再进行下一次接收
            HAL_UART_AbortReceive_IT(&GPU_TFT_USART_PORT);          // 接收未成功中止接收数据
        }
        
    }
    return( reStat );               // 错误 退出.
}


/**
  * @brief  UART Abort Receive Complete callback.
  * @param  huart UART handle.
  * @retval None
  */
void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
//  if ( huart->Instance == USART1 )
//  {
//      osSemaphoreRelease(USART1_SendHandle);
//  }
    UNUSED(huart);
  /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_UART_AbortReceiveCpltCallback can be implemented in the user file.
   */
}
