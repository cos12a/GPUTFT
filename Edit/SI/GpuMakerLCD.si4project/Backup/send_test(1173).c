#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "cmsis_os.h"


typedef struct sendTxtCMD
{
    const int8_t *pTxt;
    uint8_t txtLen;
    
}TestTxtCmd;
    
const int8_t help[] = "1help\r";
const int8_t tab1[] = "2Test\r";
const int8_t tab2[] = "3last\r";
const int8_t tab3[] = "?\r";

#define CMD_COUNT      (sizeof(sendCMD)/sizeof(sendCMD[0]))
#define ADD_CMD_QUANTITY                4


void RxReceive(void *argument)
{
    const TestTxtCmd sendCMD[ADD_CMD_QUANTITY] = 
    {
        {help, sizeof(help)},
        {tab1, sizeof(tab1)},
        {tab2, sizeof(tab2)},
        {tab3, sizeof(tab3)}

    };
    static uint8_t cmdCnt = 0u;
    

    (void)argument;

//    for(;;);

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
    }



}

