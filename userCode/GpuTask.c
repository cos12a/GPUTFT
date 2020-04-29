#include "GpuTask.h"



#include  <terminal.h>


/*
*********************************************************************************************************
*                                            LOCAL DEFINES
*********************************************************************************************************
*/


#define  TERMINAL_NEW_LINE              (CPU_CHAR *)"\r\n"
#define  TERMINAL_BLANK_LINE            (CPU_CHAR *)"\r\n\r\n"
#define  TERMINAL_STR_HELP              (CPU_CHAR *)"-h"

/*
*********************************************************************************************************
*                                       ARGUMENT ERROR MESSAGES
*********************************************************************************************************
*/

#define  TERMINAL_ARG_ERR_FC            (CPU_CHAR *)"Term_fc: usage: Term_fc"

/*
*********************************************************************************************************
*                                    COMMAND EXPLANATION MESSAGES
*********************************************************************************************************
*/

#define  TERMINAL_CMD_EXP_FC            (CPU_CHAR *)"                List terminal history items."


/*
*********************************************************************************************************
*                                           LOCAL CONSTANTS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                       LOCAL GLOBAL VARIABLES
*********************************************************************************************************
*/

#if (TERMINAL_CFG_HISTORY_EN == DEF_ENABLED)
static  CPU_CHAR     Terminal_History[TERMINAL_CFG_HISTORY_ITEMS_NBR][TERMINAL_CFG_HISTORY_ITEM_LEN];
static  CPU_INT16U   Terminal_HistoryIxFirst;
static  CPU_INT16U   Terminal_HistoryIxLast;
static  CPU_INT16U   Terminal_HistoryIxShown;
static  CPU_BOOLEAN  Terminal_HistoryShown;
static  CPU_BOOLEAN  Terminal_HistoryEmpty;
static  CPU_INT16U   Terminal_HistoryCnt;
#endif


/*
*********************************************************************************************************
*                                      LOCAL FUNCTION PROTOTYPES
*********************************************************************************************************
*/

static  CPU_INT16S  Terminal_OutFnct       (CPU_CHAR         *pbuf,
                                            CPU_INT16U        buf_len,
                                            void             *popt);

static  CPU_INT16S  Terminal_Help          (SHELL_OUT_FNCT    out_fnct,
                                            SHELL_CMD_PARAM  *pcmd_param);

#if (TERMINAL_CFG_HISTORY_EN == DEF_ENABLED)
static  void        Terminal_HistoryInit   (void);

static  void        Terminal_HistoryNextGet(CPU_CHAR         *pstr);

static  void        Terminal_HistoryPrevGet(CPU_CHAR         *pstr);

static  void        Terminal_HistoryPut    (CPU_CHAR         *pstr);

static  CPU_INT16S  Terminal_fc            (CPU_INT16U        argc,
                                            CPU_CHAR         *argv[],
                                            SHELL_OUT_FNCT    out_fnct,
                                            SHELL_CMD_PARAM  *pcmd_param);
#endif

/*
*********************************************************************************************************
*                                         SHELL COMMAND TABLE
*********************************************************************************************************
*/

#if (TERMINAL_CFG_HISTORY_EN == DEF_ENABLED)
static  SHELL_CMD  Terminal_CmdTbl [] = {
    {"Term_fc", Terminal_fc},
    {0,         0          }
};
#endif


/*
*********************************************************************************************************
*                                     LOCAL CONFIGURATION ERRORS
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                                           Terminal_Init()
*
* Description : Initialize terminal.
*
* Argument(s) : none.
*
* Return(s)   : DEF_OK,   if terminal was initialized.
*               DEF_FAIL, otherwise.
*
* Caller(s)   : Application.
*
* Note(s)     : none.
*********************************************************************************************************
*/

CPU_BOOLEAN  Terminal_Init (void)
{
    CPU_BOOLEAN  ok;


                                                                /* ------------------- INIT SERIAL IF ----------------- */
    ok = TerminalSerial_Init();
    if (ok == DEF_OK) {



                                                                /* ------------------ INIT OS SERVICES ---------------- */
        ok = Terminal_OS_Init((void *)0);
        if (ok != DEF_OK) {                                     /* If OS not init'd ...                                 */
            TerminalSerial_Exit();                              /* ... exit serial if.                                  */
        }
    }

    return (ok);
}


/*
*********************************************************************************************************
*                                           Terminal_Task()
*
* Description : Terminal task.
*
* Argument(s) : p_arg       Argument passed to the task (ignored).
*
* Return(s)   : none.
*
* Caller(s)   : Terminal OS port.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  Terminal_Task (void *p_arg)
{
    CPU_CHAR         cmd[TERMINAL_CFG_MAX_CMD_LEN + 1u];
    CPU_SIZE_T       cmd_len;
    SHELL_CMD_PARAM  cmd_param;
    CPU_INT16S       cmp_val;
    CPU_SIZE_T       cursor_pos;
    CPU_CHAR         cwd_path[TERMINAL_CFG_MAX_PATH_LEN + 1u];
    SHELL_ERR        err;
    CPU_INT08U       esc_type;
    CPU_BOOLEAN      ins;


    (void)p_arg;

                                                                /* --------------------- INIT VARS -------------------- */
    Mem_Set((void     *)&cwd_path[0],                           /* Clr cur working dir path.                            */
            (CPU_INT08U) 0x00u,
            (CPU_SIZE_T) TERMINAL_CFG_MAX_PATH_LEN);

    Str_Copy(cwd_path, (CPU_CHAR *)"\\");

    Mem_Set((void     *)&cmd[0],                                /* Clr cur line.                                        */
            (CPU_INT08U) 0x00u,
            (CPU_SIZE_T) TERMINAL_CFG_MAX_CMD_LEN);

#if (TERMINAL_CFG_HISTORY_EN == DEF_ENABLED)
    Terminal_HistoryInit();
    Shell_CmdTblAdd((CPU_CHAR *)"Term", Terminal_CmdTbl, &err);
#endif

    cursor_pos                 = 0u;
    cmd_len                    = 0u;
    ins                        = DEF_NO;

    cmd_param.pcur_working_dir = (void *)cwd_path;
    cmd_param.pout_opt         = (void *)0;

    TerminalMode_Prompt();                                      /* Show first prompt.                                   */




    while (DEF_TRUE) {
                                                                /* -------------------- RD NEW LINE ------------------- */
        esc_type = TerminalMode_RdLine(&cmd[0],
                                        TERMINAL_CFG_MAX_CMD_LEN,
                                       &cursor_pos,
                                        ins);
        cmd_len  = Str_Len(&cmd[0]);


        switch (esc_type) {
#if (TERMINAL_CFG_HISTORY_EN == DEF_ENABLED)
            case TERMINAL_ESC_TYPE_UP:                          /* ------------- MOVE TO PREV HISTORY ITEM ------------ */
                 TerminalMode_Clr(cmd_len, cursor_pos);         /* Clr terminal line.                                   */
                 Terminal_HistoryPrevGet(cmd);                  /* Get prev history item.                               */
                 cmd_len    = Str_Len(cmd);
                 cursor_pos = cmd_len;                          /* Cursor at end of line.                               */
                 Terminal_WrStr(cmd, cmd_len);                  /* Wr  prev history item to terminal.                   */
                 break;



            case TERMINAL_ESC_TYPE_DOWN:                        /* ------------- MOVE TO NEXT HISTORY ITEM ------------ */
                 TerminalMode_Clr(cmd_len, cursor_pos);         /* Clr terminal line.                                   */
                 Terminal_HistoryNextGet(cmd);                  /* Get next history item.                               */
                 cmd_len    = Str_Len(cmd);
                 cursor_pos = cmd_len;                          /* Cursor at end of line.                               */
                 Terminal_WrStr(cmd, cmd_len);                  /* Wr  next history item to terminal.                   */
                 break;
#else




            case TERMINAL_ESC_TYPE_UP:                          /* ---------------- UNSUPPORTED UP/DOWN --------------- */
            case TERMINAL_ESC_TYPE_DOWN:
                 TerminalMode_Clr(cmd_len, cursor_pos);         /* Clear line.                                          */
                 Str_Copy(cmd, (CPU_CHAR *)"");
                 break;
#endif



            case TERMINAL_ESC_TYPE_INS:                         /* ---------------- TOGGLE INSERT MODE ---------------- */
                 if (ins == DEF_YES) {
                     ins = DEF_NO;
                 } else {
                     ins = DEF_YES;
                 }
                 break;



            case TERMINAL_ESC_TYPE_NONE:                        /* --------------------- EXEC CMD --------------------- */
            default:
#if (TERMINAL_CFG_HISTORY_EN == DEF_ENABLED)
                 Terminal_HistoryPut(cmd);                      /* Put line into history.                               */
#endif

                 cmp_val = Str_Cmp(cmd, (CPU_CHAR *)"");
                 if (cmp_val != 0) {
                     TerminalMode_NewLine();                    /* Move to new line.                                    */

                     cmp_val = Str_Cmp(cmd, (CPU_CHAR *)"?");
                     if (cmp_val == 0) {
                        (void)Terminal_Help( Terminal_OutFnct,  /* List all cmds ...                                    */
                                            &cmd_param);


                     } else {
                        (void)Shell_Exec( cmd,                  /* ... OR exec cmd.                                     */
                                          Terminal_OutFnct,
                                         &cmd_param,
                                         &err);

                         switch (err) {
                             case SHELL_ERR_CMD_NOT_FOUND:
                             case SHELL_ERR_CMD_SEARCH:
                             case SHELL_ERR_ARG_TBL_FULL:
                                  Terminal_WrStr((CPU_CHAR *)"Command not found\r\n", 19);
                                  break;

                             case SHELL_ERR_NONE:
                             case SHELL_ERR_NULL_PTR:
                             case SHELL_ERR_CMD_EXEC:
                             default:
                                  break;
                         }
                     }
                 }



                                                                /* ------------------ DISP NEW PROMPT ----------------- */
                 TerminalMode_Prompt();                         /* Show new prompt.                                     */
                 Str_Copy(cmd, (CPU_CHAR *)"");                 /* Clear cmd.                                           */
                 cursor_pos = 0u;                               /* Cursor pos'd at beginning of line.                   */
                 break;
        }
    }
}


/*
*********************************************************************************************************
*                                          Terminal_WrStr()
*
* Description : Write string to terminal.
*
* Argument(s) : pbuf        Pointer to the buffer to transmit.
*
*               buf_len     Number of bytes in the buffer.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  Terminal_WrStr (CPU_CHAR    *pbuf,
                      CPU_SIZE_T   buf_len)
{
    TerminalSerial_Wr((void     *)pbuf,
                      (CPU_SIZE_T)buf_len);
}


/*
*********************************************************************************************************
*                                          Terminal_WrChar()
*
* Description : Write character to terminal.
*
* Argument(s) : c           Character to transmit.
*
* Return(s)   : none.
*
* Caller(s)   : various.
*
* Note(s)     : none.
*********************************************************************************************************
*/

void  Terminal_WrChar (CPU_CHAR  c)
{
    TerminalSerial_WrByte((CPU_INT08U)c);
}


/*
*********************************************************************************************************
*********************************************************************************************************
*                                           LOCAL FUNCTIONS
*********************************************************************************************************
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*                                          Terminal_OutFnct()
*
* Description : Out function used by Shell.
*
* Argument(s) : pbuf        Pointer to the buffer contianing data to send.
*
*               buf_len     Length of buffer.
*
*               popt        Pointer to options (unused).
*
* Return(s)   : Number of positive data octets transmitted.
*
* Caller(s)   : Shell, as a result of command execution in Terminal_Task().
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  CPU_INT16S  Terminal_OutFnct (CPU_CHAR    *pbuf,
                                      CPU_INT16U   buf_len,
                                      void        *popt)
{
    (void)popt;

    TerminalSerial_Wr((void     *)pbuf,
                      (CPU_SIZE_T)buf_len);

    return ((CPU_INT16S)buf_len);
}


/*
*********************************************************************************************************
*                                           Terminal_Help()
*
* Description : List all commands.
*
* Argument(s) : out_fnct    The output function.
*
*               pcmd_param  Pointer to the command parameters.
*
* Return(s)   : SHELL_EXEC_ERR, if an error is encountered.
*               SHELL_ERR_NONE, otherwise.
*
* Caller(s)   : Shell, in response to command execution.
*
* Note(s)     : none.
*********************************************************************************************************
*/

static  CPU_INT16S  Terminal_Help (SHELL_OUT_FNCT    out_fnct,
                                   SHELL_CMD_PARAM  *pcmd_param)
{
    SHELL_CMD         *pcmd;
    SHELL_MODULE_CMD  *pmodule_cmd;


    pmodule_cmd = Shell_ModuleCmdUsedPoolPtr;
    while (pmodule_cmd != (SHELL_MODULE_CMD *)0) {
        pcmd = pmodule_cmd->CmdTblPtr;
        if (pcmd != (SHELL_CMD *)0) {
            while (pcmd->Fnct != (SHELL_CMD_FNCT)0) {
                (void)out_fnct((CPU_CHAR *)pcmd->Name,
                               (CPU_INT16U)Str_Len(pcmd->Name),
                                           pcmd_param->pout_opt);
                (void)out_fnct(TERMINAL_NEW_LINE, 2,                               pcmd_param->pout_opt);
                pcmd++;
            }
        }
        pmodule_cmd = pmodule_cmd->NextModuleCmdPtr;
    }

    return (SHELL_ERR_NONE);
}



