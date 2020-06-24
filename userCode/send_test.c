#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "cmsis_os.h"
#include "common_err.h"

#include "clk_cmd_priv.h"
#include "usart.h"

typedef struct sendTxtCMD
{
    const int8_t *pTxt;
    uint8_t txtLen;
    
}TestTxtCmd;




#define ADD_CMD_QUANTITY                5u
    
const int8_t help[] = "help\r";
const int8_t tab1[] = "2Test\r";
const int8_t tab2[] = "3last\r";
const int8_t tab3[] = "?\r";
const int8_t clkGet[] = "clk_get\r";

const int8_t GPU_Test[] = "DR2;CLS(0);DS24(4,0,'错误 ',1);BOS(0,30,319,130,11);\r\n";


#define CMD_COUNT      (sizeof(sendCMD)/sizeof(sendCMD[0]))

extern void CLK_AppTaskStart(void * p_arg);
extern void display_time(void);
extern HAL_StatusTypeDef GPU_tx_and_rx_hand(void         *pbuf,
                                   uint16_t    buf_len);
extern void receive_processing_task(void *pvParameters);
extern void stream_send_data( void );
extern void vAFunction( void );

void RxReceive(void *argument)
{
    RTOS_ERR err;
    const TestTxtCmd sendCMD[ADD_CMD_QUANTITY] = 
    {
        {help, sizeof(help)},
        {tab1, sizeof(tab1)},
        {tab2, sizeof(tab2)},
        {tab3, sizeof(tab3)},
        {clkGet, sizeof(clkGet)}

    };
    static uint8_t cmdCnt = 0u;
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 8000ul );
        
    (void)argument;
    
    osDelay( xTicksToWait );
#if !defined( RING_BUF_TEST )
    ClkCmd_Init(&err);
    
//同一个命令再次加入,检测是否存在问题.(不会出问题Shell_CmdTblAdd函数已经做了判断)
//    ClkCmd_Init(&err);      

    
    CLK_AppTaskStart(NULL);
#endif
    vAFunction();


    for(;;)
    {
      osDelay(8000);
      
#if !defined( RING_BUF_TEST )
extern  StreamBufferHandle_t Rx2StreamBuffer;
     size_t sendCnt = xStreamBufferSend( Rx2StreamBuffer,
                          ( const void * ) sendCMD[cmdCnt].pTxt ,
                          sendCMD[cmdCnt].txtLen,
                          portMAX_DELAY );
     cmdCnt ++;
     if ( ADD_CMD_QUANTITY <= cmdCnt) 
     {
        cmdCnt = 0u;
        
     }
    osDelay(2000);
    GPU_tx_and_rx_hand((void *)GPU_Test, sizeof( GPU_Test ));
    display_time();
    
#endif

    
    stream_send_data();
    
    receive_processing_task(NULL);

    
    
    }

}

