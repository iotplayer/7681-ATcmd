#ifndef __WIFI_TASK_H__
#define __WIFI_TASK_H__
#include "xip_ovly.h"
/******************************************************************************
 * MODULE NAME:     wifi_task.h
 * PROJECT CODE:    __MT7681__
 * DESCRIPTION:
 * DESIGNER:        ShangWei Huang
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
 *
 *
 * SOURCE:
 * ISSUES:
 *    First Implementation.
 * NOTES TO USERS:
 *
 ******************************************************************************/


/******************************************************************************
 * PROTOTYPE
 ******************************************************************************/
void wifiTASK_LowPrioTask (void);
void wifi_rx_proc(void);
void wifi_state_machine(void);
void cali_period_exec(void);

void ws_goto_smnt(void)                        XIP_ATTRIBUTE(".xipsec0");
void wifi_state_chg(uint8 iStateMachine, uint8 iSubState)  XIP_ATTRIBUTE(".xipsec0");
void ws_init(OUT bool *pb_enable)        XIP_ATTRIBUTE(".xipsec0");
void ws_smt_conn(OUT bool *pb_enable)    XIP_ATTRIBUTE(".xipsec0");
void ws_scan(OUT bool *pb_enable)        XIP_ATTRIBUTE(".xipsec0");
void ws_auth(OUT bool *pb_enable)        XIP_ATTRIBUTE(".xipsec0");
void ws_assoc(OUT bool *pb_enable)       XIP_ATTRIBUTE(".xipsec0");

void ws_4way(OUT bool *pb_enable);
void ws_connected(OUT bool *pb_enable);
void setFlagVfyInit(bool flag);

#endif /* __WIFI_TASK_H__ */

