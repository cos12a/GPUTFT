#include "Ring_buf_priv.h"

#include "FreeRTOS.h"
#include "stream_buffer.h"
#include "cmsis_os.h"
#include "common_err.h"

#include "stdint.h"
#include "task.h"


#include "lib_def.h"
#include  "lib_mem.h"
#include  "lib_math.h"
#include  "lib_str.h"

#include "shell.h"
#include "sh_shell.h"

#include "toolchains.h"



/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL DEFINES
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RING_CMD_ARG_BEGIN                  ASCII_CHAR_HYPHEN_MINUS
#define  RING_CMD_ARG_TIME_TYPE              ASCII_CHAR_LATIN_LOWER_T

#define  RING_CMD_NBR_MIN_PER_HR             60u
#define  RING_CMD_NBR_SEC_PER_MIN            60u

#define  RTOS_MODULE_CUR                    RTOS_CFG_MODULE_COMMON

/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL CONSTANTS
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RING_CMD_HELP                                        ("help")
#define  RING_CMD_HELP_SHORT                                  ("h")

#define  RING_CMD_FORMAT_DATETIME                             ("datetime")
#define  RING_CMD_FORMAT_NTP                                  ("ntp")
#define  RING_CMD_FORMAT_UNIX                                 ("unix")

#define  RING_CMD_FORMAT_DATETIME_SHORT                       ("d")
#define  RING_CMD_FORMAT_NTP_SHORT                            ("n")
#define  RING_CMD_FORMAT_UNIX_SHORT                           ("u")

#define  RING_CMD_OUTPUT_CMD_LIST                             ("Command List: ")
#define  RING_CMD_OUTPUT_ERR                                  ("Error: ")
#define  RING_CMD_OUTPUT_SUCCESS                              ("Completed successfully")
#define  RING_CMD_OUTPUT_TABULATION                           ("\t")

#define  RING_CMD_OUTPUT_ERR_CMD_ARG_INVALID                  ("Invalid Arguments")
#define  RING_CMD_OUTPUT_ERR_CMD_NOT_IMPLEMENTED              ("This command is not yet implemented")
#define  RING_CMD_OUTPUT_ERR_CONV_DISABLED                    ("RING_CFG_STR_CONV_EN is not enabled. Formatting not available.")
#define  RING_CMD_OUTPUT_ERR_CMD_INTERNAL_ERR                 ("Clk Internal Error. Date & Time cannot be converted.")

#define  RING_CMD_HELP_SET                                    ("usage: RING_set [VALUE] {FORMAT}\r\n")

#define  RING_CMD_HELP_GET                                    ("usage: RING_get {FORMAT}\r\n")

#define  RING_CMD_HELP_VALUE                                  ("where VALUE is:\r\n"                                                 \
                                                              "  YYYY-MM-DD {HH:MM:SS {UTC+/-HH:MM}}      for DATETIME format.\r\n" \
                                                              "  a 32-bit integer greater than 946684799  for UNIX     format.\r\n" \
                                                              "  a 32-bit integer greater than 3155673599 for NTP      format.\r\n")

#define  RING_CMD_HELP_FORMAT                                 ("where FORMAT is:\r\n"                        \
                                                              "\r\n"                                        \
                                                              "  -d, --datetime   DATETIME format.\r\n"     \
                                                              "  -u, --unix       UNIX     format.\r\n"     \
                                                              "  -n, --ntp        NTP      format.\r\n\r\n" \
                                                              "  if FORMAT is not provided, VALUE is assumed to be in DATETIME format.\r\n")


#define RING_WRITE_ERR                                        (CPU_CHAR *) ("Data write input Err.\r\n")
#define RING_WRITE_SUCCESS                                    (CPU_CHAR *) ("Data write input success.\r\n")
#define RING_CMD_WRITE_SHORT                                   ("w")
#define RING_CMD_WRITE                                          ("write")
#define RING_CMD_READ                                          ("read")
#define RING_CMD_READ_SHORT                                   ("r")

//#define  TERMINAL_ARG_ERR_FC            (CPU_CHAR *)("Term_fc: usage: Term_fc")

#define RING_CREATE_ERR                                     (CPU_CHAR *)("Ring create is fail.\r\n")
#define RING_CREATE_SUCCESS                                 (CPU_CHAR *)("Ring create is OK.\r\n")

#define CMD_UNFINISHED                                  (CPU_CHAR *)("Shell cmd is CMD_UNFINISHED.\r\n")

#define CMD_EXECUTE_SUCCESS                             (CPU_CHAR *)("The command execution is complete.\r\n")


/*
*********************************************************************************************************
*                                            LOCAL DATA TYPES
*********************************************************************************************************
*/



#define RING_STREAM_TEST_SIZE                   ( ( size_t ) 32 )
#define RECEIVE_DATA_LENGTH                        8u
#define MAX_SEND_BUF_SIZE                       ( RING_STREAM_TEST_SIZE / RECEIVE_DATA_LENGTH )


//static uint8_t rxBuff[RING_STREAM_TEST_SIZE];

static RING_BUF rxRing_buf;


/********************************************************************************************************
 ********************************************************************************************************
 *                                               DATA TYPES
 ********************************************************************************************************
 *******************************************************************************************************/

/*
 ********************************************************************************************************
 *                                       CLOCK COMMAND TIME DATA TYPE
 *******************************************************************************************************/

typedef enum RING_cmd_time_type {
  RING_CMD_TIME_TYPE_NONE = 0x00,
  RING_CMD_TIME_TYPE_DATETIME = 0x30,
  RING_CMD_TIME_TYPE_NTP = 0x31,
  RING_CMD_TIME_TYPE_UNIX = 0x32
} RING_CMD_TIME_TYPE;

/*
 ********************************************************************************************************
 *                                   CLOCK COMMAND ARGUMENT DATA TYPE
 *******************************************************************************************************/

typedef struct RING_cmd_arg {
  RING_CMD_TIME_TYPE TimeType;
  CPU_CHAR          *DatePtr;
  CPU_CHAR          *TimePtr;
  CPU_CHAR          *OffsetPtr;
} RING_CMD_ARG;


/*
 ********************************************************************************************************
 *                                   CLOCK COMMAND PARSE STATUS DATA TYPE
 *******************************************************************************************************/

typedef enum Ring_cmd_parse_status {
  RING_CMD_PARSE_STATUS_SUCCESS,
  RING_CMD_PARSE_STATUS_EMPTY,
  RING_CMD_PARSE_STATUS_INVALID_ARG,
  RING_CMD_PARSE_STATUS_HELP
} RING_CMD_PARSE_STATUS;



/*
*********************************************************************************************************
*                                         LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                        LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static CPU_INT16S ring_buf_init(CPU_INT16U      argc,
                                       CPU_CHAR        *p_argv[],
                                       SHELL_OUT_FNCT  out_fnct,
                                       SHELL_CMD_PARAM *p_cmd_param);

/********************************************************************************************************
 ********************************************************************************************************
 *                                       LOCAL FUNCTION PROTOTYPES
 ********************************************************************************************************
 *******************************************************************************************************/

static  CPU_INT16S  RingCmd_Write (CPU_INT16U        argc,
                                 CPU_CHAR         *argv[],
                                 SHELL_OUT_FNCT    out_fnct,
                                 SHELL_CMD_PARAM  *p_cmd_param);



static CPU_INT16S RingCmd_Help(CPU_INT16U      argc,
                              CPU_CHAR        *p_argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param);



static CPU_INT16S RingCmd_OutputError(CPU_CHAR        *p_error,
                                     SHELL_OUT_FNCT  out_fnct,
                                     SHELL_CMD_PARAM *p_cmd_param);


static CPU_INT16S RingCmd_OutputMsg(const CPU_CHAR  *p_msg,
                                   CPU_BOOLEAN     new_line_start,
                                   CPU_BOOLEAN     new_line_end,
                                   CPU_BOOLEAN     tab_start,
                                   SHELL_OUT_FNCT  out_fnct,
                                   SHELL_CMD_PARAM *p_cmd_param);


static  CPU_INT16S  Ring_Read (CPU_INT16U        argc,
                               CPU_CHAR         *p_argv[],
                               SHELL_OUT_FNCT    out_fnct,
                               SHELL_CMD_PARAM  *p_cmd_param);

static CPU_INT16S ring_buf_init(CPU_INT16U      argc,
                                       CPU_CHAR        *p_argv[],
                                       SHELL_OUT_FNCT  out_fnct,
                                       SHELL_CMD_PARAM *p_cmd_param);

static RING_CMD_PARSE_STATUS RingCmd_CmdArgParse(CPU_INT16U  argc,
                                                CPU_CHAR    *p_argv[],
                                                void *p_cmd_args);

static CPU_INT16S RingCmd_OutputCmdTbl(SHELL_CMD       *p_cmd_tbl,
                                      SHELL_OUT_FNCT  out_fnct,
                                      SHELL_CMD_PARAM *p_cmd_param);

static CPU_INT16S ring_test_dlist(CPU_INT16U        argc,
                                   CPU_CHAR         *p_argv[],
                                   SHELL_OUT_FNCT    out_fnct,
                                   SHELL_CMD_PARAM  *p_cmd_param);




/********************************************************************************************************
 ********************************************************************************************************
 *                                               LOCAL TABLES
 ********************************************************************************************************
 *******************************************************************************************************/

static SHELL_CMD Ring_CmdTbl [] =
{
  { "ring_help", RingCmd_Help },
  { "ring_write", RingCmd_Write },
  { "ring_get", Ring_Read },
  { "ring_init", ring_buf_init },
  { "ring_test", ring_test_dlist},
  { 0, 0 }
};


/********************************************************************************************************
 ********************************************************************************************************
 *                                                   MACRO
 ********************************************************************************************************
 *******************************************************************************************************/

#define  RING_CMD_OUT_MSG_CHK(out_val, cur_out_cnt, exit_fail_label)     do { \
    switch (out_val) {                                                       \
      case SHELL_OUT_RTN_CODE_CONN_CLOSED:                                   \
      case SHELL_OUT_ERR:                                                    \
        out_val = SHELL_EXEC_ERR;                                            \
        goto exit_fail_label;                                                \
                                                                             \
                                                                             \
      default:                                                               \
        cur_out_cnt += out_val;                                              \
        break;                                                               \
    }                                                                        \
} while (0)


/*
*********************************************************************************************************
*                                       LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                             ShShell_Init()
*
* Description : Initialize Shell for general shell commands.
*
* Argument(s) : none.
*
* Return(s)   : DEF_OK,   if general shell commands were added.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  RingCmd_Init (void)
{
    SHELL_ERR    err;
    CPU_BOOLEAN  ok;


    Shell_CmdTblAdd((CPU_CHAR *)"ring", Ring_CmdTbl, &err);

    ok = (err == SHELL_ERR_NONE) ? DEF_OK : DEF_FAIL;
    return (ok);
}

/****************************************************************************************************//**
 *                                               RingCmd_Help()
 *
 * @brief    Output the available commands.
 *
 * @param    argc            Count of the arguments supplied.
 *
 * @param    p_argv          Array of pointers to the strings which are those arguments.
 *
 * @param    out_fnct        Callback to a respond to the requester.
 *
 * @param    p_cmd_param     Pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO error(s).
 *               - SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed.
 *               - SHELL_OUT_ERR, otherwise.
 *******************************************************************************************************/


static CPU_INT16S ring_buf_init(CPU_INT16U      argc,
                                       CPU_CHAR        *p_argv[],
                                       SHELL_OUT_FNCT  out_fnct,
                                       SHELL_CMD_PARAM *p_cmd_param)
{

    CPU_INT16S           ret_val = 0u;
    CPU_INT16S           byte_out_cnt = 0u;
    RING_CMD_PARSE_STATUS parse_status;    
    
    parse_status = RingCmd_CmdArgParse(argc,
                                         p_argv,
                                         NULL);


    switch (parse_status) {
      case RING_CMD_PARSE_STATUS_EMPTY:
      case RING_CMD_PARSE_STATUS_SUCCESS:
        break;
    
      case RING_CMD_PARSE_STATUS_INVALID_ARG:
        ret_val = RingCmd_OutputError((CPU_CHAR *)RING_CMD_OUTPUT_ERR_CMD_ARG_INVALID,
                                     out_fnct,
                                     p_cmd_param);
    
        RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);
        goto exit_ok;
    
      case RING_CMD_PARSE_STATUS_HELP:
      default:
        ret_val = RingCmd_OutputMsg(RING_CMD_HELP_GET,
                                   DEF_YES,
                                   DEF_YES,
                                   DEF_NO,
                                   out_fnct,
                                   p_cmd_param);
    
        RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);
    
        ret_val = RingCmd_OutputMsg(RING_CMD_HELP_FORMAT,
                                   DEF_YES,
                                   DEF_YES,
                                   DEF_NO,
                                   out_fnct,
                                   p_cmd_param);
    
        RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);
        goto exit_ok;
    }

    RTOS_ERR err;    
    RingBufCreate(  &rxRing_buf,
                   RING_STREAM_TEST_SIZE,
                   &err);
    
    if (err != RTOS_ERR_NONE){
        ret_val = RingCmd_OutputError((CPU_CHAR *)RING_CREATE_ERR,
                                         out_fnct,
                                         p_cmd_param);
            
        RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);
        goto exit_ok;

    } else {
        ret_val = RingCmd_OutputMsg(RING_CREATE_SUCCESS,
                                      DEF_YES,
                                      DEF_YES,
                                      DEF_NO,
                                      out_fnct,
                                      p_cmd_param);
           
        RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);


    }


exit_ok:
    ret_val = byte_out_cnt;

exit_fail:
    return (ret_val);

}






/********************************************************************************************************
 ********************************************************************************************************
 *                                           LOCAL FUNCTIONS
 ********************************************************************************************************
 *******************************************************************************************************/

/****************************************************************************************************//**
 *                                               RingCmd_Help()
 *
 * @brief    Output the available commands.
 *
 * @param    argc            Count of the arguments supplied.
 *
 * @param    p_argv          Array of pointers to the strings which are those arguments.
 *
 * @param    out_fnct        Callback to a respond to the requester.
 *
 * @param    p_cmd_param     Pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO error(s).
 *               - SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed.
 *               - SHELL_OUT_ERR, otherwise.
 *******************************************************************************************************/
static CPU_INT16S RingCmd_Help(CPU_INT16U      argc,
                              CPU_CHAR        *p_argv[],
                              SHELL_OUT_FNCT  out_fnct,
                              SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S ret_val;

  PP_UNUSED_PARAM(argc);
  PP_UNUSED_PARAM(p_argv);

  ret_val = RingCmd_OutputCmdTbl(Ring_CmdTbl,
                                        out_fnct,
                                        p_cmd_param);

  return (ret_val);
}



static  CPU_INT16S  RingCmd_Write (CPU_INT16U        argc,
                                 CPU_CHAR         *p_argv[],
                                 SHELL_OUT_FNCT    out_fnct,
                                 SHELL_CMD_PARAM  *p_cmd_param)
{

    CPU_INT16S           ret_val = 0u;
    CPU_INT16S           byte_out_cnt = 0u;
    CPU_BOOLEAN          success;
    RING_CMD_PARSE_STATUS parse_status;
    void * p_param = NULL;
    
    parse_status = RingCmd_CmdArgParse(argc,
                                         p_argv,
                                         p_param);

    switch (parse_status) {
      case RING_CMD_PARSE_STATUS_SUCCESS:
        break;
    
      case RING_CMD_PARSE_STATUS_INVALID_ARG:
        ret_val = RingCmd_OutputError((CPU_CHAR *)RING_CMD_OUTPUT_ERR_CMD_ARG_INVALID,
                                     out_fnct,
                                     p_cmd_param);
    
        RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);
        goto exit_ok;
        
      case RING_CMD_PARSE_STATUS_EMPTY:
      case RING_CMD_PARSE_STATUS_HELP:
      default:
        ret_val = RingCmd_OutputMsg(RING_CMD_HELP_GET,
                                   DEF_YES,
                                   DEF_YES,
                                   DEF_NO,
                                   out_fnct,
                                   p_cmd_param);
    
        RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);
    
        ret_val = RingCmd_OutputMsg(RING_CMD_HELP_FORMAT,
                                   DEF_YES,
                                   DEF_YES,
                                   DEF_NO,
                                   out_fnct,
                                   p_cmd_param);
    
        RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);
        goto exit_ok;
    }
    
//    success = RingBufWr(&rxRing_buf,
//                         Str_Len((void *)p_param),
//                         p_param);
    success = DEF_OK;
    if (success == DEF_OK) {
        ret_val = RingCmd_OutputMsg(RING_WRITE_SUCCESS,
                                      DEF_YES,
                                      DEF_YES,
                                      DEF_NO,
                                      out_fnct,
                                      p_cmd_param);

        RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);
        
        ret_val = RingCmd_OutputMsg((char *)p_param,
                                      DEF_YES,
                                      DEF_YES,
                                      DEF_NO,
                                      out_fnct,
                                      p_cmd_param);

        RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);    
    } else {
        ret_val = RingCmd_OutputMsg(RING_WRITE_ERR,
                                      DEF_YES,
                                      DEF_YES,
                                      DEF_NO,
                                      out_fnct,
                                      p_cmd_param);
        
        RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);        
        goto exit_fail;
    }
    

exit_ok:
    ret_val = byte_out_cnt;

exit_fail:
    return (ret_val);

}

static char Test_data = 0u;
static char Test_Flag = 0u;
static CPU_INT16S ring_test_dlist(CPU_INT16U        argc,
                               CPU_CHAR         *p_argv[],
                               SHELL_OUT_FNCT    out_fnct,
                               SHELL_CMD_PARAM  *p_cmd_param)
{

    CPU_INT16S           ret_val = 0u;
    CPU_INT16S           byte_out_cnt = 0u;
    //    CPU_BOOLEAN          success;
    RING_CMD_PARSE_STATUS parse_status;
    CPU_INT16S           read_len;

    parse_status = RingCmd_CmdArgParse(argc,
                                          p_argv,
                                          (void *)&read_len);
    if ( ++Test_data > 100u){
        Test_data = 0u;
    }
    
    Test_Flag++;
    Test_Flag &= 1u;

    ret_val = RingCmd_OutputMsg(CMD_EXECUTE_SUCCESS,
                                  DEF_YES,
                                  DEF_YES,
                                  DEF_NO,
                                  out_fnct,
                                  p_cmd_param);

    RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);
    goto exit_ok;



exit_ok:
    
exit_fail: 
    return(1u);
}


static  CPU_INT16S  Ring_Read (CPU_INT16U        argc,
                               CPU_CHAR         *p_argv[],
                               SHELL_OUT_FNCT    out_fnct,
                               SHELL_CMD_PARAM  *p_cmd_param)
{
    CPU_INT16S           ret_val = 0u;
    CPU_INT16S           byte_out_cnt = 0u;
//    CPU_BOOLEAN          success;
    RING_CMD_PARSE_STATUS parse_status;
    CPU_INT16S           read_len;
    
    parse_status = RingCmd_CmdArgParse(argc,
                                      p_argv,
                                      (void *)&read_len);

    switch (parse_status) {
      
      case RING_CMD_PARSE_STATUS_SUCCESS:
        break;
    
      case RING_CMD_PARSE_STATUS_INVALID_ARG:
        ret_val = RingCmd_OutputError((CPU_CHAR *)RING_CMD_OUTPUT_ERR_CMD_ARG_INVALID,
                                     out_fnct,
                                     p_cmd_param);
    
        RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);
        goto exit_ok;
      case RING_CMD_PARSE_STATUS_EMPTY:
      case RING_CMD_PARSE_STATUS_HELP:
      default:
        ret_val = RingCmd_OutputMsg(RING_CMD_HELP_GET,
                                   DEF_YES,
                                   DEF_YES,
                                   DEF_NO,
                                   out_fnct,
                                   p_cmd_param);
    
        RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);
    
        ret_val = RingCmd_OutputMsg(RING_CMD_HELP_FORMAT,
                                   DEF_YES,
                                   DEF_YES,
                                   DEF_NO,
                                   out_fnct,
                                   p_cmd_param);
    
        RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);
        goto exit_ok;
    }

    ret_val = RingCmd_OutputMsg(CMD_UNFINISHED,
                                      DEF_YES,
                                      DEF_YES,
                                      DEF_NO,
                                      out_fnct,
                                      p_cmd_param);
        
    RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);        
    goto exit_fail;

exit_ok:
    ret_val = byte_out_cnt;

exit_fail:
    return (ret_val);
    
}


  




/****************************************************************************************************//**
*                                           RingCmd_CmdArgParse()
*
* @brief    Parse and validate the argument for a clock test command.
*
* @param    argc        Count of the arguments supplied.
*
* @param    p_argv      Array of pointers to the strings which are those arguments.
*
* @param    p_cmd_args  Pointer to structure that will be filled during parse operation.
*
* @return   Clock command parse status:
*               - RING_CMD_PARSE_STATUS_SUCCESS
*               - RING_CMD_PARSE_STATUS_EMPTY
*               - RING_CMD_PARSE_STATUS_INVALID_ARG
*               - RING_CMD_PARSE_STATUS_HELP
*
* @note
*******************************************************************************************************/
static RING_CMD_PARSE_STATUS RingCmd_CmdArgParse(CPU_INT16U  argc,
                                                            CPU_CHAR    *p_argv[],
                                                            void *p_cmd_args)
{
    CPU_INT16U i;
//    CPU_INT16U arg_caught = 0u;

//    PP_UNUSED_PARAM(p_cmd_args);

    //    p_cmd_args->TimeType = RING_CMD_TIME_TYPE_NONE;
    //    p_cmd_args->DatePtr = DEF_NULL;
    //    p_cmd_args->TimePtr = DEF_NULL;
    //    p_cmd_args->OffsetPtr = DEF_NULL;

    if (argc == 1) {
      return (RING_CMD_PARSE_STATUS_EMPTY);
    }

    for (i = 1u; i < argc; i++) {
      if (*p_argv[i] == RING_CMD_ARG_BEGIN) {
        if (*(p_argv[i] + 1) == RING_CMD_ARG_BEGIN) {              // --option type argument.
          if (Str_Cmp(p_argv[i] + 2, RING_CMD_HELP) == 0) {
            return (RING_CMD_PARSE_STATUS_HELP);
          } else if (Str_Cmp(p_argv[i] + 2, RING_CMD_READ) == 0) {
//            *p_cmd_args = Str_Len(p_argv[i]);
            return (RING_CMD_PARSE_STATUS_SUCCESS);
          } else if (Str_Cmp(p_argv[i] + 2, RING_CMD_WRITE) == 0) {
            if (argc > 2){                   // 如果长度大于2,表明后面跟有数据.
                return (RING_CMD_PARSE_STATUS_SUCCESS);
            } else {
                return (RING_CMD_PARSE_STATUS_HELP);
            }
          } else {
            return (RING_CMD_PARSE_STATUS_INVALID_ARG);
          }
        } else {                                                  // -o type argument.
          if (Str_Cmp(p_argv[i] + 1, RING_CMD_READ_SHORT) == 0){
//            CPU_INT16S strLen = Str_Len(p_argv[i] + 1);
//            p_cmd_args = (void *)strLen;
            return(RING_CMD_PARSE_STATUS_SUCCESS);
          } else if (Str_Cmp(p_argv[i] + 1, RING_CMD_WRITE_SHORT) == 0) {
//            p_cmd_args = p_argv;
            if (argc > 1){                              // 如果长度大于1,表明后面跟有数据.
                return (RING_CMD_PARSE_STATUS_SUCCESS);
            } else {
                return (RING_CMD_PARSE_STATUS_HELP);
            }
          } else if (Str_Cmp(p_argv[i] + 1, RING_CMD_HELP_SHORT) == 0) {
            return (RING_CMD_PARSE_STATUS_HELP);
          } else {
            return (RING_CMD_PARSE_STATUS_INVALID_ARG);
          }
        }
      } 
    }  
    return (RING_CMD_PARSE_STATUS_SUCCESS);
}



/****************************************************************************************************//**
 *                                           RingCmd_OutputCmdTbl()
 *
 * @brief    Format and output the clock test command table
 *
 * @param    p_cmd_tbl       is the pointer on the pointer table
 *
 * @param    out_fnct        is a callback to a respond to the requester.
 *
 * @param    p_cmd_param     is a pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO error(s).
 *               - SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed.
 *               - SHELL_OUT_ERR, otherwise.
 *******************************************************************************************************/
static CPU_INT16S RingCmd_OutputCmdTbl(SHELL_CMD       *p_cmd_tbl,
                                      SHELL_OUT_FNCT  out_fnct,
                                      SHELL_CMD_PARAM *p_cmd_param)
{
  SHELL_CMD  *p_shell_cmd;
  CPU_INT16S ret_val;
  CPU_INT16S acc_ret_val;

  ret_val = RingCmd_OutputMsg(RING_CMD_OUTPUT_CMD_LIST,
                             DEF_YES,
                             DEF_YES,
                             DEF_NO,
                             out_fnct,
                             p_cmd_param);
  switch (ret_val) {
    case SHELL_OUT_RTN_CODE_CONN_CLOSED:
    case SHELL_OUT_ERR:
      return (SHELL_EXEC_ERR);

    default:
      break;
  }

  acc_ret_val = ret_val;
  p_shell_cmd = p_cmd_tbl;

  while (p_shell_cmd->Fnct != 0) {
    ret_val = RingCmd_OutputMsg(p_shell_cmd->Name,
                               DEF_NO,
                               DEF_YES,
                               DEF_YES,
                               out_fnct,
                               p_cmd_param);
    switch (ret_val) {
      case SHELL_OUT_RTN_CODE_CONN_CLOSED:
      case SHELL_OUT_ERR:
        return (SHELL_EXEC_ERR);

      default:
        break;
    }
    p_shell_cmd++;
    acc_ret_val += ret_val;
  }

  return (acc_ret_val);
}


/****************************************************************************************************//**
 *                                           RingCmd_OutputError()
 *
 * @brief    Outputs error message.
 *
 * @param    p_error         Pointer to a string describing the error.
 *
 * @param    out_fnct        Callback to a respond to the requester.
 *
 * @param    p_cmd_param     Pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO error(s).
 *               - SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed.
 *               - SHELL_OUT_ERR, otherwise.
 *******************************************************************************************************/
static CPU_INT16S RingCmd_OutputError(CPU_CHAR        *p_error,
                                     SHELL_OUT_FNCT  out_fnct,
                                     SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16S ret_val;
  CPU_INT16S byte_out_cnt = 0;

  ret_val = RingCmd_OutputMsg(RING_CMD_OUTPUT_ERR,
                             DEF_YES,
                             DEF_NO,
                             DEF_NO,
                             out_fnct,
                             p_cmd_param);

  RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);

  ret_val = RingCmd_OutputMsg(p_error,
                             DEF_NO,
                             DEF_YES,
                             DEF_NO,
                             out_fnct,
                             p_cmd_param);

  RING_CMD_OUT_MSG_CHK(ret_val, byte_out_cnt, exit_fail);

  ret_val = byte_out_cnt;

exit_fail:
  return (ret_val);
}


/****************************************************************************************************//**
 *                                           RingCmd_OutputMsg()
 *
 * @brief    Format and output a message.
 *
 * @param    p_msg           Pointer of char on the string to format and output.
 *
 * @param    new_line_start  If DEF_YES, will add a new line character at the start.
 *
 * @param    new_line_end    If DEF_YES, will add a new line character at the end.
 *
 * @param    tab_start       If DEF_YES, will add a tab character at the start.
 *
 * @param    out_fnct        Callback to a respond to the requester.
 *
 * @param    p_cmd_param     Pointer to additional information to pass to the command.
 *
 * @return   The number of positive data octets transmitted, if NO error(s).
 *               - SHELL_OUT_RTN_CODE_CONN_CLOSED, if implemented connection closed.
 *               - SHELL_OUT_ERR, otherwise.
 *******************************************************************************************************/
static CPU_INT16S RingCmd_OutputMsg(const CPU_CHAR  *p_msg,
                                   CPU_BOOLEAN     new_line_start,
                                   CPU_BOOLEAN     new_line_end,
                                   CPU_BOOLEAN     tab_start,
                                   SHELL_OUT_FNCT  out_fnct,
                                   SHELL_CMD_PARAM *p_cmd_param)
{
  CPU_INT16U output_len;
  CPU_INT16S output;
  CPU_INT16S byte_out_cnt = 0;

  if (new_line_start == DEF_YES) {
    output = out_fnct((CPU_CHAR *)STR_NEW_LINE,
                      STR_NEW_LINE_LEN,
                      p_cmd_param->pout_opt);

    RING_CMD_OUT_MSG_CHK(output, byte_out_cnt, exit_fail);
  }

  if (tab_start == DEF_YES) {
    output = out_fnct((CPU_CHAR *)RING_CMD_OUTPUT_TABULATION,
                      1,
                      p_cmd_param->pout_opt);

    RING_CMD_OUT_MSG_CHK(output, byte_out_cnt, exit_fail);
  }

  output_len = (CPU_INT16U)Str_Len(p_msg);
  output = out_fnct((CPU_CHAR *)p_msg,
                    output_len,
                    p_cmd_param->pout_opt);

  RING_CMD_OUT_MSG_CHK(output, byte_out_cnt, exit_fail);

  if (new_line_end == DEF_YES) {
    output = out_fnct((CPU_CHAR *)STR_NEW_LINE,
                      STR_NEW_LINE_LEN,
                      p_cmd_param->pout_opt);

    RING_CMD_OUT_MSG_CHK(output, byte_out_cnt, exit_fail);
  }

  output = byte_out_cnt;

exit_fail:
  return (output);
}

