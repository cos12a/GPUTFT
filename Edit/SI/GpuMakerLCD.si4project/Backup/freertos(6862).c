/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};
/* Definitions for USART1_TX */
osThreadId_t USART1_TXHandle;
const osThreadAttr_t USART1_TX_attributes = {
  .name = "USART1_TX",
  .priority = (osPriority_t) osPriorityRealtime,
  .stack_size = 128 * 4
};
/* Definitions for USART2_TX */
osThreadId_t USART2_TXHandle;
const osThreadAttr_t USART2_TX_attributes = {
  .name = "USART2_TX",
  .priority = (osPriority_t) osPriorityRealtime1,
  .stack_size = 128 * 4
};
/* Definitions for UC_SHELL_task */
osThreadId_t UC_SHELL_taskHandle;
const osThreadAttr_t UC_SHELL_task_attributes = {
  .name = "UC_SHELL_task",
  .priority = (osPriority_t) osPriorityRealtime,
  .stack_size = 256 * 4
};
/* Definitions for USART1_TX_Mutex */
osMutexId_t USART1_TX_MutexHandle;
const osMutexAttr_t USART1_TX_Mutex_attributes = {
  .name = "USART1_TX_Mutex"
};
/* Definitions for USART2_TX_Mutex */
osMutexId_t USART2_TX_MutexHandle;
const osMutexAttr_t USART2_TX_Mutex_attributes = {
  .name = "USART2_TX_Mutex"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
   
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void usart1TxTask(void *argument);
void Usart2TxTask(void *argument);
void uc_shell_task(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
       
  /* USER CODE END Init */
  /* Create the mutex(es) */
  /* creation of USART1_TX_Mutex */
  USART1_TX_MutexHandle = osMutexNew(&USART1_TX_Mutex_attributes);

  /* creation of USART2_TX_Mutex */
  USART2_TX_MutexHandle = osMutexNew(&USART2_TX_Mutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* creation of USART1_TX */
  USART1_TXHandle = osThreadNew(usart1TxTask, NULL, &USART1_TX_attributes);

  /* creation of USART2_TX */
  USART2_TXHandle = osThreadNew(Usart2TxTask, NULL, &USART2_TX_attributes);

  /* creation of UC_SHELL_task */
  UC_SHELL_taskHandle = osThreadNew(uc_shell_task, NULL, &UC_SHELL_task_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */


  vStartStreamBufferInterruptInit();
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used 
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
//    osMutexWait(USART1_TX_MutexHandle, portMAX_DELAY);
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_usart1TxTask */
/**
* @brief Function implementing the USART1_TX thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_usart1TxTask */
__weak void usart1TxTask(void *argument)
{
  /* USER CODE BEGIN usart1TxTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END usart1TxTask */
}

/* USER CODE BEGIN Header_Usart2TxTask */
/**
* @brief Function implementing the USART2_TX thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_Usart2TxTask */
__weak void Usart2TxTask(void *argument)
{
  /* USER CODE BEGIN Usart2TxTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END Usart2TxTask */
}

/* USER CODE BEGIN Header_uc_shell_task */
/**
* @brief Function implementing the UC_SHELL_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_uc_shell_task */
__weak void uc_shell_task(void *argument)
{
  /* USER CODE BEGIN uc_shell_task */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END uc_shell_task */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
     
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
