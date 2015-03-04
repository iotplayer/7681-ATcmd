#include <stdio.h>
#include "string.h"
#include "types.h"
#include "iot_api.h"
#include "config.h"
#include "flash_map.h"
#include "uip-conf.h"
#include "iot_custom.h"
#ifdef CONFIG_SOFTAP
#include "ap_pub.h"
#endif

/******************************************************************************
* MODULE NAME:     iot_customer.c
* PROJECT CODE:    __MT7681__
* DESCRIPTION:
* DESIGNER:
* DATE:            Jan 2014
*
* SOURCE CONTROL:
*
* LICENSE:
*     This source code is copyright (c) 2014 Mediatek Tech. Inc.
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

/************************************************/
/* Macros */
/************************************************/
#ifdef CONFIG_STATION
/* Calcurate PMK by 7681 software, it will spend 6sec */
#define PMK_CAL_BY_SW
#endif

/************************************************/
/* Extern and Global Paramenters */
/************************************************/
extern MLME_STRUCT  *pIoTMlme;
#ifdef CONFIG_SOFTAP
extern AP_ADMIN_CONFIG  *pIoTApCfg;
#else
extern STA_ADMIN_CONFIG *pIoTStaCfg;
#endif
#if (UART_INTERRUPT == 1)
/* We can use UART TX POLL scheme(such as debug/test),default is FALSE*/
bool UART_TX_POLL_ENABLE = FALSE;
extern void uart_rx_dispatch(void);
#endif
#if (IOT_SMP_RSSI_COLLECT == 1)
extern RX_RSSI_INFO gRxRSSIInfo;
#endif

IOT_CUST_OP     IoTCustOp;
IOT_CUST_TIMER  IoTCustTimer;
IOT_ADAPTER     IoTpAd;

/*TRUE: printf_high()/DBGPRINT_HIGH() is enabled,    FALSE: printf_high/DBGPRINT_HIGH()  is disabled*/
bool  PRINT_FLAG = TRUE;

/*this is the Common CFG region default table */
IOT_COM_CFG Com_Cfg = {
    DEFAULT_BOOT_FW_IDX,
    DEFAULT_RECOVERY_MODE_STATUS,
    DEFAULT_IO_MODE,

    DEFAULT_UART_BAUDRATE,
    DEFAULT_UART_DATA_BITS,
    DEFAULT_UART_PARITY,
    DEFAULT_UART_STOP_BITS,

    DEFAULT_TCP_UDP_CS,
    DEFAULT_IOT_TCP_SRV_PORT,
    DEFAULT_LOCAL_TCP_SRV_PORT,
    DEFAULT_IOT_UDP_SRV_PORT,
    DEFAULT_LOCAL_UDP_SRV_PORT,
    DEFAULT_USE_DHCP,
    DEFAULT_STATIC_IP,
    DEFAULT_SUBNET_MASK_IP,
    DEFAULT_DNS_IP,
    DEFAULT_GATEWAY_IP,
    DEFAULT_IOT_SERVER_IP,
    DEFAULT_IOT_CMD_PWD
};

/*this is the User CFG region default table */
IOT_USR_CFG Usr_Cfg = {
    DEFAULT_VENDOR_NEME,
    DEFAULT_PRODUCT_TYPE,
    DEFAULT_PRODUCT_NAME
};

/*Default setting of STA Config Block*/

/*Default setting of AP Config Block*/


/*bit0-  read Calibration settings (TxPower/Tx Freq Offset) from [0:Flash,  1:Efuse]*/
uint8 gCaliFrEfuse = 0x00;

/*unit:ms  indicated recovery mode duration*/
#if (ATCMD_RECOVERY_SUPPORT==1)
uint16 gRecoveryModeTime = 4000;
#endif

/*Indicated if use specified SSID to do scan  (TRUE/FALSE)  */
/*the SSID is got from Smart connection state or Flash        */
bool bSpecAP = TRUE;


/************************************************/
/* Functions */
/************************************************/
void iot_cust_scan_done(void)
{
    /* Example for customer's smart connection implementation*/
#if (CFG_SUPPORT_MTK_SMNT == 0)
#ifdef PMK_CAL_BY_SW
    /*Calcurate PMK by 7681 software, it will spend 6sec*/
    if (pIoTStaCfg->AuthMode >= Ndis802_11AuthModeWPA)  {
        /*Deriver PMK by AP 's SSID and Password*/
        uint8 keyMaterial[40] = {0};

        RtmpPasswordHash(pIoTStaCfg->Passphase, pIoTStaCfg->Ssid,
                         pIoTStaCfg->SsidLen,   keyMaterial);
        memcpy(pIoTStaCfg->PMK, keyMaterial, LEN_PMK);

        //printf_high("keyMaterial:");
        //dump(keyMaterial, 40);
    }
#endif
#endif

    /*Do something after AP Scanned*/
    {

    }
}

/*========================================================================
    Routine    Description:
        iot_cust_subtask1

    Arguments:
    Return Value:
========================================================================*/
void iot_cust_subtask1(void)
{

}

/*========================================================================
    Routine    Description:
        iot_cust_preinit

    Arguments:
    Return Value:
========================================================================*/
void iot_cust_preinit(void)
{
    /*The callback in system Boot state, do not print msg as Uart is not ready**/
}



/*========================================================================
    Routine    Description:
        iot_cust_init

    Arguments:
    Return Value:
========================================================================*/
void iot_cust_init(void)
{
    /* run customer initial function */
}





/*========================================================================
    Routine    Description:
        cust_subtask:  sub task functioin

    Arguments:
    Return Value:
========================================================================*/
/* Sample code for custom SubTask and Timer */
void cust_subtask(void)
{
    /* The Task to Handle the AT Command */
#if (ATCMD_SUPPORT == 1)
#if (UART_INTERRUPT == 1)
    uart_rx_dispatch();
#else
    iot_atcmd_hdlr();
#endif
#endif

    /* The Task to Handle the Uart to Wifi feature */
#if (UARTRX_TO_AIR_LEVEL == 2)
    iot_cust_u2w_buf_act();
#endif

    /*  The Task to collect all rx packet and calculate the average signal strength*/
#if (IOT_SMP_RSSI_COLLECT == 1)
    {
        static uint32 PreTime = 0;
        uint32 CurTime = 0;

        CurTime = iot_get_ms_time();
        if (((CurTime - PreTime) >= IOT_SMP_RSSI_COLLECT_PERIOD) || (CurTime < PreTime)) {
            PreTime = CurTime;
            /* one-second periodic execution */
            printf_high("gRxRSSIInfo.AvgRssi0= %d \n\n", gRxRSSIInfo.AvgRssi0);
            gRxRSSIInfo.AvgRssi0 = 0;
        }
    }
#endif

    /*  run cutsomer sub task 1 */
    if (IoTCustOp.IoTCustSubTask1 != NULL)
        IoTCustOp.IoTCustSubTask1();
}


/*========================================================================
    Routine    Description:
        cust_app_init

    Arguments:
    Return Value:
========================================================================*/
void cust_app_init(void)
{
    uint8 i=0;

#if (ATCMD_RECOVERY_SUPPORT==0)
    /*GPIO initial*/
    for (i=0; i<=4; i++) {
        iot_gpio_output(i, iot_gpio_defval());
    }
#endif

#if (IOT_SMP_RSSI_COLLECT == 1)
    NdisZeroMemory(&gRxRSSIInfo, sizeof(RX_RSSI_INFO));
#endif

#ifdef CONFIG_STATION
    /*Beacon lost timeout timer*/
    cnmTimerInitTimer(&pIoTMlme->BeaconTimer, iot_bcn_timeout_action,0, 0);
#endif

    /*for customer timer initialization*/
    cnmTimerInitTimer(&IoTCustTimer.custTimer0,  iot_cust_timer0_timeout_actioin, 0, 0);

#if (UARTRX_TO_AIR_LEVEL == 2)
    iot_cust_u2w_init(UART2WIFI_TIMER_INTERVAL,  UART2WIFI_LEN_THRESHOLD);
#endif

    cnmTimerStartTimer (&IoTCustTimer.custTimer0, 100);


#if (MT7681_POWER_SAVING == 1)
    pIoTMlme->PMLevel = IOT_STA_PWR_SAV_LVL_DEF;
#endif
    
    /* run customer initial function */
    if (IoTCustOp.IoTCustInit != NULL) {
        IoTCustOp.IoTCustInit();
    }
}

/*========================================================================
    Routine    Description:
        pre_init_cfg

    Arguments:
    Return Value:
========================================================================*/
void pre_init_cfg(void)
{
    memcpy(&IoTpAd.ComCfg , &Com_Cfg , sizeof(IOT_COM_CFG));
    memcpy(&IoTpAd.UsrCfg , &Usr_Cfg , sizeof(IOT_USR_CFG));

    /*read Uart setting in the flash, then config uart HW*/
    load_com_cfg();

    /*read user config in the flash*/
    load_usr_cfg();

    /* The entry for customization */
    if (IoTCustOp.IoTCustPreInit != NULL)
        IoTCustOp.IoTCustPreInit();
}

#ifdef CONFIG_STATION
#if (CFG_SUPPORT_MTK_SMNT == 0)
/*========================================================================
    Routine    Description:
        IoT_Cust_SMNT_Sta_Chg_Init

    Arguments:     the callback function while doing wifi_state_chg(WIFI_STATE_SMNT, 0);
                 once jump to SmartConnection State, maybe you need do SmartConnection Initialization here

                          wifi_state_chg(WIFI_STATE_SMNT, 0); shall be implement  after WIFI_STATE_INIT done

    Return Value:     None

========================================================================*/

void IoT_Cust_SMNT_Sta_Chg_Init(void)
{
    /* Example for customer's smart connection implementation*/

    /*Smnt is valid in STA mode, and should not available in SoftAP mode*/
    iot_set_rxfilter(0x7f93);    /*Enable Sniffer NotToMe unicast, not myBSS Packet*/
    pIoTMlme->DataEnable = 1;  /*Enable to handle Data frame in WifiRxDoneInterruptHandle */

    /* Once jump to SmartConnection State, do Initialize here for customer's SmartConnection sub stateMachine */
    {
        //customer smart connect state initialization
    }
}


/*========================================================================
    Routine    Description:
        IoT_Cust_SM_Smnt

    Arguments:     This is the entry of customer's smart connection (smnt)
    Return Value:  None
========================================================================*/
void IoT_Cust_SM_Smnt(void)
{
    /* Example for customer's smart connection implementation*/

    /*Step1:  create parameter to store the smnt information */
    /* or it is better to create a structure*/
    uint8  Ssid[MAX_LEN_OF_SSID+1] = "belkin";
    uint8  Passphase[CIPHER_TEXT_LEN] ="12345678" ;
    uint8  PMK[CIPHER_TEXT_LEN];            //= "PMK1234";

    /* Must initialize Passphase and PMK  if no reference value be set on the above*/
    //memset(Passphase, 0 ,CIPHER_TEXT_LEN);
    memset(PMK, 0 ,CIPHER_TEXT_LEN);

    /*Step2:  create a sub stateMachine for smnt */
    /* Notice: the real Rx packet parser should not here, but deal with STAHandleRxDataFrame() */
    {
        /* Smnt connection state machine start */
        /* Smnt connection done */
    }

    /*Step3:  After smart connection done */
    /* it need set smart connection information , then start to scan*/
    /*The Authmode shall be detected in scan state again, if AuthMode>WPA, PMK will be calculated in scan state*/
    pIoTStaCfg->AuthMode       = Ndis802_11AuthModeOpen;
    pIoTStaCfg->SsidLen          = strlen(Ssid);
    pIoTStaCfg->PassphaseLen = strlen(Passphase); //sizeof(Passphase);
    memcpy(pIoTStaCfg->Ssid,       Ssid,      pIoTStaCfg->SsidLen);
    memcpy(pIoTStaCfg->Passphase, Passphase, pIoTStaCfg->PassphaseLen);
    memcpy(pIoTStaCfg->PMK,  PMK, strlen(PMK));

    wifi_state_chg(WIFI_STATE_SCAN, 0);  /* Change wifi state to SCAN*/
}
#endif  /*(CFG_SUPPORT_MTK_SMNT == 0)*/
#endif  /*CONFIG_STATION*/



/*========================================================================
    Routine    Description:
        iot_cust_ops: register hook function for customization

    Arguments:     None
    Return Value:  None
========================================================================*/
void iot_cust_ops(void)
{
    IoTCustOp.IoTCustPreInit         = iot_cust_preinit;                 /*The callback in system Boot state, do not print msg as Uart is not ready*/
    IoTCustOp.IoTCustInit             = iot_cust_init;                       /*The callback in system initial state*/

    /*The callback for each of Wifi State Machine*/
#ifdef CONFIG_STATION
    IoTCustOp.IoTCustWifiSMInit      = NULL;                                  /*Callback function while for STA wifi stateMachine initialize;*/
#if (CFG_SUPPORT_MTK_SMNT == 1)
    IoTCustOp.IoTCustWifiSMSmnt      = SMTCN_process;             /*MTK SmartConnection sub statemachine*/
    IoTCustOp.IoTCustSMNTStaChgInit= SMTCN_state_chg_init;  /*MTK SmartConnection sub statemachine initial function*/
#else
    IoTCustOp.IoTCustWifiSMSmnt      = IoT_Cust_SM_Smnt;                   /*Customer's SmartConnection sub statemachine*/
    IoTCustOp.IoTCustSMNTStaChgInit= IoT_Cust_SMNT_Sta_Chg_Init;  /*Customer's SmartConnection sub statemachine initial function*/
#endif
    IoTCustOp.IoTCustScanDone         = iot_cust_scan_done;     /*Callback function while Scan done;*/
    IoTCustOp.IoTCustWifiSMConnect  = NULL;                               /*Callback function while STA got IP address done;*/
#endif

    /* The callback in  the wifi main task , it shall be called every cycle of the wifi main task*/
    IoTCustOp.IoTCustSubTask1         = iot_cust_subtask1;
}



/*========================================================================
    Routine    Description:
        iot_cust_timer0_timeout_actioin

    Arguments:
    Return Value:
========================================================================*/
void iot_cust_timer0_timeout_actioin(uint32 param, uint32 param2)
{
    printf("iot_cust_timer0_timeout_actioin\n");

#if (UARTRX_TO_AIR_LEVEL == 2)
    iot_cust_u2w_mode_chg_hdlr();
#endif

    cnmTimerStartTimer (&IoTCustTimer.custTimer0, 100);
}


/*========================================================================
    Routine    Description:
        iot_gpio_defval

    Arguments:
    Return Value:    gpio defaut setting
========================================================================*/
int32 iot_gpio_defval(void)
{
    return DEFAULT_GPIO04_OUTVAL;
}



/*========================================================================
    Routine    Description:
        reset_cfg

    Arguments:
    Return Value:
========================================================================*/
void reset_cfg(void)
{
#ifdef CONFIG_STATION
    /* clean all content in the STA region of the flash */
    reset_sta_cfg();
#endif

#ifdef CONFIG_SOFTAP
    /* clean all content in the AP region of the flash */
    reset_ap_cfg();
#endif

    /* set default in the COMMON region of the flash */
    reset_com_cfg(TRUE);

    /* set default in the USER region of the flash */
    reset_usr_cfg(TRUE);
}

/*========================================================================
    Routine    Description:
        load_usr_cfg

    Arguments:
    Return Value:
========================================================================*/
bool load_usr_cfg(void)
{
    //printf_high("%s \n",__FUNCTION__);

#if (ENABLE_FLASH_SETTING == 1)
    /* read settings stored on flash USER CONFIG BLOCK */
    //memset(IoTpAd.flash_rw_buf, 0, sizeof(IoTpAd.flash_rw_buf));
    spi_flash_read(FLASH_USR_CFG_BASE, IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));

    /*use stored flag to shrink code size*/
    if (IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_INFO_STORED] == PRODUCT_INFO_STORED)  {
        /*if flash stored valid vendor name , product name and product type*/
        memcpy(IoTpAd.UsrCfg.VendorName,    &IoTpAd.flash_rw_buf[FLASH_USR_CFG_VENDOR_NAME],    FLASH_USR_CFG_VENDOR_NAME_LEN);
        memcpy(IoTpAd.UsrCfg.ProductType,    &IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_TYPE],    FLASH_USR_CFG_PRODUCT_TYPE_LEN);
        memcpy(IoTpAd.UsrCfg.ProductName,   &IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_NAME],  FLASH_USR_CFG_PRODUCT_NAME_LEN);
    } else {
        reset_usr_cfg(TRUE);
    }
#else
     memcpy(&IoTpAd.UsrCfg , &Usr_Cfg , sizeof(IOT_USR_CFG));
#endif

    return TRUE;
}

/*========================================================================
    Routine    Description:
        reset_usr_cfg

    Arguments:
    Return Value:
========================================================================*/

bool reset_usr_cfg(bool bUpFlash)
{
    //printf_high("%s \n",__FUNCTION__);
    
    /* Notice: erase User config Flash region , default size is one sector [4KB] */
    spi_flash_erase_sector(FLASH_USR_CFG_BASE);
    
    memset(IoTpAd.flash_rw_buf ,0xff, sizeof(IoTpAd.flash_rw_buf));
    memcpy(&IoTpAd.UsrCfg , &Usr_Cfg , sizeof(IOT_USR_CFG));

    if (bUpFlash == TRUE) {
        IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_INFO_STORED] = PRODUCT_INFO_STORED;
        memcpy(&IoTpAd.flash_rw_buf[FLASH_USR_CFG_VENDOR_NAME] ,  Usr_Cfg.VendorName, FLASH_USR_CFG_VENDOR_NAME_LEN);
        memcpy(&IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_TYPE] , Usr_Cfg.ProductType, FLASH_USR_CFG_PRODUCT_TYPE_LEN);
        memcpy(&IoTpAd.flash_rw_buf[FLASH_USR_CFG_PRODUCT_NAME] , Usr_Cfg.ProductName, FLASH_USR_CFG_PRODUCT_NAME_LEN);

        spi_flash_write(FLASH_USR_CFG_BASE, IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));
    }

    return TRUE;
}

/*========================================================================
    Routine    Description:
        default_boot_cfg

    Arguments:
    Return Value:
========================================================================*/
bool default_boot_cfg(void)
{
    //printf_high("%s \n",__FUNCTION__);

    IoTpAd.ComCfg.BootFWIdx           = DEFAULT_BOOT_FW_IDX;
    IoTpAd.ComCfg.RecovModeStatus = DEFAULT_RECOVERY_MODE_STATUS;
    IoTpAd.ComCfg.IOMode               = DEFAULT_IO_MODE;

    return TRUE;
}

bool default_uart_cfg(void)
{
    //printf_high("%s \n",__FUNCTION__);

    IoTpAd.ComCfg.UART_Baudrate    = DEFAULT_UART_BAUDRATE;
    IoTpAd.ComCfg.UART_DataBits     = DEFAULT_UART_DATA_BITS;
    IoTpAd.ComCfg.UART_Parity         = DEFAULT_UART_PARITY;
    IoTpAd.ComCfg.UART_StopBits     = DEFAULT_UART_STOP_BITS;

    return TRUE;
}

bool default_ip_cfg(void)
{
    //printf_high("%s \n",__FUNCTION__);

    IoTpAd.ComCfg.TCPUDP_CS                = DEFAULT_TCP_UDP_CS; //Com_Cfg.TCPUDP_CS;
    IoTpAd.ComCfg.IoT_TCP_Srv_Port       = DEFAULT_IOT_TCP_SRV_PORT;   //Com_Cfg.IoT_TCP_Srv_Port;
    IoTpAd.ComCfg.Local_TCP_Srv_Port     = DEFAULT_LOCAL_TCP_SRV_PORT;   //Com_Cfg.Local_TCP_Srv_Port;
    IoTpAd.ComCfg.IoT_UDP_Srv_Port       = DEFAULT_IOT_UDP_SRV_PORT;   //Com_Cfg.IoT_UDP_Srv_Port;
    IoTpAd.ComCfg.Local_UDP_Srv_Port     = DEFAULT_LOCAL_UDP_SRV_PORT;   //Com_Cfg.Local_UDP_Srv_Port;

    IoTpAd.ComCfg.Use_DHCP = DEFAULT_USE_DHCP;    //Com_Cfg.Use_DHCP;
    memcpy(IoTpAd.ComCfg.STATIC_IP,        Com_Cfg.STATIC_IP,     MAC_IP_LEN);
    memcpy(IoTpAd.ComCfg.SubnetMask_IP,    Com_Cfg.SubnetMask_IP, MAC_IP_LEN);
    memcpy(IoTpAd.ComCfg.DNS_IP,        Com_Cfg.DNS_IP,        MAC_IP_LEN);
    memcpy(IoTpAd.ComCfg.GateWay_IP,    Com_Cfg.GateWay_IP,    MAC_IP_LEN);
    memcpy(IoTpAd.ComCfg.IoT_ServeIP,    Com_Cfg.IoT_ServeIP,    MAC_IP_LEN);

    return TRUE;
}


/*========================================================================
    Routine    Description:
        load_com_cfg

    Arguments:
    Return Value:
========================================================================*/
bool load_com_cfg(void)
{
    //printf_high("%s \n",__FUNCTION__);

#if (ENABLE_FLASH_SETTING == 1)
    /* read settings stored on flash Common CONFIG BLOCK */
    //memset(IoTpAd.flash_rw_buf, 0, sizeof(IoTpAd.flash_rw_buf));
    spi_flash_read(FLASH_COM_CFG_BASE, IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));

    /*use stored flag to shrink code size*/
    if (IoTpAd.flash_rw_buf[FLASH_COM_CFG_INFO_STORED] == COMMON_INFO_STORED) {
        IoTpAd.ComCfg.BootFWIdx            =  IoTpAd.flash_rw_buf[FLASH_COM_CFG_BOOT_IDX];
        IoTpAd.ComCfg.RecovModeStatus     =  IoTpAd.flash_rw_buf[FLASH_COM_CFG_RECOVERY_MODE_STATUS];
        IoTpAd.ComCfg.IOMode            =  IoTpAd.flash_rw_buf[FLASH_COM_CFG_IO_SET];

        memcpy(&IoTpAd.ComCfg.UART_Baudrate, &IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_BAUD],   FLASH_COM_CFG_UART_BAUD_LEN);
        IoTpAd.ComCfg.UART_DataBits    =  IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_DATA_BITS];
        IoTpAd.ComCfg.UART_Parity     =  IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_PARITY];
        IoTpAd.ComCfg.UART_StopBits    =  IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_STOP_BITS];

        if ((IoTpAd.ComCfg.UART_Baudrate > UART_BAUD_3200000) ||
            (IoTpAd.ComCfg.UART_Parity > pa_space) ||
            ((IoTpAd.ComCfg.UART_DataBits < len_5) || (IoTpAd.ComCfg.UART_DataBits > len_8))  ||
            ((IoTpAd.ComCfg.UART_StopBits < sb_1)  || (IoTpAd.ComCfg.UART_StopBits > sb_1_5)) ) {
            default_uart_cfg();
        }

        IoTpAd.ComCfg.TCPUDP_CS        =  IoTpAd.flash_rw_buf[FLASH_COM_CFG_TCPUDP_CS_SET];
        IoTpAd.ComCfg.Use_DHCP         =  IoTpAd.flash_rw_buf[FLASH_COM_CFG_USE_DHCP];
        memcpy(&IoTpAd.ComCfg.IoT_TCP_Srv_Port,    &IoTpAd.flash_rw_buf[FLASH_COM_CFG_IOT_TCP_SRV_PORT],FLASH_COM_CFG_IOT_TCP_SRV_PORT_LEN);
        memcpy(&IoTpAd.ComCfg.Local_TCP_Srv_Port,    &IoTpAd.flash_rw_buf[FLASH_COM_CFG_LOCAL_TCP_SRV_PORT],FLASH_COM_CFG_LOCAL_TCP_SRV_PORT_LEN);
        memcpy(&IoTpAd.ComCfg.IoT_UDP_Srv_Port, &IoTpAd.flash_rw_buf[FLASH_COM_CFG_IOT_UDP_SRV_PORT],FLASH_COM_CFG_IOT_UDP_SRV_PORT_LEN);
        memcpy(&IoTpAd.ComCfg.Local_UDP_Srv_Port,    &IoTpAd.flash_rw_buf[FLASH_COM_CFG_LOCAL_UDP_SRV_PORT],FLASH_COM_CFG_LOCAL_UDP_SRV_PORT_LEN);
        memcpy(IoTpAd.ComCfg.STATIC_IP,        &IoTpAd.flash_rw_buf[FLASH_COM_CFG_STATIC_IP],         FLASH_COM_CFG_STATIC_IP_LEN);
        memcpy(IoTpAd.ComCfg.SubnetMask_IP,    &IoTpAd.flash_rw_buf[FLASH_COM_CFG_SUBNET_MASK_IP], FLASH_COM_CFG_SUBNET_MASK_IP_LEN);
        memcpy(IoTpAd.ComCfg.DNS_IP,        &IoTpAd.flash_rw_buf[FLASH_COM_CFG_DNS_SERVER_IP],  FLASH_COM_CFG_DNS_SERVER_IP_LEN);
        memcpy(IoTpAd.ComCfg.GateWay_IP,    &IoTpAd.flash_rw_buf[FLASH_COM_CFG_GATEWAY_IP],     FLASH_COM_CFG_GATEWAY_IP_LEN);
        memcpy(IoTpAd.ComCfg.IoT_ServeIP,    &IoTpAd.flash_rw_buf[FLASH_COM_CFG_IOT_SERVER_IP],     FLASH_COM_CFG_IOT_SERVER_IP_LEN);

        if ( ((IoTpAd.ComCfg.IoT_TCP_Srv_Port == 0) || (IoTpAd.ComCfg.IoT_TCP_Srv_Port == 0xFFFF)) ||
             ((IoTpAd.ComCfg.Local_TCP_Srv_Port == 0) || (IoTpAd.ComCfg.Local_TCP_Srv_Port == 0xFFFF)) ||
             ((IoTpAd.ComCfg.IoT_UDP_Srv_Port == 0) || (IoTpAd.ComCfg.IoT_UDP_Srv_Port == 0xFFFF)) ||
             ((IoTpAd.ComCfg.Local_UDP_Srv_Port == 0) || (IoTpAd.ComCfg.Local_UDP_Srv_Port == 0xFFFF)) ||
             (check_data_valid(IoTpAd.ComCfg.STATIC_IP,    MAC_IP_LEN) == FALSE) ||
             (check_data_valid(IoTpAd.ComCfg.SubnetMask_IP,MAC_IP_LEN) == FALSE) ||
             (check_data_valid(IoTpAd.ComCfg.DNS_IP,       MAC_IP_LEN) == FALSE) ||
             (check_data_valid(IoTpAd.ComCfg.GateWay_IP,   MAC_IP_LEN) == FALSE) ||
             (check_data_valid(IoTpAd.ComCfg.IoT_ServeIP,  MAC_IP_LEN) == FALSE) ) {
            default_ip_cfg();
        }

        memcpy(IoTpAd.ComCfg.CmdPWD, &IoTpAd.flash_rw_buf[FLASH_COM_CFG_CMD_PWD],  FLASH_COM_CFG_CMD_PWD_LEN);
    } else {
        reset_com_cfg(TRUE);
    }
#else
    default_boot_cfg();
    default_uart_cfg();
    default_ip_cfg();
#endif

    return TRUE;
}

/*========================================================================
    Routine    Description:
        reset_com_cfg

    Arguments:
    Return Value:
========================================================================*/
bool reset_com_cfg(bool bUpFlash)
{
    //printf_high("%s \n",__FUNCTION__);

    /* Notice: erase Common config Flash region , default size is one sector [4KB] */
    spi_flash_erase_sector(FLASH_COM_CFG_BASE);

    memset(IoTpAd.flash_rw_buf ,0xff, sizeof(IoTpAd.flash_rw_buf));

    default_boot_cfg();
    default_uart_cfg();
    default_ip_cfg();

    if (bUpFlash == TRUE) {
        IoTpAd.flash_rw_buf[FLASH_COM_CFG_INFO_STORED]    = COMMON_INFO_STORED;
        IoTpAd.flash_rw_buf[FLASH_COM_CFG_BOOT_IDX]       = Com_Cfg.BootFWIdx;
        IoTpAd.flash_rw_buf[FLASH_COM_CFG_RECOVERY_MODE_STATUS] = Com_Cfg.RecovModeStatus;
        IoTpAd.flash_rw_buf[FLASH_COM_CFG_IO_SET]         = Com_Cfg.IOMode;

        memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_BAUD],&Com_Cfg.UART_Baudrate, FLASH_COM_CFG_UART_BAUD_LEN);
        IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_DATA_BITS]   = Com_Cfg.UART_DataBits;
        IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_PARITY]        = Com_Cfg.UART_Parity;
        IoTpAd.flash_rw_buf[FLASH_COM_CFG_UART_STOP_BITS]   = Com_Cfg.UART_StopBits;

        IoTpAd.flash_rw_buf[FLASH_COM_CFG_TCPUDP_CS_SET]    = Com_Cfg.TCPUDP_CS;
        memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_IOT_TCP_SRV_PORT], &Com_Cfg.IoT_TCP_Srv_Port,    FLASH_COM_CFG_IOT_TCP_SRV_PORT_LEN);
        memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_LOCAL_TCP_SRV_PORT], &Com_Cfg.Local_TCP_Srv_Port,    FLASH_COM_CFG_LOCAL_TCP_SRV_PORT_LEN);
        memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_IOT_UDP_SRV_PORT], &Com_Cfg.IoT_UDP_Srv_Port,    FLASH_COM_CFG_IOT_UDP_SRV_PORT_LEN);
        memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_LOCAL_UDP_SRV_PORT], &Com_Cfg.Local_UDP_Srv_Port,    FLASH_COM_CFG_LOCAL_UDP_SRV_PORT_LEN);
        IoTpAd.flash_rw_buf[FLASH_COM_CFG_USE_DHCP]          = Com_Cfg.Use_DHCP;

        memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_STATIC_IP],     Com_Cfg.STATIC_IP,     MAC_IP_LEN);
        memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_SUBNET_MASK_IP],Com_Cfg.SubnetMask_IP, MAC_IP_LEN);
        memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_DNS_SERVER_IP], Com_Cfg.DNS_IP,         MAC_IP_LEN);
        memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_GATEWAY_IP],    Com_Cfg.GateWay_IP,     MAC_IP_LEN);
        memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_IOT_SERVER_IP], Com_Cfg.IoT_ServeIP,     MAC_IP_LEN);

        memcpy(&IoTpAd.flash_rw_buf[FLASH_COM_CFG_CMD_PWD],        Com_Cfg.CmdPWD ,         PASSWORD_MAX_LEN);

        spi_flash_write(FLASH_COM_CFG_BASE, IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));

    }

    return TRUE;
}

/*========================================================================
    Routine    Description:
        reset_sta_cfg

    Arguments:
    Return Value:
========================================================================*/
bool reset_sta_cfg(void)
{
    /* Notice: erase sta config Flash region , default size is one sector [4KB] */
    spi_flash_erase_sector(FLASH_STA_CFG_BASE);

    return TRUE;
}

/*========================================================================
    Routine    Description:
        reset_ap_cfg

    Arguments:
    Return Value:
========================================================================*/
bool reset_ap_cfg(void)
{
    /* Notice: erase ap config Flash region , default size is one sector [4KB] */
    spi_flash_erase_sector(FLASH_AP_CFG_BASE);

    return TRUE;
}
/*========================================================================
    Routine    Description:
        iot_cust_gpiint_hdlr --  GPIO interrupt handler

    Arguments:
    [IN] GPI_STS  -  [bit0:bit4]:  GPIO0~4 Interrupt status

    Return Value:  NONE
    Note:
========================================================================*/
void iot_cust_gpiint_hdlr(IN uint8 GPI_STS)
{
    if ((GPI_STS >> 0) & 0x01) {
        printf_high("GPIO_0 interrupted\n");
    } else if ((GPI_STS >> 1) & 0x01) {
        printf_high("GPIO_1 interrupted\n");
    } else if ((GPI_STS >> 2) & 0x01) {
        printf_high("GPIO_2 interrupted\n");
    } else if ((GPI_STS >> 3) & 0x01) {
        printf_high("GPIO_3 interrupted\n");
    } else if ((GPI_STS >> 4) & 0x01) {
        printf_high("GPIO_4 interrupted\n");
    } else if ((GPI_STS >> 5) & 0x01) {
        printf_high("GPIO_5 interrupted\n");
    } else if ((GPI_STS >> 6) & 0x01) {
        printf_high("GPIO_6 interrupted\n");
    } else {
        printf_high("Ignored\n");
    }
}


#if (HW_TIMER1_SUPPORT==1)
void iot_cust_hwtimer1_hdlr(void)
{
    /*Sample code for HW timer1 interrupt handle*/
    /*Notice:  Do not implement too much process here, as it is running in interrupt level*/

#if 0
    uint8 input, Polarity;

    /*Make GPIO 4 blinking by Timer1 HW EINT */
    iot_gpio_read(4, &input, &Polarity);
    IoT_gpio_output(4, (input==0)?1:0 );
#endif
}

uint32 iot_cust_hwtimer1_tick(void)
{
    return TICK_HZ_HWTIMER1;
}
#endif

#if (IOT_PWM_SUPPORT == 1)
uint16 iot_cust_pwm_def_resolution(void)
{
    /*The range of PWM resolution*/
    return PWM_HIGHEST_LEVEL;
}
#endif

