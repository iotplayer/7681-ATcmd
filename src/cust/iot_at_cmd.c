#include <stdio.h>
#include "string.h"
#include "types.h"
#include "uart.h"
#include "iot_custom.h"
#include "iot_api.h"
#include "eeprom.h"
#include "uart_sw.h"
#include "bmd.h"
#include "ate.h"
#include "wifi_task.h"

#ifdef CONFIG_SOFTAP
#include "ap_pub.h"
#endif

/******************************************************************************
* MODULE NAME:     iot_at_cmd.c
* PROJECT CODE:    __MT7681__
* DESCRIPTION:
* DESIGNER:
* DATE:            Jan 2014
*
* SOURCE CONTROL:
*
* LICENSE:
*     This source code is copyright (c) 2014 Mediatek. Inc.
*     All rights reserved.
*
* REVISION     HISTORY:
*   V1.0.0     Jan 2014    - Initial Version V1.0
*
*
* SOURCE:
* ISSUES:
*    First Implementation.
* NOTES TO USERS:
*
******************************************************************************/

/******************************************************************************
 * MACRO DEFINITION
 ******************************************************************************/
#define SYNC_STATE_0 0
#define SYNC_STATE_1 1
#define SYNC_STATE_2 2
#define SYNC_STATE_3 3

/******************************************************************************
 * GLOBAL PARAMTER
 ******************************************************************************/
int8  state   = 0;

#if (UART_INTERRUPT != 1)
uint8 command[AT_CMD_MAX_LEN];
int16 cmd_len = 0;
#endif

#if (ATCMD_ATE_SUPPORT == 1)
bool gCaliEnabled = FALSE;
extern ATE_INFO gATEInfo;
#endif

/******************************************************************************
 * EXTERN PARAMTER
 ******************************************************************************/
extern  char *optarg;
extern  int16 optind;
extern  int iot_uart_rx_mode;
extern  MLME_STRUCT *pIoTMlme;

#if (EP_LOAD_SUPPORT == 1)
extern EEPROM_CFG eepcfg;
#endif

#if (UART_INTERRUPT == 1)
extern PKT_DESC       uartrx2_desc;    //descrypt packet length in uart rx ring2
extern BUFFER_INFO   uartrx2_info;     //uart rx ring2
#endif

/******************************************************************************
 * EXTERN FUNCTION
 ******************************************************************************/


/******************************************************************************
 * FUNCTION
 ******************************************************************************/
void iot_atcmd_resp_header(
    IN OUT int8 *pHeader,
    IN OUT size_t* plen,
    IN int8* ATcmdPrefix,
    IN int8* ATcmdType)
{
    size_t len2 =0;

    /*AT command prefix*/
    *plen = strlen(ATcmdPrefix);
    memcpy(pHeader, ATcmdPrefix, *plen);

    /*AT command Type*/
    len2 = strlen(ATcmdType);
    memcpy(pHeader + *plen, ATcmdType, len2);
    *plen += len2;

    len2 = strlen("=");
    memcpy(pHeader + *plen, "=", len2);
    *plen += len2;

    return;
}

#if (ATCMD_UART_SUPPORT == 1) && (UART_SUPPORT == 1)
/* Format:    AT#Uart -b57600 -w7 -p1 -s1 +enter*/
int16 iot_exec_atcmd_uart(puchar pCmdBuf, int16 AtCmdLen)
{
    iot_atcmd_uart_atcfg(pCmdBuf, AtCmdLen);
    return 0;
}
#endif

#if (ATCMD_ATE_SUPPORT == 1)   //20140528 delete old ATE calibration cmd handler
/*AT#ATECAL -S0*/
/*AT#ATECAL -S1 -C1 -m1 -c7 -b0 -g0 -f65 -p30 -l800 -n100000 -r10+enter*/
/*AT#ATECAL -S2 -C1 -t5000+enter*/
int16 iot_exec_atcmd_ate_cal2(puchar pCmdBuf, int16 AtCmdLen)
{
    char *argv[MAX_OPTION_COUNT];
    char *opString = "S:m:c:b:g:f:p:l:n:r:t:C:u:P:?";
    char *endptr = NULL;
    long num = 0;
    int16 argc = 0;
    char opt = 0;
    uint8 OldValue=0;

    split_string_cmd(pCmdBuf, AtCmdLen, &argc, argv);
    opt = getopt(argc, argv, opString);

    /*To indicate Calibration mode is enabled for Recovery/Calibration State Machine*/
    gCaliEnabled = TRUE;

    while (opt != -1) {
        switch (opt) {
            case 'S':
                /*ATE start,  ATE TxMode,  ATE RxMode (0:START,   1:TxMode   , 2:RxMode)*/
                num = simple_strtol(optarg,&endptr,10);
                ATESTART((uint8)num);
                break;
            case 'C':
                /*ATE Set Channel   (0~14)*/
                num = simple_strtol(optarg,&endptr,10);
                //printf_high("1-SwitchCH:%d\n",num);
                Set_ATE_Switch_CH((uint8)num);
                break;
            case 'm':
                /*ATE Tx Mode        (0:CCK, 1:OFDM, 2:HTMIX,  3:HT_GREEN_FIELD)*/
                num = simple_strtol(optarg,&endptr,10);
                //printf_high("2-TxMode:%d\n",num);
                Set_ATE_TX_MODE_Proc((uint8)num);
                break;
            case 'c':
                /*ATE MCS rate       (MCS num is ralated with TxMode)*/
                num = simple_strtol(optarg,&endptr,10);
                //printf_high("3-MCS:%d\n",num);
                Set_ATE_TX_MCS_Proc((uint8)num);
                break;
            case 'b':
                /*ATE Bandwidth      (0:BW_20,   1:BW_40)*/
                num = simple_strtol(optarg,&endptr,10);
                //printf_high("4-BW:%d\n",num);
                MT7681_Set_ATE_TX_BW_Proc((uint8)num);
                break;
            case 'g':
                /*ATE ShortGI Mode        (0:Full GI    1:Half GI)*/
                num = simple_strtol(optarg,&endptr,10);
                //printf_high("5-GI:%d\n",num);
                Set_ATE_TX_GI_Proc((uint8)num);
                //open this function with num=0,  no frame be detect by sniffer with this command
                //AT#ATECAL -S1 -C1 -m2 -c7 -b0 -g0 -l800 -f65 -p30 -n100000
                //if -c5, -c6 is OK,  -c7,-c8 NG
                break;
            case 'f':
                /*ATE Freq Offset              (0~FF)*/
                num = simple_strtol(optarg,&endptr,10);
                //printf_high("7-FreqOffset:%d\n",num);
                Set_ATE_TX_FREQ_OFFSET_Proc((uint8)num);  //system halt if call this function, maybe stack issue
                break;
            case 'p':
                /*ATE Tx Power                 (0~47)*/
                num = simple_strtol(optarg,&endptr,10);
                //printf_high("8-TxPower:%d\n",num);
                Set_ATE_TX_POWER((uint32)num);
                break;
            case 'l':   //default length : ATE_TX_PAYLOAD_LEN=800
                num = simple_strtol(optarg,&endptr,10);
                //printf_high("6-PayloadLen:%d\n",num);
                Set_ATE_TX_PAYLOAD_LEN((uint32)num);
                break;
            case 'n':
                /*ATE Tx Frame Sent Counter   (0~0xFFFFFFF)*/
                num = simple_strtol(optarg,&endptr,10);
                //printf_high("9-SentCounter:%d\n",num);
                Set_ATE_TX_COUNT_Proc((uint32)num);
                break;
            case 'r':
                /*ATE Tx Frame Sent Speed   (0~0xFFFFFFF), unit:1ms*/
                num = simple_strtol(optarg,&endptr,10);
                //printf_high("10-SentSpeed:%d ms\n",num);
                Set_ATE_TX_Speed_Proc((uint32)num);
                break;
            case 'u':
                /*ATE Tx Count in every -r uint  (0~20)*/
                num = simple_strtol(optarg,&endptr,10);
                //printf_high("11-SetTxCntPerSpeed: %d \n",num);
                Set_ATE_TX_Cnt_Per_Speed((uint32)num);
                break;
            case 't':   //default time/duration : ATE_RX_CNT_DURATION=1000
                /*ATE Rx Count Duration  (0~0xFFFFFFF), unit:1ms*/
                num = simple_strtol(optarg,&endptr,10);
                //printf_high("12-SetRxCntDuration:%d ms\n",num);
                Set_ATE_RX_DURATION((uint32)num);
                break;
            case 'P':  //set BBP Tx Power0 (Range:[0~3], 
                //0-NormalTxPwr, 1-DropTxPwrBy6dB, 2-DropTxPwrBy12dB, 3-AddTxPwrBy6dB)
                num = simple_strtol(optarg,&endptr,10);
                //Printf_High("13-SetBBPTxPower:%d ms\n",num);
                OldValue = Set_ATE_BBP_TXPWR0((uint8)num);
                printf_high("(BBPTxPwr0 Old=%d, New=%d)\n", OldValue, num);
                break;
            case '?':
            default:
                break;
        }
        opt = getopt(argc, argv, opString);
    }

    return 0;
}

uint8 RTMPIoctlE2PROM(bool type, char *pBuf, int16 Len)
{
    char  TmpBuf[16];
    char *opString   = "=";
    char *endptr     = NULL;
    char *Data         = NULL;
    int16 DataLen    = 0;
    uint32 Value     = 0;
    uint32 Offset    = 0;
    int16  OffsetLen = 0;

    uint8 eData      = 0;
    uint8 fData[1]   = {0};

    memset(TmpBuf, 0, sizeof(TmpBuf));

    if ((pBuf == NULL) || (Len == 0))
        return 1;

    if (strchr(pBuf, ' ') != NULL)
        return 2;

    Data = strpbrk(pBuf, opString);

    /*for efuse/flash read*/
    if (Data == NULL) {
        Offset = simple_strtol(pBuf, &endptr, 16);
        if (type == 0) {
            if (set_ate_efuse_read((uint16)Offset, &eData) == TRUE )
                printf_high("[0x%x]=0x%02x\n",Offset, eData);
        } else {
            spi_flash_read((uint32)Offset, fData, 1);
            printf_high("[0x%x]=[0x%02x]\n",Offset,fData[0]);
        }
    } else {    /*for efuse/flash write*/
        OffsetLen = Data - pBuf;
        memcpy(TmpBuf, pBuf, OffsetLen);
        Offset = simple_strtol(TmpBuf, &endptr, 16);

        Data++;   /*not include character "="  or "space"*/
        DataLen = Len-OffsetLen-1;
        memset(TmpBuf, 0, sizeof(TmpBuf));
        memcpy(TmpBuf, Data, DataLen);
        Value  = simple_strtol(TmpBuf, &endptr, 16);

        if (type == 0) {
            if (set_ate_efuse_write((uint16)Offset, (uint8)Value) == FALSE)
                printf_high("Offset must a even num\n");
        } else {
            spi_flash_write((uint32)Offset, (uint8*)&Value, 1);
            printf_high("[0x%x]=[0x%02x]\n",Offset,Value);
        }
    }
}


uint8 ate_cmd_cali_hdlr(char *pBuf, int16 Len)
{
    char  Cmd[16];
    int16 CmdLen   = 0;
    char *Data       = NULL;
    int16 DataLen  = 0;
    char *opString = "=";
    long  num      = 0;
    char *endptr   = NULL;

    memset(Cmd, 0, sizeof(Cmd));

    if ((pBuf == NULL) || (Len == 0))
        return 1;

    if (strchr(pBuf, ' ') != NULL) {
        Data = strpbrk(pBuf, " ");        /*for case:  set e2p d0=1E*/
    } else {
        Data = strpbrk(pBuf, opString);   /*for case:  set ATECHANNEL=1*/
    }

    if (Data == NULL)
        return 2;

    CmdLen = Data - pBuf;
    memcpy(Cmd, pBuf, CmdLen);

    Data++;   /*not include character "="  or "space"*/
    DataLen = Len-CmdLen-1;

    //printf_high("LINE:%d, Cmd[%s], CmdLen[%d]\n",__LINE__, Cmd, CmdLen);
    //printf_high("LINE:%d, Data[%s], Len[%d]\n",__LINE__, Data, DataLen);

    if (!memcmp(Cmd,"ATE",CmdLen)) {
        if (!memcmp(Data,"ATESTART",strlen("ATESTART"))) {
            ATE_INIT();
        } else if (!memcmp(Data,"TXFRAME",strlen("TXFRAME"))) {
            ATE_TXFRAME();
        } else if (!memcmp(Data,"RXFRAME",strlen("RXFRAME"))) {
            ATE_RXFRAME();
        } else if (!memcmp(Data,"ATESTOP",strlen("ATESTOP"))) {
            ATE_STOP();
        }
    } else if (!memcmp(Cmd,"ATECHANNEL",CmdLen)) {
        /*ATE Set Channel        (0~14)*/
        num = simple_strtol(Data, &endptr,10);
        //printf_high("1-SwitchCH:%d\n",num);
        Set_ATE_Switch_CH((uint8)num);
    } else if (!memcmp(Cmd,"ATETXMODE",CmdLen)) {
        /*ATE Tx Mode         (0:CCK, 1:OFDM, 2:HTMIX,  3:HT_GREEN_FIELD)*/
        num = simple_strtol(Data, &endptr,10);
        //printf_high("2-TxMode:%d\n",num);
        Set_ATE_TX_MODE_Proc((uint8)num);
    } else if (!memcmp(Cmd,"ATETXMCS",CmdLen)) {
        /*ATE MCS rate         (MCS num is ralated with TxMode)*/
        num = simple_strtol(Data, &endptr,10);
        //printf_high("3-MCS:%d\n",num);
        Set_ATE_TX_MCS_Proc((uint8)num);
    } else if (!memcmp(Cmd,"ATETXBW",CmdLen)) {
        /*ATE Bandwidth          (0:BW_20,     1:BW_40)*/
        num = simple_strtol(Data, &endptr,10);
        //printf_high("4-BW:%d\n",num);
        MT7681_Set_ATE_TX_BW_Proc((uint8)num);
    } else if (!memcmp(Cmd,"ATETXGI",CmdLen)) {
        /*ATE ShortGI Mode          (0:Full GI    1:Half GI)*/
        num = simple_strtol(Data, &endptr,10);
        //printf_high("5-GI:%d\n",num);
        Set_ATE_TX_GI_Proc((uint8)num);
    } else if (!memcmp(Cmd,"ATETXLEN",CmdLen)) {
        //default length : ATE_TX_PAYLOAD_LEN=800
        num = simple_strtol(Data, &endptr,10);
        //printf_high("6-PayloadLen:%d\n",num);
        Set_ATE_TX_PAYLOAD_LEN((uint32)num);
    } else if (!memcmp(Cmd,"ATETXFREQOFFSET",CmdLen)) {
        /*ATE Freq Offset               (0~FF)*/
        num = simple_strtol(Data, &endptr,10);
        //printf_high("7-FreqOffset:%d\n",num);
        Set_ATE_TX_FREQ_OFFSET_Proc((uint8)num);
    } else if (!memcmp(Cmd,"ATETXPOW0",CmdLen)) {
        /*ATE Tx Power               (0~47)*/
        num = simple_strtol(Data, &endptr,10);
        //printf_high("8-TxPower:%d\n",num);
        Set_ATE_TX_POWER((uint32)num);
    } else if (!memcmp(Cmd,"ATETXCNT",CmdLen)) {
        /*ATE Tx Frame Sent Counter   (0~0xFFFFFFF)*/
        num = simple_strtol(Data, &endptr,10);
        //printf_high("9-SentCounter:%d\n",num);
        Set_ATE_TX_COUNT_Proc((uint32)num);
    } else if (!memcmp(Cmd,"ATEDA",CmdLen)) {
        /*not support*/
    } else if (!memcmp(Cmd,"ResetCounter",CmdLen)) {
        Set_ResetStatCounter_Proc();
    } else if (!memcmp(Cmd,"e2p",CmdLen)) {
        RTMPIoctlE2PROM(0, Data, DataLen);
    } else if (!memcmp(Cmd,"flash",CmdLen)) {
        RTMPIoctlE2PROM(1, Data, DataLen);
    } else {
        /*not support*/
    }

    return 0;
}



/*========================================================================
    Routine    Description:
        iot_exec_atcmd_ate_cal --  Do ATE calibration for iwpriv cmd format

    Arguments:
    Return Value: 0 is success
    Note:
========================================================================*/
/*Case1:Tx Calibration*/
/*iwpriv ra0 set ATE=ATESTART+enter*/
/*iwpriv ra0 set ATE=ATECHANNEL=1+enter*/
/*iwpriv ra0 set ATE=ATETXMODE=2+enter*/
/*iwpriv ra0 set ATE=ATETXMCS=7+enter*/
/*iwpriv ra0 set ATE=ATETXBW=1+enter*/
/*iwpriv ra0 set ATE=ATETXGI=0+enter*/
/*iwpriv ra0 set ATE=ATETXLEN=800+enter*/
/*iwpriv ra0 set ATE=ATETXFREQOFFSET=33+enter*/
/*iwpriv ra0 set ATE=ATETXCNT=10000+enter*/
/*iwpriv ra0 set ATE=ATETXPOW0=16+enter*/
/*iwpriv ra0 set ATE=ATETXFRAME+enter*/

/*Case2:Rx Calibration*/
/*iwpriv ra0 set ATE=ATESTART+enter*/
/*iwpriv ra0 set ATE=ATECHANNEL=1+enter*/
/*iwpriv ra0 set ATE=ATETXMODE=0+enter*/
/*iwpriv ra0 set ATE=ATETXMCS=0+enter*/
/*iwpriv ra0 set ATE=ATETXBW=0+enter*/
/*iwpriv ra0 set ATE=ATETXFREQOFFSET=33+enter*/
/*iwpriv ra0 set ATE=ATETXCNT=10000+enter*/
/*iwpriv ra0 set ATE=ATETXPOW0=16+enter*/
/*iwpriv ra0 set ATE=ATETXFRAME+enter*/

/*Case3:efuse/Flash r/w*/
/*iwpriv ra0 set e2p 0x52+enter*/
/*iwpriv ra0 set e2p 0x52=0x01+enter*/
/*iwpriv ra0 set flash 0x17052+enter*/
/*iwpriv ra0 set flash 0x17052=0x01+enter*/
int16 iot_exec_atcmd_ate_cal(puchar cmd_buf, int16 Len)
{
    char  Cmd[16];
    int16 CmdLen   = 0;
    char *Data       = NULL;
    int16 DataLen  = 0;
    char opString[] = {' ',','};

    /*To indicate Calibration mode is enabled for Recovery/Calibration State Machine*/
    //not set to FALSE, once input ATE Calibration cmd, should keep Calibration Mode
    gCaliEnabled = TRUE;

    memset(Cmd, 0, sizeof(Cmd));

    if ((cmd_buf == NULL) || (Len == 0))
        return 1;

    Data = strpbrk((char *)cmd_buf, opString);

    if (Data == NULL) {
        /* for the case:  only has a cmd name, no cmd val
            e.g:  "iwpriv ra0 stat"  or "AT#ATECAL stat"*/
        CmdLen = Len;
        DataLen = 0;
    } else {
        /* for the case:  has a cmd name and cmd val with a sperator '='
            e.g:  "iwpriv ra0 set ATE=ATESTART"  or "AT#ATECAL set ATE=ATESTART"*/
        CmdLen = Data - (char *)cmd_buf;
        Data++;   /*not include character "=" */
        DataLen = Len-CmdLen-1;
    }
    memcpy(Cmd, (char *)cmd_buf, CmdLen);

    //printf_high("LINE:%d, Cmd[%s], CmdLen[%d]\n",__LINE__, Cmd, CmdLen);
    //printf_high("LINE:%d, Data[%s], Len[%d]\n",__LINE__, Data, DataLen);

    if (!memcmp(Cmd,"set",CmdLen)) {
        ate_cmd_cali_hdlr(Data, DataLen); /*not include 1st Space*/
    } else if (!memcmp(Cmd,"stat",CmdLen)) {
        ate_cmd_cali_stat();
    } else {
        /*Do nothing*/
    }

    return 0;
}
#endif


#if (ATCMD_FLASH_SUPPORT == 1)
/* Format:    AT#FLASH -r6 +enter*/
/* Format:    AT#FLASH -s6 -v56+enter*/
int16 iot_atcmd_exec_flash(puchar pCmdBuf, int16 AtCmdLen)
{
    char *argv[MAX_OPTION_COUNT];
    char *opString = "r:s:v:c:?";
    char *endptr = NULL;
    long num = 0;
    int16 argc = 0;
    char opt = 0;
    uint32 fOffset = 0;
    uint8 fData[1] = {0};
    uint32 stringLen = 0;
    uint8 buffer[48] = {0};

    split_string_cmd(pCmdBuf, AtCmdLen, &argc, argv);
    opt = getopt(argc, argv, opString);

    while (opt != -1) {
        switch (opt) {
            case 'r':
                /*Flash Read Offset*/
                num = simple_strtol(optarg,&endptr,0);
                spi_flash_read((uint32)num, fData, 1);
                printf_high("[0x%x]=[0x%02x]\n",num,fData[0]);
                break;
            case 's':
                /*Flash Write Offset*/
                num = simple_strtol(optarg,&endptr,0);
                fOffset = (uint32)num;
                break;
            case 'v':
                /*Flash Write Value*/
                num = simple_strtol(optarg,&endptr,0);
                if ( (fOffset < FLASH_OFFSET_RESERVE_2_START) ||
                     ((fOffset >= FLASH_OFFSET_STA_FW_START) && (fOffset < FLASH_OFFSET_RESERVE_7_START))) {
                    /*Protecte the FW region*/
                    printf_high("[0x%x] is not permit to write!\n",fOffset);
                }else{
                    spi_flash_write(fOffset, (uint8*)&num, 1);
                    printf_high("[0x%x]=[0x%02x]\n",fOffset,num);
                }
                break;
             case 'c':
                stringLen = strlen(optarg);
                if(stringLen > (sizeof(buffer)-1))
                  stringLen = sizeof(buffer)-1;
                memcpy(buffer, optarg, stringLen);
                buffer[stringLen] ='\0';
                spi_flash_write(fOffset, (uint8*)buffer, stringLen+1);
                printf_high("[0x%x]=%s\n",fOffset,buffer);
                break;
            case '?':
            default:
                break;
        }
        opt = getopt(argc, argv, opString);
    }

    return 0;
}
#endif



#if (ATCMD_EFUSE_SUPPORT == 1)
/* Format:    AT#EFUSE -r6 +enter*/
/* Format:    AT#EFUSE -s6 -v56+enter*/
int16 iot_exec_atcmd_efuse_set(puchar pCmdBuf, int16 AtCmdLen)
{
    char *argv[MAX_OPTION_COUNT];
    char *opString = "r:s:v:?";
    char *endptr = NULL;
    long num = 0;
    int16 argc = 0;
    char opt = 0;
    uint16 eOffset = 0;
    uint8 eData = 0;

    split_string_cmd(pCmdBuf, AtCmdLen, &argc, argv);
    opt = getopt(argc, argv, opString);

    while (opt != -1) {
        switch (opt) {
            case 'r':
                /*Efuse Read Offset*/
                num = simple_strtol(optarg,&endptr,0);
                //printf_high("0-Read Offset:%d\n",num);
                if ( set_ate_efuse_read((uint16)num, &eData) == TRUE )
                    printf_high("[0x%x]=0x%02x\n",num, eData);
                break;
            case 's':
                /*Efuse Write Offset*/
                num = simple_strtol(optarg,&endptr,0);
                //printf_high("1-Write Offset:%d\n",num);
                eOffset = (uint16)num;
                break;
            case 'v':
                /*Efuse Write Value*/
                num = simple_strtol(optarg,&endptr,0);
                //printf_high("2-Write Value:%d\n",num);
                if ( set_ate_efuse_write(eOffset, (uint8)num) == FALSE)
                    printf_high("Offset must a even num\n");
                break;
            case '?':
            default:
                break;
        }
        opt = getopt(argc, argv, opString);
    }

    return 0;
}
#endif

#if ((ATCMD_PS_SUPPORT == 1) && (MT7681_POWER_SAVING == 1))
/* Format:    AT#PowerSaving -l0 -t16777215+enter*/
/* -l :  range 1~5 */
/* -t :  range 0~0xFFFFFF   sleepTime = us*/
/* Format:    AT#PowerSaving -r+enter*/
int16 iot_exec_atcmd_ps_set(puchar pCmdBuf, int16 AtCmdLen)
{
    char *argv[MAX_OPTION_COUNT];
    char *opString = "r::l:t:?";
    char *endptr = NULL;
    int16 argc = 0;
    char opt = 0;

    uint8  Level = 0;
    uint32 SleepTime = 0;

    split_string_cmd(pCmdBuf, AtCmdLen, &argc, argv);
    opt = getopt(argc, argv, opString);

    while (opt != -1) {
        switch (opt) {
            case 'l':
                /*write power saving level*/
                Level = (uint8)simple_strtol(optarg,&endptr,0);
                if ((Level>0) && (Level<=5)) {
                    printf_high("[PS Lvl]=%d\n",Level);
                    pIoTMlme->PMLevel = Level;
                }
                break;
            case 't':
                /*power sleep time*/
                SleepTime = (uint32)simple_strtol(optarg,&endptr,0);
                if ((Level>0) && (Level<=5)) {
                    printf_high("[PS] Lvl=%d Sleep=%u(us)\n",Level,SleepTime);
                    LowPower(0,Level,SleepTime);
                }
                break;
            case 'r':
                /*read power saving level*/
                printf_high("[PS Lvl]=%d\n",pIoTMlme->PMLevel);
                break;
            case '?':
            default:
                break;
        }
        opt = getopt(argc, argv, opString);
    }

    return 0;
}
#endif


#if (ATCMD_NET_MODE_SUPPORT == 1)
int16 iot_exec_atcmd_netmode(puchar pCmdBuf, int16 AtCmdLen)
{
    return -1;
}
#endif

#if (ATCMD_CH_SWITCH_SUPPORT == 1)
/*========================================================================
    Routine    Description:
        iot_exec_atcmd_ch_switch --  switch channel and Bandwidth
        -b:  [0/1]   0=BW_20,    1=BW_40
        -c:  [1~14]
    Arguments:
    Return Value: 0 is success
    Note:  In present , the channel range is channel 1~14, and Bandwidth is only support 20MHz
========================================================================*/
int16 iot_exec_atcmd_ch_switch(puchar pCmdBuf, int16 AtCmdLen)
{
    char *argv[MAX_OPTION_COUNT];
    char *opString = "b:c:?";
    char *endptr = NULL;
    int16 argc = 0;
    char opt = 0;

    uint8  bbp = BW_20;
    uint8  ch  = 0;

    split_string_cmd(pCmdBuf, AtCmdLen, &argc, argv);
    opt = getopt(argc, argv, opString);

    while (opt != -1) {
        switch (opt) {
            case 'b':
                /*power saving level*/
                bbp = (uint8)simple_strtol(optarg,&endptr,0);
                if ((bbp==BW_20) || (bbp==BW_40)) {
                    printf_high("[Set BBP]=%d\n",bbp);
                    rtmp_bbp_set_bw(bbp);
                }
                break;
            case 'c':
                /*power sleep time*/
                ch = (uint8)simple_strtol(optarg,&endptr,0);
                if ((ch >0) && (ch <= 14)) {
                    printf_high("[Set CH]=%d\n",ch);
                    iot_atcmd_set_channel(ch);
                }
                break;
            case '?':
            default:
                break;
        }
        opt = getopt(argc, argv, opString);
    }

    return 0;
}
#endif

#ifdef CONFIG_SOFTAP
/*========================================================================
    Routine    Description:
        iot_exec_atcmd_conf_softap --  set soft ap configration

    Arguments:
    Return Value: 0 is success
    Note:
    example1: AT#SoftAPConf -sMT7681New1231 -a7 -p87654321 -c6
    example2: AT#SoftAPConf -m0          //store current AP cfg to flash
    example3: AT#SoftAPConf -d0           //clean current AP cfg on the flash
========================================================================*/
int16 iot_exec_atcmd_conf_softap(puchar pCmdBuf, int16 AtCmdLen)
{
    int16 argc = 0;
    char *argv[MAX_OPTION_COUNT];
    char *opString = "s:c:a:p:m:d:?";
    int opt;
    char *endptr = NULL;

    uint8 SSID[MAX_SSID_PASS_LEN+1] = {0};
    uint8 Password[MAX_SSID_PASS_LEN+1] = {0};
    uint8 Auth_Mode = 0;
    uint8 Channel = 0;
    uint32 SSIDLen = 0;
    uint32 PSWLen = 0;
    bool UpdateCfg = TRUE;

    memset(argv,0,4*MAX_OPTION_COUNT);
    split_string_cmd(pCmdBuf, AtCmdLen, &argc, argv);

    //printf("argc3:%d,%s,%s\n",argc,argv[0],argv[1]);
    optind = 1;
    optarg = NULL;

    opt = getopt(argc, argv, opString);

    while (opt != -1) {
        switch (opt) {
            case 's':
                if (optarg == NULL)
                    break;
                
                SSIDLen = strlen(optarg);
                if (SSIDLen > MAX_SSID_PASS_LEN) {
                    return -1;
                }
                memcpy(SSID, optarg, SSIDLen);
                SSID[SSIDLen] ='\0';
                printf_high("AT#SSID:[%s], Len = %d\n",SSID, strlen(optarg));
                break;
            case 'a':  /*only support 0=OPEN, 4=WPAPSK, 7=WPA2PSK, 9=WPA/WPA2PSK*/
                if (optarg == NULL)
                    break;
                
                if (strlen(optarg) > MAX_AUTH_MODE_LEN) {
                    return -2;
                }
                Auth_Mode = (uint8)simple_strtol(optarg,&endptr,0);
                printf_high("AT#Auth_Mode:%d\n",Auth_Mode);
                break;
            case 'p':
                if (optarg == NULL)
                    break;
                
                PSWLen = strlen(optarg);
                if (PSWLen > MAX_SSID_PASS_LEN) {
                    return -3;
                }
                memcpy(Password, optarg, PSWLen);
                Password[PSWLen] ='\0';
                printf_high("AT#Password:%s\n",Password);
                break;
            case 'c':
                if (optarg == NULL)
                    break;
                
                Channel = (uint8)atoi(optarg);
                printf_high("AT#Channel:%d\n",Channel);
                break;
            case 'm':
                UpdateCfg = FALSE;
                store_ap_cfg();
                break;
            case 'd':
                UpdateCfg = FALSE;
                reset_ap_cfg();
                break;
            case '?':
            default:
                break;
        }
        opt = getopt(argc, argv, opString);
    }

    if (UpdateCfg)
        iot_apcfg_update(SSID, Auth_Mode, Password, Channel);

    return 0;
}
#endif

#if (ATCMD_REBOOT_SUPPORT == 1)
/*========================================================================
    Routine    Description:
        iot_atcmd_exec_reboot --  system reboot

    Arguments:
    Return Value:  NONE
    Note:
========================================================================*/
void iot_atcmd_exec_reboot(void)
{
    iot_sys_reboot();
    return;
}
#endif

#if (ATCMD_SET_SMNT_SUPPORT == 1)
/*========================================================================
    Routine Description:
        iot_atcmd_exec_smnt -- Enable Smart Connection
        System will enter into SmartConnection State,  until System get IP from AP or System Reboot

    Arguments:
    Return Value: 0 is success,  -1 is out of memory
    Note:
========================================================================*/
int16 iot_atcmd_exec_smnt(puchar pCmdBuf)
{
    pIoTMlme->ATSetSmnt = TRUE;
    wifi_state_chg(WIFI_STATE_INIT, 0);
    return 0;
}
#endif


#if (ATCMD_GET_VER_SUPPORT == 1)
/*========================================================================
    Routine    Description:
        iot_atcmd_exec_ver -- get FW version

    Arguments:
    Return Value: 0 is success,  -1 is out of memory
    Note:
========================================================================*/
int16 iot_atcmd_exec_ver(puchar pCmdBuf)
{
    size_t  len=0 , len2 =0;

    /* the response header format is:  "AT#CmdType=" */
    iot_atcmd_resp_header((int8 *)pCmdBuf, &len, AT_CMD_PREFIX, AT_CMD_VER);

    /*AT command Version*/
    len2 = strlen(FW_VERISON_CUST);
    memcpy(pCmdBuf + len, FW_VERISON_CUST, len2);
    len += len2;

    len2 = strlen("\n");
    if ( (len + len2 + 3) >= AT_CMD_MAX_LEN) {
        /*pCmdBuf is mallocated on    iot_atcmd_hdlr(),
           but here it's start after "AT#"
           thus it's length is     (AT_CMD_MAX_LEN-3); */
        return -1;
    }
    memcpy(pCmdBuf + len, "\n", len2);
    len += len2;

    //len += 1;
    //*(pCmdBuf+len) = '\n';

    iot_uart_output((puchar)pCmdBuf, (int16)len);
    return 0;
}
#endif

#if (ATCMD_JTAGMODE_SUPPORT == 1)
void iot_exec_atcmd_jtag_switch(puchar pCmdBuf, int16 AtCmdLen)
{
    int16 argc = 0;
    char *argv[MAX_OPTION_COUNT];
    char *opString = "m:?";
    char opt=0;
    char *endptr;

    uint8 switch_on=0;
    uint8 content=0;

    memset(argv,0,4*MAX_OPTION_COUNT);
    split_string_cmd(pCmdBuf, AtCmdLen, &argc, argv);
    opt = getopt(argc, argv, opString);

    while (opt != -1) {
        switch (opt) {
            case 'm':
                switch_on =  (uint8) simple_strtoul(optarg,&endptr,10);
                //printf_high("1:%s,%d\n",optarg, socket);
                content++;
                break;
            case '?':
            default:
                break;
        }
        opt = getopt(argc, argv, opString);
    }

    if (content == 1)
        iot_jtag_mode_switch(switch_on);
    else
        iot_jtag_mode_get();

    return;
}
#endif


int16 iot_atcmd_parser(puchar cmd_buf, int16 AtCmdLen)
{
    int16 ret_code = 0;
#if (ATCMD_CH_SWITCH_SUPPORT == 1)
    int8 cTypeLen = 0;
#endif

    //BUFFER_INFO  *rx_ring = &(UARTPort.Rx_Buffer);
    //PKT_DESC     *rx_desc = &(UARTPort.Rx_desc);

    cmd_buf[AtCmdLen] = '\0';
    //printf_high("AT command: %s , len=%d \n",cmd_buf, AtCmdLen);
    //printf_high("ring buf:pkt=%d,R=%d,W=%d\n", rx_desc->pkt_num,BRead(rx_ring),BWrite(rx_ring));

    /* The current process is not encough*/
    /* need improve for the command type and paramters parsing */

    /* Format:    AT#Default+enter*/
    if (!memcmp(cmd_buf,AT_CMD_DEFAULT,sizeof(AT_CMD_DEFAULT)-1)) {
        reset_cfg();
        iot_linkdown(0);   //disable Link Down to fix STA machine keeps SMNT and no reponse
    }
#if (ATCMD_SET_SMNT_SUPPORT == 1)
    /* Format:  AT#Smnt+enter*/
    else if (!memcmp(cmd_buf,AT_CMD_SET_SMNT,sizeof(AT_CMD_SET_SMNT)-1)) {
        ret_code = iot_atcmd_exec_smnt(cmd_buf);
    }
#endif
#if (ATCMD_GET_VER_SUPPORT == 1)
    /* Format:  AT#Ver+enter*/
    else if (!memcmp(cmd_buf,AT_CMD_VER,sizeof(AT_CMD_VER)-1)) {
        ret_code = iot_atcmd_exec_ver(cmd_buf);
    }
#endif
#if (ATCMD_NET_MODE_SUPPORT == 1)
    else if (!memcmp(cmd_buf,AT_CMD_NETMODE,sizeof(AT_CMD_NETMODE)-1)) {
        ret_code = iot_exec_atcmd_netmode(cmd_buf, AtCmdLen);
    }
#endif
#if (ATCMD_REBOOT_SUPPORT == 1)
    /* Format:  AT#Reboot+enter*/
    else if (!memcmp(cmd_buf,AT_CMD_REBOOT,sizeof(AT_CMD_REBOOT)-1)) {
        iot_atcmd_exec_reboot();
    }
#endif
#if (ATCMD_CH_SWITCH_SUPPORT == 1)
    /* Format:  AT#Channel -b0 -c6+enter */
    //-b:  [0/1]     0=BW_20,     1=BW_40
    //-c:  [1~14]
    else if (!memcmp(cmd_buf,AT_CMD_CHANNEL,sizeof(AT_CMD_CHANNEL)-1)) {
        ret_code = iot_exec_atcmd_ch_switch(cmd_buf, AtCmdLen);
    }
#endif
#if    (ATCMD_SOFTAP_SUPPORT == 1) && (ATCMD_SUPPORT == 1)
    /* Format:    AT#SoftAPConf -s[ssid] -c[channel] -a[auth_mode] -p[password]+enter*/
    /*                 now, only support Open mode without password */
    /* Format:    AT#SoftAPConf -m1+enter    --->store current AP setting to flash*/
    /* Format:    AT#SoftAPConf -d1+enter     --->clear AP setting in flash*/
    else if (!memcmp(cmd_buf,AT_CMD_SOFTAP_CFG, sizeof(AT_CMD_SOFTAP_CFG)-1)) {
        ret_code = iot_exec_atcmd_conf_softap(cmd_buf, AtCmdLen);

        if (ret_code == 0)
            printf_high("Config SoftAP success\n");
        else
            printf_high("Config SoftAP fail: %d\n", ret_code);
    }
#endif
#if (ATCMD_FLASH_SUPPORT == 1)
    /* Format:    AT#FLASH -r6 +enter*/
    /* Format:    AT#FLASH -s6 -v56+enter*/
    else if (!memcmp(cmd_buf,AT_CMD_FLASH_SET,sizeof(AT_CMD_FLASH_SET)-1)) {
        //printf_high("Flash Write/Read \n");
        ret_code = iot_atcmd_exec_flash(cmd_buf, AtCmdLen);
    }
#endif
#if (ATCMD_EFUSE_SUPPORT == 1)
    /* Format:    AT#EFUSE -r6 +enter*/
    /* Format:    AT#EFUSE -s6 -v56 +enter*/
    else if (!memcmp(cmd_buf,AT_CMD_EFUSE_SET,sizeof(AT_CMD_EFUSE_SET)-1)) {
        //printf_high("Efuse Write/Read \n");
        ret_code = iot_exec_atcmd_efuse_set(cmd_buf, AtCmdLen);
    }
#endif
#if (ATCMD_ATE_SUPPORT == 1)  //20140528 delete old ATE calibration cmd handler,  be instead of iwpriv cmd format handler
    /* Format:    AT#ATECAL -C1 -m1 -c7 -g0 -f65 -p30 -n100000+enter*/
    else if (!memcmp(cmd_buf,AT_CMD_ATE_CAL,sizeof(AT_CMD_ATE_CAL)-1)) {
        //printf_high("ATECAL \n");
        gATEInfo.ATECmdFmt = ATE_CMD_TYPE_AT;
        ret_code = iot_exec_atcmd_ate_cal2(cmd_buf, AtCmdLen);
    }
#endif
#if (ATCMD_UART_SUPPORT == 1) && (UART_SUPPORT == 1)
    /* Format:    AT#Uart -b57600 -w7 -p1 -s1 +enter*/
    else if (!memcmp(cmd_buf,AT_CMD_UART,sizeof(AT_CMD_UART)-1)) {
        ret_code = iot_exec_atcmd_uart(cmd_buf, AtCmdLen);
    }
#endif
#if ((ATCMD_PS_SUPPORT == 1) && (MT7681_POWER_SAVING == 1))
    /* Format:    AT#PowerSaving -l0 -t2000+enter*/ /* sleepTime = us */
    else if (!memcmp(cmd_buf,AT_CMD_PS_SET,sizeof(AT_CMD_PS_SET)-1)) {
        ret_code = iot_exec_atcmd_ps_set(cmd_buf, AtCmdLen);
    }
#endif

    /*Only for Debug*/
#if (ATCMD_ATE_MBR_CTL == 1)
    else if (!memcmp(cmd_buf,AT_CMD_MAC_SET,sizeof(AT_CMD_MAC_SET)-1)) {
        ret_code = iot_exec_atcmd_mac_rw(cmd_buf, AtCmdLen);
    } else if (!memcmp(cmd_buf,AT_CMD_BBP_SET,sizeof(AT_CMD_BBP_SET)-1)) {
        ret_code = iot_exec_atcmd_bbp_rw(cmd_buf, AtCmdLen);
    } else if (!memcmp(cmd_buf,AT_CMD_RF_SET,sizeof(AT_CMD_RF_SET)-1)) {
        ret_code = iot_exec_atcmd_rf_rw(cmd_buf, AtCmdLen);
    }
#endif

    /*jinchuan  These functions developping is pending, no plan to use */
#if  0 //(ATCMD_TCPIP_SUPPORT == 1) && (CFG_SUPPORT_TCPIP == 1)

    /* Format:    AT#Tcp_Connect -a192.168.1.131 -p1234 +enter*/
    else if (!memcmp(cmd_buf,AT_CMD_TCPCONNECT,sizeof(AT_CMD_TCPCONNECT)-1)) {
        iot_tcpip_connect(cmd_buf, AtCmdLen);
    }
    /* Format:    AT#Tcp_Send -s1 -dtest data +enter*/
    else if (!memcmp(cmd_buf,AT_CMD_TCPSEND,sizeof(AT_CMD_TCPSEND)-1)) {
        iot_tcpip_send(cmd_buf, AtCmdLen);
    }
    /* Format:    AT#Tcp_Listen -p7682 +enter*/
    else if (!memcmp(cmd_buf,AT_CMD_TCPLISTEN,sizeof(AT_CMD_TCPLISTEN)-1)) {
        iot_tcpip_listen(cmd_buf, AtCmdLen);
    }
    /* Format:    AT#Tcp_Disconnect -s0 +enter*/
    else if (!memcmp(cmd_buf,AT_CMD_TCPDISCONNECT,sizeof(AT_CMD_TCPDISCONNECT)-1)) {
        iot_tcpip_disconnect(cmd_buf, AtCmdLen);
    }
    /* Format:    AT#Udp_Create -a192.168.1.132 -r1234 -l4321 +enter*/
    else if (!memcmp(cmd_buf,AT_CMD_UDPCREATE,sizeof(AT_CMD_UDPCREATE)-1)) {
        iot_tcpip_udp_creat(cmd_buf, AtCmdLen);
    }
    /* Format:    AT#Udp_Remove -s130 +enter*/
    else if (!memcmp(cmd_buf,AT_CMD_UDPREMOVE,sizeof(AT_CMD_UDPREMOVE)-1)) {
        iot_tcpip_udp_del(cmd_buf, AtCmdLen);
    }
    /* Format:    AT#Udp_Send -s130 -d1234 -a192.168.1.132 -r1234 +enter*/
    else if (!memcmp(cmd_buf,AT_CMD_UDPSEND,sizeof(AT_CMD_UDPSEND)-1)) {
        iot_tcpip_udp_send(cmd_buf, AtCmdLen);
    }
    /* Format:    AT#StaticIP -i192.168.1.100 -m255.255.255.0 -g192.168.1.1 -d172.26.2.185 +enter*/
    else if (!memcmp(cmd_buf,AT_CMD_STATIC_IP,sizeof(AT_CMD_STATIC_IP)-1)) {
        iot_tcpip_set_static_ip(cmd_buf, AtCmdLen);
    } else if (!memcmp(cmd_buf,AT_CMD_NETSTAT,sizeof(AT_CMD_NETSTAT)-1)) {

        iot_tcpip_netstat(cmd_buf, AtCmdLen);
    }
#endif
#if (ATCMD_UART_FW_SUPPORT == 1) && (UART_SUPPORT == 1)
    /* Format:    AT#UpdateFW+enter   (the range of type is [1~7] ) */
    else if (!memcmp(cmd_buf,AT_CMD_UPDATEFW,sizeof(AT_CMD_UPDATEFW)-1)) {
        iot_xmodem_update_fw_start(); /*Disable Uart Rx Interrupt*/
        iot_xmodem_update_fw();
        iot_xmodem_update_fw_stop();  /*Restore Uart Rx Interrupt*/
    }
#endif
#if (ATCMD_JTAGMODE_SUPPORT == 1)
    else if (!memcmp(cmd_buf,AT_CMD_JTAGMODE,sizeof(AT_CMD_JTAGMODE)-1)) {
        iot_exec_atcmd_jtag_switch(cmd_buf, AtCmdLen);
    }
#endif
    else if (!memcmp(cmd_buf,"LOG",3)) {
        extern bool PRINT_FLAG;
        PRINT_FLAG = !PRINT_FLAG;
        if(PRINT_FLAG)
            iot_uart_output((uint8*)"LOG ON\r\n", strlen("LOG ON"));
        else
            iot_uart_output((uint8*)"LOG OFF\r\n", strlen("LOG OFF"));
    }

    return ret_code;
}

#if (ATCMD_RECOVERY_SUPPORT==0)

#ifdef CONFIG_STATION
extern STA_ADMIN_CONFIG *pIoTStaCfg;
bool g_useFlashSetting = FALSE;
#endif
#ifdef CONFIG_SOFTAP
extern AP_ADMIN_CONFIG  *pIoTApCfg;
#endif

// assume each write can only be 48/64 bytes
static uint8 _socket_output_buffer[50];
static bool _socket_output_buffer_in_use = FALSE;
static uint8 _socket_output_buffer_datalen;
static uint32 _socket_output_port;

static inline void _at_response(char* text)
{
    iot_uart_output((uint8*)text, (int16)strlen(text));
}

static inline void _at_response_ok()
{
    _at_response("\r\nOK\r\n");
}

static inline void _at_response_err()
{
    _at_response("\r\nERROR\r\n");
}

static void _parse_tokens(char* data, char** tokens, int num)
{
    char* p = data;
    int i = 0;

    while(i < num)
    {
        tokens[i] = p;
        while(*p && *p != ',')
           p++;
        if(!*p)
            break;
        *p = 0; p++;
        i++;
    }
}

static int base64_encode_len(int len)
{
    return ((len+2)/3)*4;
}

static uint8 base64_enc_map(uint8 n)
{
    if(n < 26)
        return 'A'+n;
    if(n < 52)
        return 'a'+(n-26);
    if(n < 62)
        return '0'+(n-52);
    return n == 62 ? '+' : '/';
}

static void base64_encode(const uint8* src, int len, uint8* dest)
{
    uint32 w;
    uint8 t;
    while(len >= 3)
    {
        w = ((uint32)src[0])<<16 | ((uint32)src[1])<<8 | ((uint32)src[2]);

        printf_high("base64:0x%08X, %X, %X, %X\r\n", w, src[0], src[1], src[2]);

        dest[0] = base64_enc_map((w>>18)&0x3F);
        dest[1] = base64_enc_map((w>>12)&0x3F);
        dest[2] = base64_enc_map((w>>6)&0x3F);
        dest[3] = base64_enc_map((w)&0x3F);

        len-=3;
        src+=3;
        dest+=4;
    }
    if(!len)
        return;

    if(len == 2)
    {
        w = ((uint32)src[0])<<8 | ((uint32)src[1]);

        dest[0] = base64_enc_map((w>>10)&0x3F);
        dest[1] = base64_enc_map((w>>4)&0x3F);
        dest[2] = base64_enc_map((w&0x0F)<<2);
        dest[3] = '=';
    }
    else
    {
        w = src[0];

        dest[0] = base64_enc_map((w>>2)&0x3F);
        dest[1] = base64_enc_map((w&0x03)<<4);
        dest[2] = '=';
        dest[3] = '=';
    }
}

static int base64_decode_len(int len)
{
    return ((len+3)/4)*3;
}

static uint32 base64_dec_map(uint8 n)
{
    if(n >= 'A' && n <= 'Z')
        return n - 'A';
    if(n >= 'a' && n <= 'z')
        return n - 'a' + 26;
    if(n >= '0' && n <= '9')
        return n - '0' + 52;
    return n == '+' ? 62 : 63;
}

static int base64_decode(const uint8* src, int len, uint8* dest)
{
    uint32 w;
    uint8 t;
    int result = 0;

    // remove trailing =
    while(src[len-1] == '=')
        len--;

    while(len >= 4)
    {
        w = (base64_dec_map(src[0]) << 18) |
            (base64_dec_map(src[1]) << 12) |
            (base64_dec_map(src[2]) << 6) |
            base64_dec_map(src[3]);

        dest[0] = (w>>16)&0xFF;
        dest[1] = (w>>8)&0xFF;
        dest[2] = (w)&0xFF;

        len-=4;
        src+=4;
        dest+=3;
        result+=3;
    }
    if(!len)
        return result;

    if(len == 3)
    {
        w = (base64_dec_map(src[0]) << 18) |
            (base64_dec_map(src[1]) << 12) |
            (base64_dec_map(src[2]) << 6) |
            0;

        dest[0] = (w>>16)&0xFF;
        dest[1] = (w>>8)&0xFF;
        result+=2;
    }
    else if(len == 2)
    {
        w = (base64_dec_map(src[0]) << 18) |
            (base64_dec_map(src[1]) << 12) |
            0;

        dest[0] = (w>>16)&0xFF;
        result+=1;
    }
    else
    {
        // should not happen.
    }
    return result;
}

static int16 iot_hook_AT_command(char* cmd_buf, int16 AtCmdLen)
{
    int8 i;
    int8 cmdlen = 0;
    int8 para_pos = 0;
    int8 cmdtype = 0;

    cmd_buf[AtCmdLen] = 0;

    /* split command, parameter */
    i=0;
    while(cmd_buf[i] != '=' && cmd_buf[i] != '?' && cmd_buf[i])
        i++;
    cmdlen = i;
    if(cmd_buf[i] == '=')
    {
        cmdtype = 1;
        para_pos = i+1;
    }
    else if(cmd_buf[i] == '?')
        cmdtype = 2;
    else
        cmdtype = 0;

    // process command according to command
    if(!memcmp(cmd_buf, "GPI", 3))
    {
        int8 pin = cmd_buf[3] - '0';
        if(pin < 0 || pin > 4 || cmdtype == 0)
        {
            _at_response_err();
        }
        else if(cmdtype == 2)
        {
            uint32 v = 0;
            // query gpio
            if(iot_gpio_input(pin, &v) == 0)
            {
                sprintf(cmd_buf, "\r\n+GPIO%d:%d\r\n", pin, v);
                _at_response(cmd_buf);
            }
            else
                _at_response_err();
        }
        else if(cmdtype == 1)
        {
            // set gpio
            int32 v = atoi(cmd_buf + para_pos);

            if(v != 0 && v != 1)
                _at_response_err();
            else
            {
                iot_sw_pwm_del(pin);
                iot_gpio_output(pin, v);
                _at_response_ok();
            }
        }
    }
    if(!memcmp(cmd_buf, "GPW", 3))
    {
        int8 pin = cmd_buf[3] - '0';
        if(pin < 0 || pin > 4 || cmdtype == 0)
        {
            _at_response_err();
        }
        else if(cmdtype == 2)
        {
            _at_response_err();
        }
        else if(cmdtype == 1)
        {
            char dummy = 0;
            char* tokens[2];
            int dutycycle, resolution;

            tokens[0] = tokens[1] = &dummy;
            _parse_tokens(cmd_buf + para_pos, tokens, 2);

            dutycycle = atoi(tokens[0]);
            resolution = atoi(tokens[1]);

            // check for parameters and apply
            if(resolution <= 0 || dutycycle < 0 || dutycycle > resolution)
                _at_response_err();
            else if(iot_sw_pwm_add(pin, dutycycle, resolution) == 0)
                _at_response_ok();
            else
                _at_response_err();
                
        }
    }
    else if(!memcmp(cmd_buf, "WMAC", 4))
    {
        if(cmdtype == 2)
        {
#ifdef CONFIG_STATION
            sprintf(cmd_buf, "\r\n+WMAC:%02X-%02X-%02X-%02X-%02X-%02X\r\n", pIoTStaCfg->MyMacAddr[0], pIoTStaCfg->MyMacAddr[1], pIoTStaCfg->MyMacAddr[2], pIoTStaCfg->MyMacAddr[3], pIoTStaCfg->MyMacAddr[4], pIoTStaCfg->MyMacAddr[5]);
            _at_response(cmd_buf);
#elif defined(CONFIG_SOFTAP)
            sprintf(cmd_buf, "\r\n+WMAC:%02X-%02X-%02X-%02X-%02X-%02X\r\n", pIoTApCfg->MBSSID.Bssid[0], pIoTApCfg->MBSSID.Bssid[1], pIoTApCfg->MBSSID.Bssid[2], pIoTApCfg->MBSSID.Bssid[3], pIoTApCfg->MBSSID.Bssid[4], pIoTApCfg->MBSSID.Bssid[5]);
            _at_response(cmd_buf);
#else
            _at_response_err();
#endif
        }
        else
            _at_response_err();
    }
    else if(!memcmp(cmd_buf, "WSWM", 4))
    {
        if(cmdtype == 1)
        {
            // magic address of mode config = 0x18001
            uint8 dummy;
            switch(atoi(cmd_buf+para_pos))
            {
            case 1:
                dummy = 0;
                spi_flash_write(0x18001, &dummy, 1);
                _at_response_ok();
                iot_sys_reboot();
                break;
            case 2:
                dummy = 1;
                spi_flash_write(0x18001, &dummy, 1);
                _at_response_ok();
                iot_sys_reboot();
                break;
            default:
                _at_response_err();
            }
        }
        if(cmdtype == 2)
        {
#ifdef CONFIG_STATION
            _at_response("\r\n+WSWM:1\r\n");
#elif defined(CONFIG_SOFTAP)
            _at_response("\r\n+WSWM:2\r\n");
#else
            _at_response_err();
#endif
        }
        else
            _at_response_err();
    }
#ifdef CONFIG_STATION
    else if(!memcmp(cmd_buf, "WCAP", 4))
    {
        if(cmdtype == 1)
        {
            char* p = cmd_buf + para_pos;

            if(*p == 0)
            {
                // disconnect case, reset station config and reboot
                reset_sta_cfg();
                // TBD make sure disconnect
                iot_sys_reboot();
                _at_response_ok();
            }
            else
            {
                // parse parameters
                uint8 PMK[40];
                int8 authmode;
                char dummy = 0;
                char* tokens[3];

                tokens[0] = tokens[1] = tokens[2] = &dummy;
                _parse_tokens(p, tokens, 3);

                // check for parameters
                authmode = atoi(tokens[2]);

                if(strlen(tokens[0]) > MAX_LEN_OF_SSID || strlen(tokens[1]) > CIPHER_TEXT_LEN)
                    _at_response_err();

                // because calculate PMK need around 6 secs, response first
                _at_response_ok();

                // store the setting and return to init.
                pIoTStaCfg->AuthMode        = authmode;
                pIoTStaCfg->SsidLen         = strlen(tokens[0]);
                pIoTStaCfg->PassphaseLen    = strlen(tokens[1]);
                memcpy(pIoTStaCfg->Ssid,        tokens[0],  pIoTStaCfg->SsidLen);
                memcpy(pIoTStaCfg->Passphase,   tokens[1],  pIoTStaCfg->PassphaseLen);
                RtmpPasswordHash(pIoTStaCfg->Passphase, pIoTStaCfg->Ssid,
                                 pIoTStaCfg->SsidLen, PMK);
                memcpy(pIoTStaCfg->PMK,         PMK,    LEN_PMK);
                store_sta_cfg();
                g_useFlashSetting = TRUE;
                wifi_state_chg(WIFI_STATE_INIT, 0);
            }

        }
        else
            _at_response_err();

    }
    else if(!memcmp(cmd_buf, "WQIP", 4))
    {
        if(cmdtype == 2)
        {
            uip_ipaddr_t ip, gateway;
            uip_ipaddr_t* dns;
            uip_gethostaddr(&ip);
            uip_getdraddr(&gateway);
            dns = resolv_getserver();
            sprintf(cmd_buf, "\r\n+WQIP:%d.%d.%d.%d,%d.%d.%d.%d,%d.%d.%d.%d\r\n", 
                uip_ipaddr1(&ip), uip_ipaddr2(&ip), uip_ipaddr3(&ip), uip_ipaddr4(&ip), 
                uip_ipaddr1(&gateway), uip_ipaddr2(&gateway), uip_ipaddr3(&gateway), uip_ipaddr4(&gateway), 
                uip_ipaddr1(dns), uip_ipaddr2(dns), uip_ipaddr3(dns), uip_ipaddr4(dns)
                );

            _at_response(cmd_buf);

        }
        else
            _at_response_err();
    }
    else if(!memcmp(cmd_buf, "WDNL", 4))
    {
        if(cmdtype == 1)
        {
            char* p = cmd_buf + para_pos;

            if(*p == 0 || strlen(p) >= 32) // uip resolver only has 32bytes buffer for name
            {
                _at_response_err();
            }
            else
            {
                // parse parameters
                uip_ipaddr_t* ip;
                ip = resolv_lookup(p);
                if(ip)
                {
                    char buf[64];
                    _at_response_ok();
                    sprintf(buf, "\r\n+WDNL:%s,%d.%d.%d.%d\r\n", p,
                        uip_ipaddr1(ip), uip_ipaddr2(ip), uip_ipaddr3(ip), uip_ipaddr4(ip));

                    _at_response(buf);
                }
                else
                {
                    resolv_query(p);
                    _at_response_ok();
                }
            }
        }
        else
            _at_response_err();

    }
#endif
#ifdef CONFIG_SOFTAP
    else if(!memcmp(cmd_buf, "WAPC", 4))
    {
        if(cmdtype == 1)
        {
            char* p = cmd_buf + para_pos;

            if(*p == 0)
            {
                _at_response_err();
            }
            else
            {
                // parse parameters
                int8 authmode;
                char dummy = 0;
                char* tokens[3];

                tokens[0] = tokens[1] = tokens[2] = &dummy;
                _parse_tokens(p, tokens, 3);

                // check for parameters
                authmode = atoi(tokens[2]);

                if(strlen(tokens[0]) > MAX_LEN_OF_SSID || strlen(tokens[1]) > CIPHER_TEXT_LEN)
                    _at_response_err();

                // update and store config
                iot_apcfg_update(tokens[0], authmode, tokens[1], 0);
                store_ap_cfg();
                _at_response_ok();
            }
        }
        else if(cmdtype == 2)
        {
            char buf[20+MAX_LEN_OF_SSID+CIPHER_TEXT_LEN];
            memset(buf, 0, sizeof(buf));
            strcpy(buf, "\r\n+WAPC:");
            strncpy(buf + strlen(buf), pIoTApCfg->MBSSID.Ssid, pIoTApCfg->MBSSID.SsidLen);
            strcpy(buf + strlen(buf), ",");
            strncpy(buf + strlen(buf), pIoTApCfg->MBSSID.Passphase, pIoTApCfg->MBSSID.PassphaseLen);
            sprintf(buf + strlen(buf), ",%d\r\n", 
                pIoTApCfg->MBSSID.AuthMode
                );

            _at_response(buf);
        }
        else
            _at_response_err();
    }
#endif
    else if(!memcmp(cmd_buf, "WSO", 3)) // Socket Open <remoteip>,<rport>,<mode>
    {
        if(cmdtype == 1)
        {
            char* p = cmd_buf + para_pos;

            if(*p == 0)
            {
                _at_response_err();
            }
            else
            {
                // parse parameters
                int mode, port;
                char dummy = 0;
                char* tokens[3];
                uip_ipaddr_t ip;

                tokens[0] = tokens[1] = tokens[2] = &dummy;
                _parse_tokens(p, tokens, 3);

                port = atoi(tokens[1]);
                mode = atoi(tokens[2]);
                if(!uiplib_ipaddrconv(tokens[0], (unsigned char *)&ip))
                {
                    _at_response_err();
                }
                else if (mode == 0)
                {
                    // TCP
                    UIP_CONN *conn;
                    conn = uip_connect(&ip, htons(port));
                    if(conn)
                    {
                        sprintf(cmd_buf, "\r\n+WSO:%d\r\n", HTONS(conn->lport));
                        _at_response(cmd_buf);
                    }
                    else
                        _at_response_err();
                }
                else if (mode == 1)
                {
                    // UDP
                    UIP_UDP_CONN *conn = NULL;
                    conn = uip_udp_new(&ip, HTONS(port));
                    if(conn)
                    {
                        sprintf(cmd_buf, "\r\n+WSO:%d\r\n", HTONS(conn->lport));
                        _at_response(cmd_buf);
                    }
                    else
                        _at_response_err();
                }
                else
                    _at_response_err();
            }
        }
        else
            _at_response_err();

    }
    else if(!memcmp(cmd_buf, "WSL", 3)) // Socket Listen <lport>,<mode>
    {
        if(cmdtype == 1)
        {
            char* p = cmd_buf + para_pos;

            if(*p == 0)
            {
                _at_response_err();
            }
            else
            {
                // parse parameters
                int mode, port;
                char dummy = 0;
                char* tokens[2];

                tokens[0] = tokens[1] = &dummy;
                _parse_tokens(p, tokens, 2);

                port = atoi(tokens[0]);
                mode = atoi(tokens[1]);
                if (mode == 0)
                {
                    // TCP
                    uip_listen(HTONS(port));
                    _at_response_ok();
                }
                else if (mode == 1)
                {
                    // TBD, not support yet
                    _at_response_err();
                }
            }
        }
        else
            _at_response_err();
    }    
    else if(!memcmp(cmd_buf, "WSW", 3)) // Socket Write <lport>,<data>
    {
        if(_socket_output_buffer_in_use)
        {
            _at_response("\r\nBUSY\r\n");
        }
        else if(cmdtype == 1)
        {
            char* p = cmd_buf + para_pos;

            if(*p == 0)
            {
                _at_response_err();
            }
            else
            {
                // parse parameters
                int port, len;
                char dummy = 0;
                char* tokens[2];

                tokens[0] = tokens[1] = &dummy;
                _parse_tokens(p, tokens, 2);

                port = atoi(tokens[0]);
                len = strlen(tokens[1]);

                if(len > 64)
                {
                    _at_response_err();
                }
                else
                {
                    _socket_output_buffer_datalen = base64_decode(tokens[1], len, _socket_output_buffer);
                    _socket_output_buffer_in_use = TRUE;
                    _socket_output_port = port;
                    _at_response_ok();
                }
            }
        }
        else
            _at_response_err();
    }
    /*
    else if(!memcmp(cmd_buf, "", 0))
    {
    }
    */

    return 0;
}

void iot_hook_state_machine()
{
#ifdef CONFIG_STATION
    static bool connected = FALSE;

    switch (pIoTMlme->CurrentWifiState)
    {
        case WIFI_STATE_CONNED:
            if(!connected)
            {
                char buf[40+MAX_LEN_OF_SSID+CIPHER_TEXT_LEN];

                memset(buf, 0, sizeof(buf));
                strcpy(buf, "\r\n+WCAP:");
                strncpy(buf + strlen(buf), pIoTStaCfg->Ssid, pIoTStaCfg->SsidLen);
                strcpy(buf + strlen(buf), ",");
                strncpy(buf + strlen(buf), pIoTStaCfg->Passphase, pIoTStaCfg->PassphaseLen);
                sprintf(buf + strlen(buf), ",%d,%02X-%02X-%02X-%02X-%02X-%02X\r\n", pIoTStaCfg->AuthMode, pIoTStaCfg->Bssid[0], pIoTStaCfg->Bssid[1], pIoTStaCfg->Bssid[2], pIoTStaCfg->Bssid[3], pIoTStaCfg->Bssid[4], pIoTStaCfg->Bssid[5]);
                _at_response(buf);
                connected = TRUE;
            }
            break;
        default:
            if(connected)
            {
                _at_response("\r\n+WCAP:\r\n");
                connected = FALSE;
            }
    }
#endif
}

void iot_hook_uip_appcall(u16_t lport, uip_ipaddr_t* rip, u16_t rport, bool is_udp)
{
    char buf[100];

    printf_high("UIP p=%d, f=0x%02X r=%d\r\n", 
        HTONS(lport), uip_flags, HTONS(rport));

    if (uip_newdata()) 
    {
        char *src = uip_appdata;
        char *dest = buf;
        int remain = uip_datalen();
        int r;

        while(remain > 0)
        {
            int part = remain > 48 ? 48 : remain;

            sprintf(buf, "\r\n+WSDR:%d,%d,", HTONS(lport), base64_encode_len(part));
            dest = buf + strlen(buf);
            base64_encode((uint8*)src, part, (uint8*)dest);
            dest+= base64_encode_len(part);
            dest[0] = '\r';
            dest[1] = '\n';
            dest[2] = 0;
            _at_response(buf);
            src+=part;
            remain-=part;
        }
    }

    if(_socket_output_buffer_in_use)
    {
        if (_socket_output_port == HTONS(lport))
        {
            if(uip_poll())
            {
                uip_send(_socket_output_buffer, _socket_output_buffer_datalen);
                if(is_udp)
                {
                    _socket_output_buffer_in_use = FALSE;
                    sprintf(buf, "\r\n+WSDS:%d\r\n", _socket_output_port);
                    _at_response(buf);
                }
            }
            else if(uip_acked())
            {
                if(!is_udp)
                {
                    _socket_output_buffer_in_use = FALSE;
                    sprintf(buf, "\r\n+WSDS:%d\r\n", _socket_output_port);
                    _at_response(buf);
                }
            }
        }
    }

    if(uip_aborted() || uip_closed())
    {
        sprintf(buf, "\r\n+WSS:%d,%d\r\n", HTONS(lport), 0);
        _at_response(buf);
    }
    if(uip_connected())
    {
        sprintf(buf, "\r\n+WSS:%d,%d\r\n", HTONS(lport), 1);
        _at_response(buf);
    }
}

#ifdef CONFIG_STATION
void resolv_found(char *name, u16_t *ipaddr)
{
    char buf[64];
    sprintf(buf, "\r\n+WDNL:%s,%d.%d.%d.%d\r\n", name,
        uip_ipaddr1(ipaddr), uip_ipaddr2(ipaddr), uip_ipaddr3(ipaddr), uip_ipaddr4(ipaddr));

    _at_response(buf);
}
#endif

#else
static int16 iot_hook_AT_command(char* cmd_buf, int16 AtCmdLen)
{
    return 0;
}

void iot_hook_state_machine()
{

}

void iot_hook_uip_appcall(u16_t lport, uip_ipaddr_t* rip, u16_t rport)
{

}
#endif

int16 iot_iwcmd_parser(puchar cmd_buf, int16 AtCmdLen)
{
    int16 ret_code = 0;

#if (ATCMD_ATE_SUPPORT == 1)
    cmd_buf[AtCmdLen] = '\0';
    gATEInfo.ATECmdFmt = ATE_CMD_TYPE_IWPRIV;
    ret_code = iot_exec_atcmd_ate_cal(cmd_buf, AtCmdLen);
#endif

    return iot_hook_AT_command((char*)cmd_buf, AtCmdLen);
}


#if (UART_INTERRUPT != 1)
int16 iot_atcmd_detect(uint8* pType)
{
    uint8 ch = 0;
    int32 read = -1;
    int16 ret_len = 0;

    static uint8  CmdType = PKT_UNKNOWN;
    static uint8  ATMatchNum = 0;
    static uint8  IWMatchNum = 0;
    char ATCmdPrefixAT[]=AT_CMD_PREFIX;
    char ATCmdPrefixIW[]=AT_CMD_PREFIX2;

    if (UART_LSROverErr() != 0)
        return -1;

    /*Notice: Must not use printf_high in the while block,  the Rx FIFO,RxINT and ringbuf will mess*/
    while (1) {
        read = UART_GetChar((uint8*)&ch);

        if (read == -1)
            return -1;

#if (UARTRX_TO_AIR_LEVEL == 1)
        uart_rb_push(ch);

#elif (UARTRX_TO_AIR_LEVEL == 2)
        if (iot_uart_rx_mode == UARTRX_PUREDATA_MODE) {
            if (cmd_len >= AT_CMD_MAX_LEN) {
                cmd_len = 0;
            }
            command[cmd_len] = ch;
            cmd_len++;
            continue;
        }
#endif

        if (CmdType == PKT_UNKNOWN) {
            /*case 1:AT#*/
            if (ATCmdPrefixAT[ATMatchNum] == ch)
                ATMatchNum++;
            else
                ATMatchNum = 0;

            /*case 2:iwpriv ra0*/
            if (ATCmdPrefixIW[IWMatchNum] == ch)
                IWMatchNum++;
            else
                IWMatchNum = 0;

            if (( ATMatchNum == sizeof(ATCmdPrefixAT)-1 ) ||   //match case 1: AT#
                ( IWMatchNum == sizeof(ATCmdPrefixIW)-1 ) ) {  //match case 2:iwpriv ra0
                if ( ATMatchNum == sizeof(ATCmdPrefixAT)-1 )
                    CmdType = PKT_ATCMD;             //match case 1: AT#
                else if ( IWMatchNum == sizeof(ATCmdPrefixIW)-1 )
                    CmdType = PKT_IWCMD;             //match case 2:iwpriv ra0

                ATMatchNum = 0;
                IWMatchNum = 0;
                continue;
            }
        } else if ((PKT_ATCMD == CmdType) || (PKT_IWCMD == CmdType)) {
            if (ch == '\n' || ch == '\r') {
                *pType = CmdType;
                CmdType = PKT_UNKNOWN;
                ret_len = cmd_len;   /*not include '\n'*/
                cmd_len = 0;
                return ret_len;
            } else {
                command[cmd_len] = ch;
                cmd_len++;
                if (cmd_len >= AT_CMD_MAX_LEN) {
                    CmdType = PKT_UNKNOWN;
                    cmd_len = 0;
                    return -2;
                }
            }
        }
    }
    return 0;

}

void iot_atcmd_hdlr(void)
{
    int16 AtCmdLen = 0;
    int16 ret_code = 0;
    uint8 Type = PKT_UNKNOWN;

    AtCmdLen = iot_atcmd_detect(&Type);

    if (AtCmdLen > 0) {
        printf_high("AtCmdLen=%d, Type=%d \n",AtCmdLen,Type);

        if (Type == PKT_ATCMD)         //match case 1: AT#
            ret_code = iot_atcmd_parser(command, AtCmdLen);
        else if (Type == PKT_IWCMD)    //match case 2:iwpriv ra0
            ret_code = iot_iwcmd_parser(command, AtCmdLen);
        else
            return;

        if (ret_code != 0) {
            //printf_high("exec error: %d\n",ret_code);
        }
    } else if (AtCmdLen == -2) {
        //printf_high("=>%s AT command is too long\n",__FUNCTION__);
    }
}
#endif
