/**
  ******************************************************************************
  * @file    stm32f1xx_hal_rtc.c
  * @author  MCD Application Team
  * @brief   RTC HAL module driver.
  *          This file provides firmware functions to manage the following
  *          functionalities of the Real Time Clock (RTC) peripheral:
  *           + Initialization and de-initialization functions
  *           + RTC Time and Date functions
  *           + RTC Alarm functions
  *           + Peripheral Control functions
  *           + Peripheral State functions
  *
  @verbatim
 =============================================================================
                  ##### How to use this driver #####
 =================================================================
  [..]
    (+) Enable the RTC domain access (see description in the section above).
    (+) Configure the RTC Prescaler (Asynchronous prescaler to generate RTC 1Hz time base)
        using the HAL_RTC_Init() function.

  *** Time and Date configuration ***
 ==================================
  [..]
    (+) To configure the RTC Calendar (Time and Date) use the HAL_RTC_SetTime()
        and HAL_RTC_SetDate() functions.
    (+) To read the RTC Calendar, use the HAL_RTC_GetTime() and HAL_RTC_GetDate() functions.

  *** Alarm configuration ***
 ==========================
  [..]
    (+) To configure the RTC Alarm use the HAL_RTC_SetAlarm() function.
        You can also configure the RTC Alarm with interrupt mode using the HAL_RTC_SetAlarm_IT() function.
    (+) To read the RTC Alarm, use the HAL_RTC_GetAlarm() function.

  *** Tamper configuration ***
 ===========================
  [..]
    (+) Enable the RTC Tamper and configure the Tamper Level using the
        HAL_RTCEx_SetTamper() function. You can configure RTC Tamper with interrupt
        mode using HAL_RTCEx_SetTamper_IT() function.
    (+) The TAMPER1 alternate function can be mapped to PC13

  *** Backup Data Registers configuration ***
 ==========================================
  [..]
    (+) To write to the RTC Backup Data registers, use the HAL_RTCEx_BKUPWrite()
        function.
    (+) To read the RTC Backup Data registers, use the HAL_RTCEx_BKUPRead()
        function.

                  ##### WARNING: Drivers Restrictions  #####
 =================================================================
  [..] RTC version used on STM32F1 families is version V1. All the features supported by V2
       (other families) will be not supported on F1.
  [..] As on V2, main RTC features are managed by HW. But on F1, date feature is completely
       managed by SW.
  [..] Then, there are some restrictions compared to other families:
    (+) Only format 24 hours supported in HAL (format 12 hours not supported)
    (+) Date is saved in SRAM. Then, when MCU is in STOP or STANDBY mode, date will be lost.
        User should implement a way to save date before entering in low power mode (an
        example is provided with firmware package based on backup registers)
    (+) Date is automatically updated each time a HAL_RTC_GetTime or HAL_RTC_GetDate is called.
    (+) Alarm detection is limited to 1 day. It will expire only 1 time (no alarm repetition, need
        to program a new alarm)

              ##### Backup Domain Operating Condition #####
 =============================================================================
  [..] The real-time clock (RTC) and the RTC backup registers can be powered
       from the VBAT voltage when the main VDD supply is powered off.
       To retain the content of the RTC backup registers and supply the RTC
       when VDD is turned off, VBAT pin can be connected to an optional
       standby voltage supplied by a battery or by another source.

  [..] To allow the RTC operating even when the main digital supply (VDD) is turned
       off, the VBAT pin powers the following blocks:
    (#) The RTC
    (#) The LSE oscillator
    (#) The backup SRAM when the low power backup regulator is enabled
    (#) PC13 to PC15 I/Os, plus PI8 I/O (when available)

  [..] When the backup domain is supplied by VDD (analog switch connected to VDD),
       the following pins are available:
    (+) PC13 can be used as a Tamper pin

  [..] When the backup domain is supplied by VBAT (analog switch connected to VBAT
       because VDD is not present), the following pins are available:
    (+) PC13 can be used as the Tamper pin

                   ##### Backup Domain Reset #####
 =================================================================
  [..] The backup domain reset sets all RTC registers and the RCC_BDCR register
       to their reset values.
  [..] A backup domain reset is generated when one of the following events occurs:
    (#) Software reset, triggered by setting the BDRST bit in the
        RCC Backup domain control register (RCC_BDCR).
    (#) VDD or VBAT power on, if both supplies have previously been powered off.
    (#) Tamper detection event resets all data backup registers.

                   ##### Backup Domain Access #####
 =================================================================
  [..] After reset, the backup domain (RTC registers, RTC backup data
       registers and backup SRAM) is protected against possible unwanted write
       accesses.
  [..] To enable access to the RTC Domain and RTC registers, proceed as follows:
    (+) Call the function HAL_RCCEx_PeriphCLKConfig in using RCC_PERIPHCLK_RTC for
        PeriphClockSelection and select RTCClockSelection (LSE, LSI or HSE)
    (+) Enable the BKP clock in using __HAL_RCC_BKP_CLK_ENABLE()

                  ##### RTC and low power modes #####
 =================================================================
  [..] The MCU can be woken up from a low power mode by an RTC alternate
       function.
  [..] The RTC alternate functions are the RTC alarms (Alarm A),
       and RTC tamper event detection.
       These RTC alternate functions can wake up the system from the Stop and
       Standby low power modes.
  [..] The system can also wake up from low power modes without depending
       on an external interrupt (Auto-wakeup mode), by using the RTC alarm.

  *** Callback registration ***
 ============================================
  [..]
  The compilation define  USE_HAL_RTC_REGISTER_CALLBACKS when set to 1
  allows the user to configure dynamically the driver callbacks.
  Use Function @ref HAL_RTC_RegisterCallback() to register an interrupt callback.
  
  [..]
  Function @ref HAL_RTC_RegisterCallback() allows to register following callbacks:
    (+) AlarmAEventCallback          : RTC Alarm A Event callback.
    (+) Tamper1EventCallback         : RTC Tamper 1 Event callback.
    (+) MspInitCallback              : RTC MspInit callback.
    (+) MspDeInitCallback            : RTC MspDeInit callback.
  [..]	  
  This function takes as parameters the HAL peripheral handle, the Callback ID
  and a pointer to the user callback function.
  
  [..]
  Use function @ref HAL_RTC_UnRegisterCallback() to reset a callback to the default
  weak function.
  @ref HAL_RTC_UnRegisterCallback() takes as parameters the HAL peripheral handle,
  and the Callback ID.
  This function allows to reset following callbacks:
    (+) AlarmAEventCallback          : RTC Alarm A Event callback.
    (+) Tamper1EventCallback         : RTC Tamper 1 Event callback.
    (+) MspInitCallback              : RTC MspInit callback.
    (+) MspDeInitCallback            : RTC MspDeInit callback.
  [..]
  By default, after the @ref HAL_RTC_Init() and when the state is HAL_RTC_STATE_RESET,
  all callbacks are set to the corresponding weak functions :
  example @ref AlarmAEventCallback().
  Exception done for MspInit and MspDeInit callbacks that are reset to the legacy weak function
  in the @ref HAL_RTC_Init()/@ref HAL_RTC_DeInit() only when these callbacks are null
  (not registered beforehand).
  If not, MspInit or MspDeInit are not null, @ref HAL_RTC_Init()/@ref HAL_RTC_DeInit()
  keep and use the user MspInit/MspDeInit callbacks (registered beforehand)
  [..]
  Callbacks can be registered/unregistered in HAL_RTC_STATE_READY state only.
  Exception done MspInit/MspDeInit that can be registered/unregistered
  in HAL_RTC_STATE_READY or HAL_RTC_STATE_RESET state,
  thus registered (user) MspInit/DeInit callbacks can be used during the Init/DeInit.
  In that case first register the MspInit/MspDeInit user callbacks
  using @ref HAL_RTC_RegisterCallback() before calling @ref HAL_RTC_DeInit()
  or @ref HAL_RTC_Init() function.
  [..]
  When The compilation define USE_HAL_RTC_REGISTER_CALLBACKS is set to 0 or
  not defined, the callback registration feature is not available and all callbacks
  are set to the corresponding weak functions.
   @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */


/*

============================================================================
    #####如何使用此驱动程序#####
=================================================================
[..]
（+）启用RTC域访问（请参阅上一节中的描述）。
（+）配置RTC预分频器（异步预分频器以生成RTC 1Hz时基）
使用HAL_RTC_Init（）函数。

***时间和日期配置***
==================================
[..]
（+）要配置RTC日历（时间和日期），请使用HAL_RTC_SetTime（）和HAL_RTC_SetDate（）函数。
（+）要读取RTC日历，请使用HAL_RTC_GetTime（）和HAL_RTC_GetDate（）函数。

***警报配置***
==========================
[..]
（+）要配置RTC警报，请使用HAL_RTC_SetAlarm（）函数。您还可以使用HAL_RTC_SetAlarm_IT（）函数以中断模式配置RTC警报。
（+）要读取RTC警报，请使用HAL_RTC_GetAlarm（）函数。

***篡改配置***
===========================
[..]
（+）启用RTC篡改并使用HAL_RTCEx_SetTamper（）函数配置篡改级别。您可以使用HAL_RTCEx_SetTamper_IT（）函数以中断模式配置RTC篡改。
（+）可以将TAMPER1备用功能映射到PC13

***备份数据寄存器配置***
==========================================
[..]
（+）要写入RTC备份数据寄存器，请使用HAL_RTCEx_BKUPWrite（）函数。
（+）要读取RTC备份数据寄存器，请使用HAL_RTCEx_BKUPRead（）函数。

#####警告：驱动程序限制#####
=================================================================
[..] STM32F1系列上使用的RTC版本是V1版本。 F1将不支持V2（其他系列）支持的所有功能。
[..]与V2一样，主要的RTC功能由硬件管理。但是在F1上，日期功能完全由SW管理。
[..]然后，与其他家庭相比，存在一些限制：
（+）HAL仅支持24小时格式（不支持12小时格式）
（+）日期保存在SRAM中。然后，当MCU处于STOP或STANDBY模式时，日期将丢失。
用户应在进入低功耗模式之前实现一种保存日期的方式（基于备份寄存器的固件包提供了一个示例）
（+）每次调用HAL_RTC_GetTime或HAL_RTC_GetDate时，日期都会自动更新。
（+）警报检测仅限于1天。它只会过期1次（无警报重复，需要编写新警报）

#####备份域的运行状况#####
============================================================================
[..]当主VDD电源关闭时，实时时钟（RTC）和RTC备份寄存器可以由VBAT电压供电。
 为了保留RTC备份寄存器的内容并在VDD关闭时提供RTC，可以将VBAT引脚连接到由电池或其他电源提供的可选待机电压。

[..]为了即使在关闭主数字电源（VDD）的情况下也允许RTC运行，VBAT引脚为以下模块供电：
（＃）RTC
（＃）LSE振荡器
（＃）启用低功耗备用稳压器时的备用SRAM
（＃）PC13至PC15 I / O，以及PI8 I / O（如果有）

[..]当备份域由VDD提供（模拟开关连接到VDD）时，以下引脚可用：
（+）PC13可用作防拆针

[..]当备份域由VBAT提供（由于不存在VDD而将模拟开关连接到VBAT）时，以下引脚可用：
（+）PC13可用作防拆针

 #####备份域重置#####
=================================================================
[..]备份域重置将所有RTC寄存器和RCC_BDCR寄存器设置为其重置值。
[..]发生以下事件之一时，将生成备份域重置：
（＃）通过将RCC备份域控制寄存器（RCC_BDCR）中的BDRST位置1触发软件复位。
（＃）如果先前已关闭两个电源，则VDD或VBAT上电。
（＃）篡改检测事件将重置所有数据备份寄存器。



#####备份域重置#####
===============================================================
[..]备份域重置将所有RTC寄存器和RCC_BDCR寄存器设置为其重置值。
[..]发生以下事件之一时，将生成备份域重置：
（＃）通过将RCC备份域控制寄存器（RCC_BDCR）中的BDRST位置1触发软件复位。
（＃）如果先前已关闭两个电源，则VDD或VBAT上电。
（＃）篡改检测事件将重置所有数据备份寄存器。
 #####备份域访问#####
===============================================================
[..]重置后，备份域（RTC寄存器，RTC备份数据
 寄存器和备份SRAM）受到保护，以防止可能的不需要的写访问。
[..]要启用对RTC域和RTC寄存器的访问，请执行以下操作：
（+）使用RCC_PERIPHCLK_RTC进行PeriphClockSelection调用函数HAL_RCCEx_PeriphCLKConfig并选择RTCClockSelection（LSE，LSI或HSE）
（+）使用__HAL_RCC_BKP_CLK_ENABLE（）启用BKP时钟
##### RTC和低功耗模式#####
================================================================
[..]可以通过RTC备用功能从低功耗模式唤醒MCU。
[..] RTC备用功能是RTC警报（警报A）和RTC篡改事件检测。这些RTC备用功能可以将系统从停止和待机低功耗模式中唤醒。
[..]通过使用RTC警报，系统也可以从低功耗模式中唤醒，而无需依赖外部中断（自动唤醒模式）。

***回调注册***
=============================================
[..]编译定义USE_HAL_RTC_REGISTER_CALLBACKS设置为1时，允许用户动态配置驱动程序回调。使用函数@ref HAL_RTC_RegisterCallback（）注册中断回调。
[..]函数@ref HAL_RTC_RegisterCallback（）允许注册以下回调：
（+）AlarmAEventCallback：RTC警报事件回调。
（+）Tamper1EventCallback：RTC篡改1事件回调。
（+）MspInitCallback：RTC MspInit回调。
（+）MspDeInitCallback：RTC MspDeInit回调。
[..]该函数将HAL外设句柄，回调ID和指向用户回调函数的指针作为参数。
[..]使用函数@ref HAL_RTC_UnRegisterCallback（）将回调重置为默认的弱函数。
@ref HAL_RTC_UnRegisterCallback（）将HAL外设句柄和回调ID作为参数。
此函数允许重置以下回调：
（+）AlarmAEventCallback：RTC警报事件回调。
（+）Tamper1EventCallback：RTC篡改1事件回调。
（+）MspInitCallback：RTC MspInit回调。
（+）MspDeInitCallback：RTC MspDeInit回调。
[..]
默认情况下，在@ref HAL_RTC_Init（）之后且状态为HAL_RTC_STATE_RESET时，所有回调均设置为相应的弱函数：示例@ref AlarmAEventCallback（）。
仅当这些回调为null（未预先注册）时，MspInit和MspDeInit回调才被重置为@ref HAL_RTC_Init（）/ @ ref HAL_RTC_DeInit（）中的传统弱函数的异常完成。
如果不是，则MspInit或MspDeInit不为null，@ ref HAL_RTC_Init（）/ @ ref HAL_RTC_DeInit（）保留并使用用户MspInit / MspDeInit回调（已预先注册）。
可以在HAL_RTC_STATE_READY或HAL_RTC_STATE_RESET状态下注册/注销的MspInit / MspDeInit异常已完成，因此可以在Init / DeInit期间使用已注册（用户）的MspInit / DeInit回调。
在这种情况下，在调用@ref HAL_RTC_DeInit（）或@ref HAL_RTC_Init（）函数之前，请先使用@ref HAL_RTC_RegisterCallback（）注册MspInit / MspDeInit用户回调。
[..]当编译定义USE_HAL_RTC_REGISTER_CALLBACKS设置为0或未定义时，回调注册功能不可用，并且所有回调均设置为相应的弱函数。

*/
/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include "rtc.h"
#include <stdio.h>
#include  "clk.h"
#include "clk_test.h"
#include "clk_cmd_priv.h"


/*
*************************************************************************************************
=================================================================================================

（1） 初始化µC / Clk。如果该过程成功，则将启动µC / Clk任务，并初始化其各种数据。

（2） 建立日期/时间结构。

（3） 取得2010年的星期几，10月18日。

（4） 验证日期/时间结构字段。

（5） 用日期/时间结构设置时钟模块的当前时间戳。

（6） 通过日期/时间结构获取带格式的字符串。

（7） 将日期/时间结构转换为时间戳。

（8） 设置时钟时区。

（9） 将当前的单击日期/时间转换为日期/时间结构。

（10） 通过日期/时间结构获取穿孔的字符串。

（11） 获取时钟模块的当前时间戳。

（12） 将时间戳转换为日期/时间结构。

（13） 获取当前Clock时间戳作为Unix时间戳。

（14） 将Unix时间戳转换为日期/时间结构。

（15） 通过日期/时间结构获取带格式的字符串。

（16）  Unix时间戳等效于1974年1月1日23:59:59 UTC + 0

（17） 将Unix时间戳转换为日期/时间结构。

（18） 通过日期/时间结构获取格式化的字符串。

*************************************************************************************************
=================================================================================================
*/



extern CPU_INT16S  TerminalSerial_Wr (void        *pbuf,
                               CPU_SIZE_T   buf_len);

static const char CLOCK_INIT_OK[] = "Clock module successfully initialized\n\r";
static const char CLOCK_INIT_ER[] = "Clock module initialized failed\n\r";
static const char CLOCK_MAKE_ER[] = "Clock make date/time failed\n\r";
static const char CLOCK_NOT_VALID[] ="Clock date/time not valid\n\r";
static const char CLOCK_SET_FAIL[] = "Clock set date/time failed\r\n";

#define SEND_MAX_SIZE           128u
void AppTaskStart(void *p_arg)
{
    char sendBuf[SEND_MAX_SIZE];
    uint16_t sendLen = 0u;

    CLK_TS_SEC ts_sec = 0u;
    CLK_TS_SEC ts_unix_sec;
    CLK_TS_SEC tz_sec = 0u;
    CLK_DATE_TIME date_time;
    CPU_BOOLEAN valid;
    CPU_CHAR    str[128];
    CLK_ERR     err;

    ( void )p_arg;


    Clk_Init(&err);
    if (err == CLK_ERR_NONE) {
        TerminalSerial_Wr((void *)CLOCK_INIT_OK , sizeof(CLOCK_INIT_OK));
    } else {
        TerminalSerial_Wr((void *)CLOCK_INIT_ER, sizeof(CLOCK_INIT_ER));
        return;
    }

    tz_sec = 0u;
    valid = Clk_DateTimeMake(&date_time, 2020, 5, 11, 8, 12, 13, tz_sec);
    if (valid != DEF_OK) {
        TerminalSerial_Wr((void *)CLOCK_MAKE_ER, sizeof(CLOCK_MAKE_ER));
        return;
    }
    date_time.Yr        = 2020;
    date_time.Month     =     5;
    date_time.Day       =     11;
    date_time.DayOfWk   =  Clk_GetDayOfWk(2020, 5, 11);
    date_time.DayOfYr   = 291;
    date_time.Hr        =   8;
    date_time.Min       =   12;
    date_time.Sec       =   13;
    date_time.TZ_sec    = tz_sec;
    valid               = Clk_IsDateTimeValid(&date_time);
    if (valid != DEF_OK) {
        TerminalSerial_Wr((void *)CLOCK_NOT_VALID, sizeof(CLOCK_NOT_VALID));
        return;
    }
    
    valid = Clk_SetDateTime(&date_time);                                                      //(5)
    if (valid != DEF_OK) {
//        printf("Clock set date/time failed\r\n");
        TerminalSerial_Wr((void *)CLOCK_SET_FAIL, sizeof(CLOCK_SET_FAIL));
        return;
    }
  
    valid = Clk_DateTimeToStr(&date_time, CLK_STR_FMT_YYYY_MM_DD_HH_MM_SS_UTC, str, 128);   //  (6)
    if (valid == DEF_OK) {
        sendLen = sprintf(sendBuf, "Current Date/time :%s\r\n", str);
        TerminalSerial_Wr((void *)sendBuf, sendLen);
    } else {
        sendLen = sprintf(sendBuf, "Clock date/time to string failed\r\n");
        TerminalSerial_Wr((void *)sendBuf, sendLen);
        return;
    }
   
    Clk_DateTimeToTS(&ts_sec, &date_time);                                                    //(7)
    if (valid == DEF_OK) {
        sendLen = sprintf(sendBuf, "Clock timestamp = %u\r\n", ts_sec);
        TerminalSerial_Wr((void *)sendBuf, sendLen);
    } else {
        sendLen = sprintf(sendBuf, "Clock date/time to timestamp failed\r\n");
        TerminalSerial_Wr((void *)sendBuf, sendLen);
        return;
    }
      
    tz_sec = (-5 * 60 * 60);
    valid = Clk_SetTZ(tz_sec);                                                               // (8)
    if (valid != DEF_OK) {
        sendLen  = sprintf(sendBuf, "Clock set timezone unix failed\r\n");
        TerminalSerial_Wr((void *)sendBuf, sendLen);
        return;
    }
         
    valid = Clk_GetDateTime(&date_time);                                                   //   (9)
    if (valid != DEF_OK) {
        sendLen  = sprintf(sendBuf, "Clock get date/time failed\r\n");
        TerminalSerial_Wr((void *)sendBuf, sendLen);
        return;
    }
      
    valid = Clk_DateTimeToStr(&date_time, CLK_STR_FMT_YYYY_MM_DD_HH_MM_SS_UTC, str, 128); //   (10)
    if (valid == DEF_OK) {
        sendLen  = sprintf(sendBuf, "Current Date/time :%s\r\n", str);
        TerminalSerial_Wr((void *)sendBuf, sendLen);
    } else {
        sendLen  = sprintf(sendBuf, "Clock date/time to string failed\r\n");
        TerminalSerial_Wr((void *)sendBuf, sendLen);
        return;
    }
    ts_sec = Clk_GetTS();                                                                    //(11)
    sendLen  = sprintf(sendBuf, "Clock timestamp = %u\r\n", ts_sec);
    TerminalSerial_Wr((void *)sendBuf, sendLen);
     

    valid = Clk_TS_ToDateTime(ts_sec, 0, &date_time);                                      //  (12)
    if (valid != DEF_OK) {
        sendLen  = sprintf(sendBuf, "Clock convert timestamp to date/time failed\r\n");
        TerminalSerial_Wr((void *)sendBuf, sendLen);
        return;
    }
      
    valid = Clk_GetTS_Unix(&ts_unix_sec);                                                   // (13)
    if (valid != DEF_OK) {
        sendLen  = sprintf(sendBuf, "Clock get timestamp unix failed\r\n");
        TerminalSerial_Wr((void *)sendBuf, sendLen);
        return;
    }
      
    valid = Clk_TS_UnixToDateTime(ts_unix_sec, tz_sec, &date_time);                       //   (14)
    if (valid != DEF_OK) {
        sendLen  = sprintf(sendBuf, "Clock timestamp unix to date/time failed\r\n");
        return;
    }
  
    valid = Clk_DateTimeToStr(&date_time, CLK_STR_FMT_YYYY_MM_DD_HH_MM_SS_UTC, str, 128);  //  (15)
    if (valid == DEF_OK) {
        sendLen  = sprintf(sendBuf, "Current Date/time :%s\r\n", str);
        TerminalSerial_Wr((void *)sendBuf, sendLen);
    } else {
        sendLen  = sprintf(sendBuf, "Clock date/time to string failed\r\n");
        TerminalSerial_Wr((void *)sendBuf, sendLen);
        return;
    }
  
    ts_unix_sec = 126316799uL;
    valid      = Clk_TS_UnixToDateTime(ts_unix_sec, tz_sec, &date_time);                  //   (16)
    if (valid != DEF_OK) {
        sendLen  = sprintf(sendBuf, "Clock set date/time failed\r\n");
        return;
    }
  
    valid = Clk_DateTimeToStr(&date_time, CLK_STR_FMT_DAY_MONTH_DD_HH_MM_SS_YYYY, str, 128); //(17)
    if (valid == DEF_OK) {
        sendLen  = sprintf(sendBuf, "Unix timestamp = %s", str);
        TerminalSerial_Wr((void *)sendBuf, sendLen);
    } else {
        sendLen  = sprintf(sendBuf, "Clock date/time to string failed\r\n");
        TerminalSerial_Wr((void *)sendBuf, sendLen);
        return;
    }

    sendLen  = sprintf(sendBuf, "\r\n*****Clock date/time set succeed*******\r\n");
    TerminalSerial_Wr((void *)sendBuf, sendLen);
    Mem_Clr((void *)sendBuf, SEND_MAX_SIZE);
    
    valid = Clk_DateTimeToStr(&date_time, CLK_STR_FMT_YYYY_MM_DD_HH_MM_SS_UTC, str, 128);   //  (6)
    if (valid == DEF_OK) {
        sendLen = sprintf(sendBuf, "Current Date/time :%s\r\n", str);
        TerminalSerial_Wr((void *)sendBuf, sendLen);
    } else {
        sendLen = sprintf(sendBuf, "Clock date/time to string failed\r\n");
        TerminalSerial_Wr((void *)sendBuf, sendLen);
        return;
    }

}






/*
*********************************************************************************************************
*                                            Clk_OS_Task()
*
* Description : OS-dependent shell task to schedule & run Clock Task handler.
*
*               (1) Shell task's primary purpose is to schedule & run Clk_TaskHandler() forever;
*                   (i.e. shell task should NEVER exit).
*
*
* Argument(s) : p_data      Pointer to task initialization data (required by uC/OS-III).
*
* Return(s)   : none.
*
* Created by  : Clk_OS_Init().
*
*               This function is an INTERNAL Clock function & MUST NOT be called by application function(s).
*
* Note(s)     : (2) To prevent deadlocking any lower priority task(s), Clock Task SHOULD delay (for a brief)
*                   time after task handler exits.
*********************************************************************************************************
*/

 void  Clk_OS_Task (void  *p_data)
{
//    OS_ERR  os_err;


   (void)p_data;                                               /* Prevent 'variable unused' compiler warning.          */


//    HAL_RTCEx_SetSecond_IT(&hrtc);

//    while (DEF_ON) {
//        Clk_TaskHandler();
//        OSTimeDly((OS_TICK ) 1u,                                /* Dly for lower prio task(s) [see Note #2].            */
//                  (OS_OPT  ) OS_OPT_TIME_DLY,
//                  (OS_ERR *)&os_err);
//    }

}
 
void  Clk_OS_Init (CLK_ERR  *p_err)
 {
    HAL_StatusTypeDef status;
    
    ( void )p_err;

    status = HAL_RTCEx_SetSecond_IT(&hrtc);
    if (HAL_OK == status) {
      *p_err = CLK_OS_ERR_NONE;
    } else {
        *p_err = CLK_OS_ERR_WAIT;
    }

}




void display_time(void)
{
    char sendBuf[SEND_MAX_SIZE];
    uint16_t sendLen = 0u;

    CLK_TS_SEC ts_sec = 0u;
    CLK_TS_SEC ts_unix_sec;
    CLK_TS_SEC tz_sec = 0u;
    CLK_DATE_TIME date_time;
    CPU_BOOLEAN valid;
    CPU_CHAR    str[128];
    CLK_ERR     err;

    Mem_Clr((void *)sendBuf, SEND_MAX_SIZE);

//    valid = Clk_SetDateTime(&date_time);                                                      //(5)
//    if (valid != DEF_OK) {
//        TerminalSerial_Wr((void *)CLOCK_SET_FAIL, sizeof(CLOCK_SET_FAIL));
//        return;
//    }
// 
    ts_sec = Clk_GetTS();                                                                    //(11)
    sendLen  = sprintf(sendBuf, "Clock timestamp = %u\r\n", ts_sec);
    TerminalSerial_Wr((void *)sendBuf, sendLen);
     

    valid = Clk_TS_ToDateTime(ts_sec, 0, &date_time);                                      //  (12)
    if (valid != DEF_OK) {
        sendLen  = sprintf(sendBuf, "Clock convert timestamp to date/time failed\r\n");
        TerminalSerial_Wr((void *)sendBuf, sendLen);
        return;
    }

    valid = Clk_DateTimeToStr(&date_time, CLK_STR_FMT_YYYY_MM_DD_HH_MM_SS, str, 128);   //  (6)
    if (valid == DEF_OK) {
        sendLen = sprintf(sendBuf, "Current Date/time :%s\r\n", str);
        TerminalSerial_Wr((void *)sendBuf, sendLen);
    } else {
        sendLen = sprintf(sendBuf, "Clock date/time to string failed\r\n");
        TerminalSerial_Wr((void *)sendBuf, sendLen);
        return;
    }
}

