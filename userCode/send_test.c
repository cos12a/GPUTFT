#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "cmsis_os.h"
#include "common_err.h"

#include "clk_cmd_priv.h"

typedef struct sendTxtCMD
{
    const int8_t *pTxt;
    uint8_t txtLen;
    
}TestTxtCmd;
    
const int8_t help[] = "1help\r";
const int8_t tab1[] = "2Test\r";
const int8_t tab2[] = "3last\r";
const int8_t tab3[] = "?\r";

const int8_t GPU_Test[] = "DR2;CLS(0);DS24(4,0,'错误 ',1);BOS(0,30,319,130,11);\r\n";


#define CMD_COUNT      (sizeof(sendCMD)/sizeof(sendCMD[0]))
#define ADD_CMD_QUANTITY                4

extern void AppTaskStart(void * p_arg);
extern void display_time(void);

void RxReceive(void *argument)
{
    RTOS_ERR err;
    const TestTxtCmd sendCMD[ADD_CMD_QUANTITY] = 
    {
        {help, sizeof(help)},
        {tab1, sizeof(tab1)},
        {tab2, sizeof(tab2)},
        {tab3, sizeof(tab3)}

    };
    static uint8_t cmdCnt = 0u;
    const TickType_t xTicksToWait = pdMS_TO_TICKS( 8000ul );
        
    (void)argument;
    
    osDelay( xTicksToWait );
    
    ClkCmd_Init(&err);
    
    AppTaskStart(NULL);

    for(;;)
    {
      osDelay(8000);
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
    GPU_tx_and_rx_hand(GPU_Test, sizeof( GPU_Test ));
    display_time();
    }

}

