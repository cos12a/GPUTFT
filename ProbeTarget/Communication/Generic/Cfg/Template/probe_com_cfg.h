/*
*********************************************************************************************************
*                                      uC/Probe Communication
*
*                         (c) Copyright 2007-2008; Micrium, Inc.; Weston, FL
*
*               All rights reserved.  Protected by international copyright laws.
*               Knowledge of the source code may NOT be used to develop a similar product.
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*********************************************************************************************************
*/
#if 0

GENERAL COMMUNICATION CONFIGURATION
注释
（1）将PROBE_COM_CFG_RX_MAX_SIZE和ROBE_COM_CFG_TX_MAX_SIZE分别配置为最大接收和发送数据包大小。最大的最大数据�
�大小将导致更有效的通信以及更好的吞吐量。通常，最大的接收和发送数据包将具有可比较的大小。
（2）配置PROBE_COM_CFG_WR_REQ_EN以启用/
禁用写请求可用性。如果禁用，则将不包括用于处理写入请求的代码（允许Windows应用程序写入目标内存位
置）。
（3）配置PROBE_COM_CFG_STR_REQ_EN以启用/
禁用字符串请求可用性。如果禁用，则不包括用于处理字符串请求的代码，并且字符串读/
写接口功能将不可用。
    （a）将PROBE_COM_CFG_STR_IN_BUF_SIZE配置为字符串输入缓冲区的长度（以字节为单位）。
    （b）将PROBE_COM_CFG_STR_OUT_BUF_SIZE配置为字符串输出缓冲区的长度（以字节为单位）。
（4）配置PROBE_COM_CFG_TERMINAL_REQ_EN以启用/
禁用终端请求可用性。如果禁用，则不会编译用于处理终端请求的代码，并且终端接口功能将不可用。
（5）配置PROBE_COM_CFG_STAT_EN以启用禁用统计信息/计数器。

配置最大接收数据包大小（请参阅注释1）。
配置最大发送数据包大小（请参阅注释1）。
配置写请求可用性（请参阅注释2）。

配置字符串请求可用性（请参阅注释3）。
配置字符串输入buf的大小（请参见注释＃3a）。
配置字符串输出buf的大小（请参见注释＃3b）。
配置终端请求可用性...（请参阅注释4）。
配置统计信息/计数器（请参阅注释5）。


RS-232 COMMUNICATION CONFIGURATION
（1）将PROBE_RS232_CFG_RX_BUF_SIZE和ROBE_RS232_CFG_TX_BUF_SIZE分别配置为接收和发送缓冲区大小。这些实际上是最大�
��接收和可传输数据包的大小，因此通常应分别配置为PROBE_COM_CFG_RX_MAX_SIZE和PROBE_COM_CFG_TX_MAX_SIZE。
（2）配置PROBE_RS232_CFG_PARSE_TASK_EN以启用/
禁用任务级别的接收数据包解析。如果启用，将创建一个任务来解析收到的数据包。如果禁用，则将在中�
��级别解析数据包。
    （a）将PROBE_RS232_CFG_TASK_PRIO配置为解析任务的优先级。
    （b）将PROBE_RS232_CFG_TASK_STK_SIZE配置为解析任务的堆栈大小。
（3）将PROBE_RS232_CFG_COMM_SEL配置为所选的UART或串行通信接口。

配置接收缓冲区的大小（请参阅注释1）。
配置发送缓冲区的大小（请参阅注释1）。
配置解析任务的存在（请参阅注释2）。
配置解析任务优先级（请参阅注释＃2a）。
配置解析任务堆栈大小（请参阅注释＃2b）。


#endif


/*
*********************************************************************************************************
*
*                                    COMMUNICATION: CONFIGURATION
*
* Filename      : probe_com_cfg.h
* Version       : V2.30
* Programmer(s) : BAN
*********************************************************************************************************
* Note(s)       : (1) This file contains configuration constants for uC/Probe Communication Modules.
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                 COMMUNICATION METHOD CONFIGURATION
*********************************************************************************************************
*/

#define  PROBE_COM_CFG_RS232_EN                    DEF_ENABLED  /* Configure RS-232 communication availability.         */
#define  PROBE_COM_CFG_TCPIP_EN                    DEF_DISABLED  /* Configure TCP/IP communication availability.         */
#define  PROBE_COM_CFG_USB_EN                      DEF_DISABLED  /* Configure USB    communication availability.         */


/*
*********************************************************************************************************
*                                 GENERAL COMMUNICATION CONFIGURATION
*
* Note(s) : (1) Configure PROBE_COM_CFG_RX_MAX_SIZE & PROBE_COM_CFG_TX_MAX_SIZE to the maximum receive
*               & transmit packet sizes, respectively.  Larger maximum packet sizes will result in more
*               efficient communication with better throughput.  The largest receive & transmit packets
*               will be of comparable size, typically.
*
*           (2) Configure PROBE_COM_CFG_WR_REQ_EN to enable/disable write request availability.  If
*               disabled, code to handle write requests (which allowing the Windows application to
*               write target memory locations) will not be included.
*
*           (3) Configure PROBE_COM_CFG_STR_REQ_EN to enable/disable string request availability.  If
*               disabled, code to handle string requests will not be included, and the string read/write
*               interface functions will not be available.
*
*               (a) Configure PROBE_COM_CFG_STR_IN_BUF_SIZE to the length of the string input buffer, in
*                   bytes.
*
*               (b) Configure PROBE_COM_CFG_STR_OUT_BUF_SIZE to the length of the string output buffer,
*                   in bytes.
*
*           (4) Configure PROBE_COM_CFG_TERMINAL_REQ_EN to enable/disable terminal request availability.
*               If disabled, code to handle terminal requests will not be compiled, and the terminal
*               interface functions will not be available.
*
*           (5) Configure PROBE_COM_CFG_STAT_EN to enable disable statistics/counters.
*********************************************************************************************************
*/

#define  PROBE_COM_CFG_RX_MAX_SIZE                       256    /* Configure maximum receive  packet size (see Note #1).*/
#define  PROBE_COM_CFG_TX_MAX_SIZE                       256    /* Configure maximum transmit packet size (see Note #1).*/

#define  PROBE_COM_CFG_WR_REQ_EN                   DEF_ENABLED  /* Configure write request availability (see Note #2).  */

#define  PROBE_COM_CFG_STR_REQ_EN                  DEF_ENABLED  /* Configure string request availability (see Note #3). */
#define  PROBE_COM_CFG_STR_IN_BUF_SIZE                   128    /* Configure size of string input  buf   (see Note #3a).*/
#define  PROBE_COM_CFG_STR_OUT_BUF_SIZE                 2048    /* Configure size of string output buf   (see Note #3b).*/

#define  PROBE_COM_CFG_TERMINAL_REQ_EN             DEF_ENABLED  /* Configure terminal request availability ...          */
                                                                /* ... (see Note #4).                                   */

#define  PROBE_COM_CFG_STAT_EN                     DEF_ENABLED  /* Configure statistics/counters (see Note #5).         */


/*
*********************************************************************************************************
*                                 RS-232 COMMUNICATION CONFIGURATION
*
* Note(s) : (1) Configure PROBE_RS232_CFG_RX_BUF_SIZE & PROBE_RS232_CFG_TX_BUF_SIZE to the receive &
*               transmit buffer sizes, respectively.  These are, effectively, the sizes of the maximum
*               receivable & transmittable packets, so should generally be configured to
*               PROBE_COM_CFG_RX_MAX_SIZE & PROBE_COM_CFG_TX_MAX_SIZE, respectively.
*
*           (2) Configure PROBE_RS232_CFG_PARSE_TASK_EN to enable/disable parsing of received packets at
*               task level.  If enabled, a task will be created to parse received packets.  If disabled,
*               packets will be parsed at interrupt-level.
*
*               (a) Configure PROBE_RS232_CFG_TASK_PRIO to the priority of the parse task.
*
*               (b) Configure PROBE_RS232_CFG_TASK_STK_SIZE to the stack size of the parse task.
*
*           (3) Configure PROBE_RS232_CFG_COMM_SEL to the UART or serial communication interface selected.
*********************************************************************************************************
*/

#define  PROBE_RS232_CFG_RX_BUF_SIZE  PROBE_COM_CFG_RX_MAX_SIZE /*  Configure receive  buffer size (see Note #1).       */
#define  PROBE_RS232_CFG_TX_BUF_SIZE  PROBE_COM_CFG_TX_MAX_SIZE /*  Configure transmit buffer size (see Note #1).       */

#define  PROBE_RS232_CFG_PARSE_TASK_EN             DEF_ENABLED  /*  Configure parse task presence   (see Note #2).      */
#define  PROBE_RS232_CFG_TASK_PRIO                        10    /*  Configure parse task priority   (see Note #2a).     */
#define  PROBE_RS232_CFG_TASK_STK_SIZE                   128    /*  Configure parse task stack size (see Note #2b).     */

#define  PROBE_RS232_UART_0                                1
#define  PROBE_RS232_UART_1                                2
#define  PROBE_RS232_UART_2                                3
#define  PROBE_RS232_UART_3                                4
#define  PROBE_RS232_UART_4                                5
#define  PROBE_RS232_UART_5                                6
#define  PROBE_RS232_UART_6                                7
#define  PROBE_RS232_UART_7                                8
#define  PROBE_RS232_UART_8                                9
#define  PROBE_RS232_UART_9                               10
#define  PROBE_RS232_UART_DBG                             63
#define  PROBE_RS232_CFG_COMM_SEL         PROBE_RS232_UART_2    /* Configure UART selection (see Note #3).              */


/*
*********************************************************************************************************
*                                 TCP/IP COMMUNICATION CONFIGURATION
*
* Note(s) : (1) Configure PROBE_TCPIP_CFG_RX_BUF_SIZE & PROBE_TCPIP_CFG_TX_BUF_SIZE to the receive &
*               transmit buffer sizes, respectively.  These are, effectively, the sizes of the maximum
*               receivable & transmittable packets, so should generally be configured to
*               PROBE_COM_CFG_RX_MAX_SIZE & PROBE_COM_CFG_TX_MAX_SIZE, respectively.
*
*           (2) (a) Configure PROBE_TCPIP_CFG_TASK_PRIO to the priority of the server task.
*
*               (b) Configure PROBE_TCPIP_CFG_TASK_STK_SIZE to the stack size of the server task.
*
*           (3) Configure PROBE_TCPIP_CFG_PORT to the port on which the server listens for packets.
*********************************************************************************************************
*/

#define  PROBE_TCPIP_CFG_RX_BUF_SIZE  PROBE_COM_CFG_RX_MAX_SIZE /*  Configure receive  buffer size (see Note #1).       */
#define  PROBE_TCPIP_CFG_TX_BUF_SIZE  PROBE_COM_CFG_TX_MAX_SIZE /*  Configure transmit buffer size (see Note #1).       */

#define  PROBE_TCPIP_CFG_TASK_PRIO                        11    /*  Configure parse task priority   (see Note #2a).     */
#define  PROBE_TCPIP_CFG_TASK_STK_SIZE                   512    /*  Configure parse task stack size (see Note #2b).     */

#define  PROBE_TCPIP_CFG_PORT                           9930    /* Configure listening port of server (see Note #3).    */

/*
*********************************************************************************************************
*                                  USB COMMUNICATION CONFIGURATION
*
* Note(s) : (1) Configure PROBE_USB_CFG_RX_BUF_SIZE & PROBE_USB_CFG_TX_BUF_SIZE to the receive &
*               transmit buffer sizes, respectively.  These are, effectively, the sizes of the maximum
*               receivable & transmittable packets, so should generally be configured to
*               PROBE_COM_CFG_RX_MAX_SIZE & PROBE_COM_CFG_TX_MAX_SIZE, respectively.
*
*           (2) (a) Configure PROBE_USB_CFG_TASK_PRIO to the priority of the server task.
*
*               (b) Configure PROBE_USB_CFG_TASK_STK_SIZE to the stack size of the server task.
*********************************************************************************************************
*/


#define  PROBE_USB_CFG_RX_BUF_SIZE    PROBE_COM_CFG_RX_MAX_SIZE /*  (a) Set Rx buffer size.                             */
#define  PROBE_USB_CFG_TX_BUF_SIZE    PROBE_COM_CFG_TX_MAX_SIZE /*  (b) Set Tx buffer size.                             */

#define  PROBE_USB_CFG_TX_TIMEOUT_MS                    100u
#define  PROBE_USB_CFG_RX_TIMEOUT_MS                    100u

#define  PROBE_USB_CFG_TASK_PRIO                          12    /*  (c) Set task priority.                              */
#define  PROBE_USB_CFG_TASK_STK_SIZE                     256    /*  (d) Set task stack size.                            */

#define  PROBE_USB_CFG_INIT_STACK                  DEF_TRUE     /*  (e) The USB comm module will init stack.            */
