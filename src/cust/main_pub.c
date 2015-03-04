/******************************************************************************
 * MODULE NAME:     main.c
 * PROJECT CODE:    __MT7681__
 * DESCRIPTION:
 * DESIGNER:        Charles Su
 * DATE:            Oct 2014
 *
 * SOURCE CONTROL:
 *
 * LICENSE:
 *     This source code is copyright (c) 2014 Mediatek. Inc.
 *     All rights reserved.
 *
 * REVISION     HISTORY:
 *   V1.0.0     Oct 2014    - Initial Version V1.0
 *   V1.0.1     Dec 2014    - Add Caution
 *
 *
 * SOURCE:
 * ISSUES:
 *    First Implementation.
 * NOTES TO USERS:
 *
 ******************************************************************************/
#include "stdio.h"
#include "types.h"
#include "wifi_task.h"
#include "iot_api.h"
#include "iot_custom.h"
#include "bsp.h"
#include "app.h"
#include "initial.h"
#ifdef CONFIG_SOFTAP
#include "ap_pub.h"
#endif
#include "tcpip_main.h"

/******************************************************************************
  Extern Paramter
******************************************************************************/
uint32 __OS_Heap_Start;
extern IOT_CUST_OP IoTCustOp;
extern unsigned long _BSS_END;
#ifdef CONFIG_SOFTAP
extern AP_ADMIN_CONFIG  *pIoTApCfg;
#endif

/******************************************************************************
  Functions
******************************************************************************/
/******************************************************************************
 * main
 * Description :
 *  The main function is used to initialize all the functionalities in C.
 ******************************************************************************/

extern IOT_ADAPTER  IoTpAd;
extern IOT_COM_CFG Com_Cfg;
extern IOT_USR_CFG Usr_Cfg;

OUT int main (void)
{
    int ret = 0;
    
    /* Initialize BSP */
    __OS_Heap_Start = (uint32)&_BSS_END;

    /*mode=0, spi clk=10MHz.   mode=1,spi_clk=20MHz*/
    spi_flash_clk_set(0);

    /* customer hook function initial */
    iot_cust_ops();

    pre_init_cfg();

    bsp_init();

    iot_sys_init();

#if CFG_SUPPORT_TCPIP
    tcpip_init();
#endif

    cust_app_init();

#ifdef CONFIG_SOFTAP
    iot_ap_init();

    iot_ap_startup();
#endif

    /* Create Tasks */
    /*******************************************************************************
     ****** CAUTION : SOFTWARE PROGRAMMER SHALL NOT MODIFY THE FOLLOWING CODES *****
     ******************************************************************************/
    sysTASK_RegTask(wifiTASK_LowPrioTask);

    /* Start the Kernel process */
    sysKernelStart();

    return ret;
}

