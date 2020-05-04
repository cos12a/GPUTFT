#include "GPU_Task.h"




/*
*********************************************************************************************************
*                                        TerminalMode_RdLine()
*
* Description : Read a line from the serial interface.
*
* Argument(s) : pstr            Pointer to a buffer at which the string can be stored.
*
*               len_max         Size of the string that will be read.
*
*               pcursor_pos     Pointer to variable that specifies    the current cursor position
*                                   AND
*                               Pointer to variable that will receive the final   cursor position.
*
*               ins             Indicates insertion mode :
*
*                                   DEF_YES,  insert on.
*                                   DEF_NO, insert off.
*
* Return(s)   : Type of escape sequence encountered :
*
*                   TERMINAL_ESC_TYPE_NONE  No escape sequence.
*                   TERMINAL_ESC_TYPE_UP    'Up'   arrow key sequence (move to previous history element).
*                   TERMINAL_ESC_TYPE_DOWN  'Down' arrow key sequence (move to next history element).
*                   TERMINAL_ESC_TYPE_INS   Insert mode toggled.
*
* Caller(s)   : Terminal_Task().
*
* Note(s)     : none.
*********************************************************************************************************
*/


CPU_INT08U  GPU_Send_RdLine (CPU_CHAR     *pstr,
                                 CPU_SIZE_T    len_max,
                                 CPU_SIZE_T   *pcursor_pos)
{
    CPU_CHAR    cmd[10];
    CPU_CHAR    cmd_end;
    CPU_SIZE_T  cmd_ix;
    CPU_CHAR    cursor_char;
    CPU_SIZE_T  cursor_pos;
    CPU_CHAR    in_char;
    CPU_INT08U  rtn_val;
    CPU_SIZE_T  str_len;
    CPU_SIZE_T  str_ix;


    rtn_val    =  TERMINAL_ESC_TYPE_NONE;
    cursor_pos = *pcursor_pos;

    while (DEF_TRUE) {
        in_char = TerminalSerial_RdByte();
        switch (in_char) {
             
            case TERMINAL_VT100_C0_CR:                          /* ------------------- NEW LINE CHAR ------------------ */
            case TERMINAL_VT100_C0_LF:
                 str_len       = Str_Len(pstr);
                 pstr[str_len] = ASCII_CHAR_NULL;
                *pcursor_pos   = str_len;
                 rtn_val       = TERMINAL_ESC_TYPE_NONE;
                 return (rtn_val);
                 
            case TERMINAL_VT100_OVER_RUN:                    // 等待超时处理.
            
                rtn_val       = TERMINAL_ESC_TYPE_NONE;
                return(rtn_val);

            default:                                            /* -------------------- OTHER CHAR -------------------- */
                 if (ASCII_IsPrint(in_char) == DEF_YES){        /* Print printable char.                                */
                                                                /* Ovwr char at cursor's pos.                           */
                     if (cursor_pos < len_max) {
                         Terminal_WrChar(in_char);          // 返回接收到的数据,返回到另一个串口中去.
                         cursor_char      = pstr[cursor_pos];
                         pstr[cursor_pos] = in_char;
                         if (cursor_char == ASCII_CHAR_NULL) {  /* If char at cursor was NULL, wr new NULL.         */
                             pstr[cursor_pos + 1u] = ASCII_CHAR_NULL;
                         }
                         cursor_pos++;
                     }
                 }
                 break;
        }
    }
}



#define GPU_READ_CFG_MAX_CMD_LEN                32u





void usart1TxTask(void *argument)
{    
    CPU_CHAR         cmd[GPU_READ_CFG_MAX_CMD_LEN + 1u];
    CPU_SIZE_T       cmd_len;
    SHELL_CMD_PARAM  cmd_param;
    CPU_INT16S       cmp_val;
    CPU_SIZE_T       cursor_pos;
    CPU_CHAR         cwd_path[TERMINAL_CFG_MAX_PATH_LEN + 1u];
    SHELL_ERR        err;
    CPU_INT08U       esc_type;
    CPU_BOOLEAN      ins;


    while (DEF_TRUE) {

//半数据发送出去.
// 获得返状态,再进行下一次工作.
// 一直挂起,获得消息数据.


// 将数据发送出去.
// 得到返回数据,如果返回数据有误,超时处理.
// OK,一次执行完成.
/* -------------------- RD NEW LINE ------------------- */





        esc_type = GPU_Send_RdLine(&cmd[0],
                                        TERMINAL_CFG_MAX_CMD_LEN,
                                       &cursor_pos );

       {



       }


        cmd_len  = Str_Len(&cmd[0]);

        {
         cmp_val = Str_Cmp(cmd, (CPU_CHAR *)"");
         if (cmp_val != 0) {
             cmp_val = Str_Cmp(cmd, (CPU_CHAR *)"?");       // 如果是Ok命令,则执行完成命令
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
//         TerminalMode_Prompt();                         /* Show new prompt.                                     */
         Str_Copy(cmd, (CPU_CHAR *)"");                 /* Clear cmd.                                           */
         cursor_pos = 0u;                               /* Cursor pos'd at beginning of line.                   */
//         break;
        }
//释放互斥信号量

    }


}


void GPU_tx_and_rx_hand(void           *pbuf,
                                   uint16_t    buf_len)
{




}




