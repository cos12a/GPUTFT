#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "cmsis_os.h"
#include "common_err.h"

#include "clk_cmd_priv.h"
#include "usart.h"
#include "Ring_buf_priv.h"

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

//const int8_t GPU_Test[] = "DR2;CLS(0);DS24(4,0,'错误 ',1);BOS(0,30,319,130,11);\r\n";


#define CMD_COUNT      (sizeof(sendCMD)/sizeof(sendCMD[0]))

extern void CLK_AppTaskStart(void * p_arg);
extern void display_time(void);
extern HAL_StatusTypeDef GPU_tx_and_rx_hand(void         *pbuf,
                                   uint16_t    buf_len);
extern void receive_processing_task(void *pvParameters);
extern void stream_send_data( void );
extern void vAFunction( void );
//extern  CPU_BOOLEAN  RingCmd_Init (void);
 
void RxReceive(void *argument)
{
//    void( *argument );
    RingCmd_Init();
    for(;;)
    {
      osDelay(8000);
    }

}


