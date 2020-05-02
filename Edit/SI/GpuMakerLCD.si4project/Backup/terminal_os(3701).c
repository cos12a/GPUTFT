/*
*********************************************************************************************************
*                                              uC/Shell
*                                            Shell utility
*
*                    Copyright 2007-2020 Silicon Laboratories Inc. www.silabs.com
*
*                                 SPDX-License-Identifier: APACHE-2.0
*
*               This software is subject to an open source license and is distributed by
*                Silicon Laboratories Inc. pursuant to the terms of the Apache License,
*                    Version 2.0 available at www.apache.org/licenses/LICENSE-2.0.
*
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*                                              TERMINAL
*
*                                         uC/OS-II RTOS PORT
*
* Filename : terminal_os.c
* Version  : V1.04.00
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                            INCLUDE FILES
*********************************************************************************************************
*/

#include  <terminal.h>
#include  <FreeRTOS.h>
#include  <sh_shell.h>  


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/
//

///* Definitions for UC_SHELL_task */
//osThreadId_t UC_SHELL_taskHandle;
//const osThreadAttr_t UC_SHELL_task_attributes = {
//  .name = "UC_SHELL_task",
//  .priority = (osPriority_t) osPriorityRealtime,
//  .stack_size = 256 * 4
//};


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

//static  void  Terminal_OS_Task(void  *p_arg);



/*
*********************************************************************************************************
*                                         Terminal_OS_Init()
*
* Description : Initialize the terminal task.
*
* Argument(s) : p_arg       Argument to pass to the task.
*
* Return(s)   : DEF_FAIL    Initialize task failed.
*               DEF_OK      Initialize task successful.
*
* Caller(s)   : Terminal_Init()
*
* Note(s)     : The RTOS needs to create Terminal_OS_Task().
*********************************************************************************************************
*/

/* creation of UC_SHELL_task */
//UC_SHELL_taskHandle = osThreadNew(uc_shell_task, NULL, &UC_SHELL_task_attributes);


/*
*********************************************************************************************************
*                                         Terminal_OS_Task()
*
* Description : RTOS interface for terminal main loop.
*
* Argument(s) : p_arg       Argument to pass to the task.
*
* Return(s)   : none.
*
* Caller(s)   : RTOS.
*
* Note(s)     : none.
*********************************************************************************************************
*/



/* USER CODE BEGIN Header_uc_shell_task */
/**
* @brief Function implementing the UC_SHELL_task thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_uc_shell_task */
 void uc_shell_task(void *argument)
{
  /* USER CODE BEGIN uc_shell_task */
    Mem_Init();

    ShShell_Init();

  /* Infinite loop */
    Terminal_Task(argument);

  /* USER CODE END uc_shell_task */
}

