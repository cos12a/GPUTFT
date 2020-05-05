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



#define sbiSTREAM_BUFFER_LENGTH_BYTES		    ( ( size_t ) 64 )
#define sbiSTREAM_BUFFER_TRIGGER_LEVEL_10	    ( ( BaseType_t ) 10 )
#define sbiSTREAM_BUFFER_TRIGGER_LEVEL_2	    ( ( BaseType_t ) 2 )
#define sbiSTREAM_BUFFER_TRIGGER_LEVEL_1	    ( ( BaseType_t ) 1 )


#define RX2_BUFFER_LENGTH_BYTES                 32u
#define TX2_BUFFER_LENGTH_BYTES                 32u


static uint8_t Rx2_Buffer[ RX2_BUFFER_LENGTH_BYTES ];
static uint8_t Tx2_Buffer[ TX2_BUFFER_LENGTH_BYTES ];

/* The stream buffer that is used to send data from an interrupt to the task. */
//static StreamBufferHandle_t xStreamBuffer = NULL;

//static StreamBufferHandle_t Rx2StreamBuffer = NULL;
StreamBufferHandle_t Rx2StreamBuffer = NULL;

/* Definitions for USART2_Send */
extern osSemaphoreId_t USART2_SendHandle;

#define UC_SHELL_USART_PORT                         huart2
#define UC_SHELL_USART_MUTEXID                      USART2_TX_MutexHandle
#define UC_SHELL_USART_BUFFER                      Rx2StreamBuffer



/* The string that is sent from the interrupt to the task four bytes at a
time.  Must be multiple of 4 bytes long as the ISR sends 4 bytes at a time*/
static const char *pcStringToSend = "\r\n_____Hello FreeRTOS_____This is uc shell port.\r\n";
static const char pcString2ToSend[] = "\r\nThis is GPU_TFT Lcd port.\r\n";


static const char UC_SHELL_INIT_DONE[] = "\r\nuC-Shell init is done.\r\n";
static const char UC_SHELL_INIT_ERR[] = "\r\nuC-Shell init is Error.\r\n";    
static const char ShShell_Init_err[] = "ShShell_Init is Err.\r\n";
static const char ShShell_Init_done[] =  "ShShell_Init is Done.\r\n";
static const char userShell_Init_Done[] = "User shell init is done.\r\n";
static const char userShell_Init_Err[] = "User shell init is error.\r\n";

/* The string to task is looking for, which must be a substring of
pcStringToSend. */
//static const char * pcStringToReceive = "Hello FreeRTOS";

/* Set to pdFAIL if anything unexpected happens. */
//static BaseType_t xDemoStatus = pdPASS;

static void HAL_UART_Rx1CpltCallback(UART_HandleTypeDef *huart);
static void HAL_UART_Rx2CpltCallback(UART_HandleTypeDef *huart);
//static HAL_StatusTypeDef user_UART_Receive_IT(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_EndTransmit_IT(UART_HandleTypeDef *huart);
static void UART_DMAAbortOnError(DMA_HandleTypeDef *hdma);
static void UART_EndRxTransfer(UART_HandleTypeDef *huart);
static void HAL_UART_Tx2CpltCallback(UART_HandleTypeDef *huart);
static HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart);


void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);

HAL_StatusTypeDef user_UART_Receive_IT(UART_HandleTypeDef *huart, 
                                                uint8_t *pData, uint16_t Size);



CPU_INT16S  TerminalSerial_Wr (void        *pbuf,
                               CPU_SIZE_T   buf_len);


/* Incremented each time pcStringToReceive is correctly received, provided no
errors have occurred.  Used so the check task can check this task is still
running as expected. */
static uint32_t ulCycleCount = 0;

/*-----------------------------------------------------------*/

void vStartStreamBufferInterruptInit( void )
{

	/* Create the stream buffer that sends data from the interrupt to the
	task, and create the task. */
	Rx2StreamBuffer = xStreamBufferCreate( /* The buffer length in bytes. */
										 sbiSTREAM_BUFFER_LENGTH_BYTES,
										 /* The stream buffer's trigger level. */
										 sbiSTREAM_BUFFER_TRIGGER_LEVEL_2 );

    Mem_Init();
//	HAL_UART_Receive_IT(&huart1, Rx1_Buffer, 1u);
    

	user_UART_Receive_IT(&huart2, Rx2_Buffer, 1u);    
    if (Shell_Init()!= DEF_OK)
    {
        TerminalSerial_Wr((void *)UC_SHELL_INIT_ERR, sizeof( UC_SHELL_INIT_ERR ));

    }
    TerminalSerial_Wr((void *)UC_SHELL_INIT_DONE, sizeof( UC_SHELL_INIT_DONE ));

    if ( ShShell_Init() != DEF_OK)
    {
        TerminalSerial_Wr((void *)ShShell_Init_err, sizeof( ShShell_Init_err ));
    }
    TerminalSerial_Wr((void *)ShShell_Init_done, sizeof( ShShell_Init_done ));
    


//    TerminalSerial_Wr( (void *)pcStringToSend, stlen(pcStringToSend));
//    USAR2_send_byte((void *)pcString2ToSend, sizeof(pcString2ToSend));

    
}
/*-----------------------------------------------------------*/

/**
  * @brief  Wraps up transmission in non blocking mode.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
static HAL_StatusTypeDef UART_EndTransmit_IT(UART_HandleTypeDef *huart)
{
  /* Disable the UART Transmit Complete Interrupt */
  __HAL_UART_DISABLE_IT(huart, UART_IT_TC);

  /* Tx process is ended, restore huart->gState to Ready */
  huart->gState = HAL_UART_STATE_READY;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /*Call registered Tx complete callback*/
  huart->TxCpltCallback(huart);
#else
  /*Call legacy weak Tx complete callback*/
  HAL_UART_Tx2CpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

  return HAL_OK;
}


/**
  * @brief  Sends an amount of data in non blocking mode.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
static HAL_StatusTypeDef UART_Transmit_IT(UART_HandleTypeDef *huart)
{
  uint16_t *tmp;

  /* Check that a Tx process is ongoing */
  if (huart->gState == HAL_UART_STATE_BUSY_TX)
  {
    if (huart->Init.WordLength == UART_WORDLENGTH_9B)
    {
      tmp = (uint16_t *) huart->pTxBuffPtr;
      huart->Instance->DR = (uint16_t)(*tmp & (uint16_t)0x01FF);
      if (huart->Init.Parity == UART_PARITY_NONE)
      {
        huart->pTxBuffPtr += 2U;
      }
      else
      {
        huart->pTxBuffPtr += 1U;
      }
    }
    else
    {
      huart->Instance->DR = (uint8_t)(*huart->pTxBuffPtr++ & (uint8_t)0x00FF);
    }

    if (--huart->TxXferCount == 0U)
    {
      /* Disable the UART Transmit Complete Interrupt */
      __HAL_UART_DISABLE_IT(huart, UART_IT_TXE);

      /* Enable the UART Transmit Complete Interrupt */
      __HAL_UART_ENABLE_IT(huart, UART_IT_TC);
    }
    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}




void user_UART_IRQHandler(UART_HandleTypeDef *huart)
{
  uint32_t isrflags   = READ_REG(huart->Instance->SR);
  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
  uint32_t cr3its     = READ_REG(huart->Instance->CR3);
  uint32_t errorflags = 0x00U;
  uint32_t dmarequest = 0x00U;

  /* If no error occurs */
  errorflags = (isrflags & (uint32_t)(USART_SR_PE | USART_SR_FE | USART_SR_ORE | USART_SR_NE));
  if (errorflags == RESET)
  {
    /* UART in mode Receiver -------------------------------------------------*/
    if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
    {
      UART_Receive_IT(huart);
      return;
    }
  }
 /* If some errors occur */
  if ((errorflags != RESET) && (((cr3its & USART_CR3_EIE) != RESET) || ((cr1its & (USART_CR1_RXNEIE | USART_CR1_PEIE)) != RESET)))
  {
	/* UART parity error interrupt occurred ----------------------------------*/
	if (((isrflags & USART_SR_PE) != RESET) && ((cr1its & USART_CR1_PEIE) != RESET))
	{
	  huart->ErrorCode |= HAL_UART_ERROR_PE;
	}

	/* UART noise error interrupt occurred -----------------------------------*/
	if (((isrflags & USART_SR_NE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
	{
	  huart->ErrorCode |= HAL_UART_ERROR_NE;
	}

	/* UART frame error interrupt occurred -----------------------------------*/
	if (((isrflags & USART_SR_FE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
	{
	  huart->ErrorCode |= HAL_UART_ERROR_FE;
	}

	/* UART Over-Run interrupt occurred --------------------------------------*/
	if (((isrflags & USART_SR_ORE) != RESET) && ((cr3its & USART_CR3_EIE) != RESET))
	{
	  huart->ErrorCode |= HAL_UART_ERROR_ORE;
	}

	/* Call UART Error Call back function if need be --------------------------*/
	if (huart->ErrorCode != HAL_UART_ERROR_NONE)
	{
	  /* UART in mode Receiver -----------------------------------------------*/
	  if (((isrflags & USART_SR_RXNE) != RESET) && ((cr1its & USART_CR1_RXNEIE) != RESET))
	  {
		UART_Receive_IT(huart);
	  }

	  /* If Overrun error occurs, or if any error occurs in DMA mode reception,
		 consider error as blocking */
	  dmarequest = HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR);
	  if (((huart->ErrorCode & HAL_UART_ERROR_ORE) != RESET) || dmarequest)
	  {
		/* Blocking error : transfer is aborted
		   Set the UART state ready to be able to start again the process,
		   Disable Rx Interrupts, and disable Rx DMA request, if ongoing */
		UART_EndRxTransfer(huart);

		/* Disable the UART DMA Rx request if enabled */
		if (HAL_IS_BIT_SET(huart->Instance->CR3, USART_CR3_DMAR))
		{
		  CLEAR_BIT(huart->Instance->CR3, USART_CR3_DMAR);

		  /* Abort the UART DMA Rx channel */
		  if (huart->hdmarx != NULL)
		  {
			/* Set the UART DMA Abort callback :
			   will lead to call HAL_UART_ErrorCallback() at end of DMA abort procedure */
			huart->hdmarx->XferAbortCallback = UART_DMAAbortOnError;
			if (HAL_DMA_Abort_IT(huart->hdmarx) != HAL_OK)
			{
			  /* Call Directly XferAbortCallback function in case of error */
			  huart->hdmarx->XferAbortCallback(huart->hdmarx);
			}
		  }
		  else
		  {
			/* Call user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
			/*Call registered error callback*/
			huart->ErrorCallback(huart);
#else
			/*Call legacy weak error callback*/
			HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
		  }
		}
		else
		{
		  /* Call user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
		  /*Call registered error callback*/
		  huart->ErrorCallback(huart);
#else
		  /*Call legacy weak error callback*/
		  HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
		}
	  }
	  else
	  {
		/* Non Blocking error : transfer could go on.
		   Error is notified to user through user error callback */
#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
		/*Call registered error callback*/
		huart->ErrorCallback(huart);
#else
		/*Call legacy weak error callback*/
		HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

		huart->ErrorCode = HAL_UART_ERROR_NONE;
	  }
	}
	return;
  } /* End if some error occurs */

  /* UART in mode Transmitter ------------------------------------------------*/
  if (((isrflags & USART_SR_TXE) != RESET) && ((cr1its & USART_CR1_TXEIE) != RESET))
  {
	UART_Transmit_IT(huart);
	return;
  }

  /* UART in mode Transmitter end --------------------------------------------*/
  if (((isrflags & USART_SR_TC) != RESET) && ((cr1its & USART_CR1_TCIE) != RESET))
  {
	UART_EndTransmit_IT(huart);
	return;
  }

}



/**
  * @brief  Receives an amount of data in non blocking mode
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval HAL status
  */
static HAL_StatusTypeDef UART_Receive_IT(UART_HandleTypeDef *huart)
{
  uint16_t *tmp;

  /* Check that a Rx process is ongoing */
  if (huart->RxState == HAL_UART_STATE_BUSY_RX)
  {
    if (huart->Init.WordLength == UART_WORDLENGTH_9B)
    {
      tmp = (uint16_t *) huart->pRxBuffPtr;
      if (huart->Init.Parity == UART_PARITY_NONE)
      {
        *tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x01FF);
        huart->pRxBuffPtr += 2U;
      }
      else
      {
        *tmp = (uint16_t)(huart->Instance->DR & (uint16_t)0x00FF);
        huart->pRxBuffPtr += 1U;
      }
    }
    else
    {
      if (huart->Init.Parity == UART_PARITY_NONE)
      {
        *huart->pRxBuffPtr++ = (uint8_t)(huart->Instance->DR & (uint8_t)0x00FF);
      }
      else
      {
        *huart->pRxBuffPtr++ = (uint8_t)(huart->Instance->DR & (uint8_t)0x007F);
      }
      huart->RxXferSize ++;
    }
    

//    if (--huart->RxXferCount == 0U)
//    {
      /* Disable the UART Data Register not empty Interrupt */
//      __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);

      /* Disable the UART Parity Error Interrupt */
//      __HAL_UART_DISABLE_IT(huart, UART_IT_PE);

      /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
//      __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

      /* Rx process is completed, restore huart->RxState to Ready */
      huart->RxState = HAL_UART_STATE_BUSY_RX;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
      /*Call registered Rx complete callback*/
      huart->RxCpltCallback(huart);
#else
      /*Call legacy weak Rx complete callback*/
      	HAL_UART_Rx2CpltCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */

      return HAL_OK;
//    }
//    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}



/**
  * @brief  Receives an amount of data in non blocking mode.
  * @note   When UART parity is not enabled (PCE = 0), and Word Length is configured to 9 bits (M1-M0 = 01),
  *         the received data is handled as a set of u16. In this case, Size must indicate the number
  *         of u16 available through pData.
  * @param  huart Pointer to a UART_HandleTypeDef structure that contains
  *               the configuration information for the specified UART module.
  * @param  pData Pointer to data buffer (u8 or u16 data elements).
  * @param  Size  Amount of data elements (u8 or u16) to be received.
  * @retval HAL status
  */
HAL_StatusTypeDef user_UART_Receive_IT(UART_HandleTypeDef *huart, uint8_t *pData, uint16_t Size)
{
  /* Check that a Rx process is not already ongoing */
  if (huart->RxState == HAL_UART_STATE_READY)
  {
    if ((pData == NULL) || (Size == 0U))
    {
      return HAL_ERROR;
    }

    /* Process Locked */
    __HAL_LOCK(huart);

    huart->pRxBuffPtr = pData;
    huart->RxXferSize = 0u;
    huart->RxXferCount = 0u;

    huart->ErrorCode = HAL_UART_ERROR_NONE;
    huart->RxState = HAL_UART_STATE_BUSY_RX;

    /* Process Unlocked */
    __HAL_UNLOCK(huart);

    /* Enable the UART Parity Error Interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_PE);

    /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
    __HAL_UART_ENABLE_IT(huart, UART_IT_ERR);

    /* Enable the UART Data Register not empty Interrupt */
    __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);

    return HAL_OK;
  }
  else
  {
    return HAL_BUSY;
  }
}





// USART2 Rx完成时中断(uC-Shell使用串口)
// 释放接收完成信号量
static void HAL_UART_Rx2CpltCallback(UART_HandleTypeDef *huart)
{
    
  /* Prevent unused argument(s) compilation warning */

  const BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  size_t sendCnt = xStreamBufferSendFromISR( UC_SHELL_USART_BUFFER,
                                   ( const void * ) Rx2_Buffer,
                                   huart->RxXferSize,
                                   (BaseType_t *) &xHigherPriorityTaskWoken );

    huart->RxXferCount = 0u;
    huart->RxXferSize = 0u;
    huart->pRxBuffPtr = Rx2_Buffer;
    if ( sendCnt == 0)
    {

    // 错误处理.

    }

  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
}






#include "cpu.h"

CPU_INT08U  TerminalSerial_RdByte (void)
{
    char rxData;  
    size_t xReturned;
    
    xReturned = xStreamBufferReceive( /* The stream buffer data is being received from. */
                          UC_SHELL_USART_BUFFER,
                          /* Where to place received data. */
                          ( void * ) &( rxData ),
                          /* The number of bytes to receive. */
                          sizeof( char ),
                          /* The time to wait for the next data if the buffer
                          is empty. */
                          portMAX_DELAY );
    if ( xReturned < sizeof( char ) )
     {
         return ( 0u );
     }
     return ( rxData );

}




/**
  * @brief  Tx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */

static void HAL_UART_Tx2CpltCallback(UART_HandleTypeDef *huart)
{
    if ( huart->Instance == USART2 )
	{
        osSemaphoreRelease(USART2_SendHandle);
	}

}


/*
*********************************************************************************************************
*                                         TerminalSerial_Wr()
*
* Description : Serial output.
*
* Argument(s) : pbuf        Pointer to the buffer to transmit.
*
*               buf_len     Number of bytes in the buffer.
*
* Return(s)   : none.
*
* Caller(s)   : Terminal_Out().
*
* Note(s)     : none.
********************************************************************************************************
*/

CPU_INT16S  TerminalSerial_Wr (void        *pbuf,
                               CPU_SIZE_T   buf_len)
{
	HAL_StatusTypeDef sendStatus;
	 
    osSemaphoreAcquire(USART2_SendHandle, osWaitForever );
	sendStatus =  HAL_UART_Transmit_DMA(&UC_SHELL_USART_PORT, (uint8_t *)pbuf, buf_len);
	return( (CPU_INT16S) sendStatus );
}


void  TerminalSerial_WrByte (CPU_INT08U c)
{
    osSemaphoreAcquire(USART2_SendHandle, osWaitForever );
	HAL_UART_Transmit_DMA(&UC_SHELL_USART_PORT, (uint8_t *)&c, sizeof(char));
}




/**
  * @brief  UART error callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */

    if ( huart->Instance == USART2) {

        /* Disable the UART Data Register not empty Interrupt */
        __HAL_UART_DISABLE_IT(huart, UART_IT_RXNE);

        /* Disable the UART Parity Error Interrupt */
        __HAL_UART_DISABLE_IT(huart, UART_IT_PE);

        /* Disable the UART Error Interrupt: (Frame error, noise error, overrun error) */
        __HAL_UART_DISABLE_IT(huart, UART_IT_ERR);

        switch( huart->ErrorCode )   {

            case HAL_UART_ERROR_NONE:  
                break;
            case HAL_UART_ERROR_PE: 
                __HAL_UART_CLEAR_PEFLAG(huart);
                break;
            case HAL_UART_ERROR_NE:    
                __HAL_UART_CLEAR_NEFLAG(huart);
                break;
            case HAL_UART_ERROR_FE:   
                __HAL_UART_CLEAR_FEFLAG(huart);
                break;
            case HAL_UART_ERROR_ORE:    
                __HAL_UART_CLEAR_OREFLAG(huart);
                break;
            case HAL_UART_ERROR_DMA: 
//                __HAL_UART_CLEAR_FLAG(huart, );
                break;
            default:
                break;
                
                
            
        }
        huart->ErrorCode = HAL_UART_ERROR_NONE;

        /* Rx process is completed, restore huart->RxState to Ready */
        huart->RxState = HAL_UART_STATE_BUSY_RX;

        /* Enable the UART Parity Error Interrupt */
        __HAL_UART_ENABLE_IT(huart, UART_IT_PE);

        /* Enable the UART Error Interrupt: (Frame error, noise error, overrun error) */
        __HAL_UART_ENABLE_IT(huart, UART_IT_ERR);

        /* Enable the UART Data Register not empty Interrupt */
        __HAL_UART_ENABLE_IT(huart, UART_IT_RXNE);
        huart->RxXferCount = 0u;
        huart->RxXferSize = 0u;
        huart->pRxBuffPtr = Rx2_Buffer;
        

    }
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_ErrorCallback could be implemented in the user file
   */
}
/**
  * @brief  DMA UART communication abort callback, when initiated by HAL services on Error
  *         (To be called at end of DMA Abort procedure following error occurrence).
  * @param  hdma  Pointer to a DMA_HandleTypeDef structure that contains
  *               the configuration information for the specified DMA module.
  * @retval None
  */
static void UART_DMAAbortOnError(DMA_HandleTypeDef *hdma)
{
  UART_HandleTypeDef *huart = (UART_HandleTypeDef *)((DMA_HandleTypeDef *)hdma)->Parent;
  huart->RxXferCount = 0x00U;
  huart->TxXferCount = 0x00U;

#if (USE_HAL_UART_REGISTER_CALLBACKS == 1)
  /*Call registered error callback*/
  huart->ErrorCallback(huart);
#else
  /*Call legacy weak error callback*/
  HAL_UART_ErrorCallback(huart);
#endif /* USE_HAL_UART_REGISTER_CALLBACKS */
}




/**
  * @brief  End ongoing Rx transfer on UART peripheral (following error detection or Reception completion).
  * @param  huart UART handle.
  * @retval None
  */
static void UART_EndRxTransfer(UART_HandleTypeDef *huart)
{
  /* Disable RXNE, PE and ERR (Frame error, noise error, overrun error) interrupts */
//  CLEAR_BIT(huart->Instance->CR1, (USART_CR1_RXNEIE | USART_CR1_PEIE));
//  CLEAR_BIT(huart->Instance->CR3, USART_CR3_EIE);
    if ( huart->Instance == USART2) {
        huart->RxXferCount = 0u;
        huart->RxXferSize = 0u;
        huart->ErrorCode |= HAL_UART_ERROR_NONE;
//        while(1);
    }
        
  /* At end of Rx process, restore huart->RxState to Ready */
//  huart->RxState = HAL_UART_STATE_READY;
}




