#include "ring_buf_priv.h"

#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "cmsis_os.h"
#include "common_err.h"

#include "stdint.h"
#include "task.h"


// 方式,将要接收和发送的数据存入环形队列中,然后调用串口进行接收与发送.
// 队列中先预装入一些数据,然后慢慢调用取出数据.
// 先用随机数产生一堆数据.
// 再发送出去.
// 用RTOS的流方式就可以了.


static uint8_t rxBuff[128];

static RING_BUF rxRing_buf;
static void *receive_demode_processing(void *pData, uint16_t length);


//static StreamBufferHandle_t Rx2StreamBuffer = NULL;
static StreamBufferHandle_t  streamReceiveData = NULL;

#define sbiSTREAM_BUFFER_LENGTH_BYTES		    ( ( size_t ) 64 )
#define sbiSTREAM_BUFFER_TRIGGER_LEVEL_2	    ( ( BaseType_t ) 1 )

#define RING_STREAM_TEST_SIZE                   ( ( size_t ) 64 )
#define RECEIVE_DATA_LENGTH                        8u
#define MAX_SEND_BUF_SIZE                       ( RING_STREAM_TEST_SIZE / RECEIVE_DATA_LENGTH )


/* Used to log the status of the tests contained within this file for reporting
to a monitoring task ('check' task). */
static BaseType_t xErrorStatus = pdPASS;









static void rx_init(void)
{
    RTOS_ERR err;
    RingBufCreate(  &rxRing_buf,
                   128ul,
                   &err);

}






/*-----------------------------------------------------------*/

static void prvCheckExpectedState( BaseType_t xState )
{
	configASSERT( xState );
	if( xState == pdFAIL )
	{
		xErrorStatus = pdFAIL;
	}
}
/*-----------------------------------------------------------*/

const char ringErr[] = "Ring create is fail.\r\n";
const char ringOK[] = "Ring create is OK.\r\n";

void vAFunction( void )
{
    const size_t xStreamBufferSizeBytes = RING_STREAM_TEST_SIZE, xTriggerLevel = 8u;

    /* Create a stream buffer that can hold 100 bytes.  The memory used to hold
    both the stream buffer structure and the data in the stream buffer is
    allocated dynamically. */
    streamReceiveData = xStreamBufferCreate( xStreamBufferSizeBytes, RECEIVE_DATA_LENGTH );

    if( streamReceiveData == NULL )
    {
    /* There was not enough heap memory space available to create the
    stream buffer. */
        TerminalSerial_Wr((void *)ringErr, sizeof(ringErr)); 
        while(1);
    }
    else
    {
        TerminalSerial_Wr((void *)ringOK, sizeof(ringOK));
    /* The stream buffer was created successfully and can now be used. */
    }
    
    rx_init();

    
}



#if 0
void can_receiveCallBack_isr(void)
{
    BaseType_t const xHigherPriorityTaskWoken = pdFALSE;


    /*
    Generate recognisable data to write to the buffer.  This is just
    ascii characters that shows which loop iteration the data was written
    in. The 'FromISR' version is used to give it some exercise as a block
    time is not used, so the call must be inside a critical section so it
    runs with ports that don't support interrupt nesting (and therefore
    don't have interrupt safe critical sections). 
    */
    memset( ( void * ) pucData, ( ( int ) '0' ) + ( int ) xItem, x6ByteLength );
    taskENTER_CRITICAL();
    {
        xReturned = xStreamBufferSendFromISR( streamReceiveData, ( void * ) pucData, RECEIVE_DATA_LENGTH, &xHigherPriorityTaskWoken );
    }
    taskEXIT_CRITICAL();
    prvCheckExpectedState( xReturned == x6ByteLength );




}
#endif

const char ringAdd[] = "Send stream Data onece.\r\n";

void stream_send_data( void )
{

    static uint8_t cnt = 0u;
    uint8_t pcStringToSend[RECEIVE_DATA_LENGTH];
    size_t ux;
    const TickType_t sbDONT_BLOCK  = portMAX_DELAY;

    for ( uint8_t i = 0; i < MAX_SEND_BUF_SIZE; i++) {

        /* Try sending two bytes to the buffer. Since the size of the
        * buffer is one, we must not be able to send more than one. */
        memset( ( void * ) pcStringToSend, cnt, RECEIVE_DATA_LENGTH );
        cnt++;
        cnt &= 0xff;
        ux = xStreamBufferSend( streamReceiveData, ( void * ) pcStringToSend, ( size_t ) RECEIVE_DATA_LENGTH, sbDONT_BLOCK );
        /* Ensure that only one byte was sent. */
        configASSERT( ux == RECEIVE_DATA_LENGTH );
        TerminalSerial_Wr((void *)ringAdd, sizeof(ringAdd));

    }

}

#if 0

{

TickType_t xTimeOnEntering;


const TickType_t xTicksToWait = pdMS_TO_TICKS( 100UL );

/* The task should now have a notification pending, and so not time out. */
	xTimeOnEntering = xTaskGetTickCount();
/* Should have blocked for the entire block time. */
if( ( xTaskGetTickCount() - xTimeOnEntering ) < xTicksToWait )
{
    xErrorStatus = pdFAIL;
}

}
#endif


const char ringReceive[] = "Receive stream Data onece.\r\n";


void receive_processing_task(void *pvParameters)
{

    uint8_t pcReceivedString[RECEIVE_DATA_LENGTH];
    size_t xReceivedLength;
    TickType_t xTicksToBlock;

    /* Remove compiler warnings about unused parameters. */
    ( void ) pvParameters;

    for ( uint8_t i = 0; i < MAX_SEND_BUF_SIZE; i++) {

        memset( pcReceivedString, 0x00, RECEIVE_DATA_LENGTH );
        
        /* Has any data been sent by the client? */
        xReceivedLength = xStreamBufferReceive( streamReceiveData, ( void * ) pcReceivedString, RECEIVE_DATA_LENGTH, portMAX_DELAY );
        /* Should always receive data as max delay was used. */
    //    prvCheckExpectedState( xReceivedLength > 0 );
    //    prvCheckExpectedState( ( ( TickType_t ) ( xTaskGetTickCount() - xTimeOnEntering ) ) >= xTicksToBlock );

        receive_demode_processing(pcReceivedString, xReceivedLength);       // 数据解析处理

        RingBufWr(&rxRing_buf, RECEIVE_DATA_LENGTH, pcReceivedString);
        TerminalSerial_Wr((void *)ringReceive, sizeof(ringReceive));
    }

}



// 数据解析处理
static void *receive_demode_processing(void *pData, uint16_t length)
{

    // 如果是新的请求数据,将数据包装后发给shell exece.
    // 如果是显示更新数据,将数据发给显示更新.
    // 如果是其它请求,则将数据交给Shell 执行.
//    if ( pData[0] == '?')

}


// 创建事件处理函数
// 数据刷新处理.每间隔0.5sec刷新一次.

// 分发事件.


