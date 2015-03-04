/******************************************************************************
 * MODULE NAME:     ch_switch_pub.c
 * PROJECT CODE:    __MT7681__
 * DESCRIPTION:
 * DESIGNER:        Jinchuan
 * DATE:            April 2014
 *
 * SOURCE CONTROL:
 *
 * LICENSE:
 *     This source code is copyright (c) 2014 mediatek Tech. Inc.
 *     All rights reserved.
 *
 * REVISION     HISTORY:
 *   V1.0.0       April 2014   - Initial Version V1.0
 *
 *
 * SOURCE:
 * ISSUES:
 *    First Implementation.
 * NOTES TO USERS:
 *
 ******************************************************************************/
#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include "iot_api.h"

#ifdef __MT7681

#ifdef CONFIG_STATION
extern STA_ADMIN_CONFIG *pIoTStaCfg;
#endif

/*========================================================================
    Routine    Description:
        asic_set_channel --  switch channel , bandwidth and  40MHz above/below setting
    Arguments:
        ch    :     Channel number,     if 'bw' is BW_40,  'ch' is Center channel   [1~14]
        bw   :     Bandwidth    [BW_20, BW40]
        ext_ch:  11n bandwidth setting  [EXTCHA_NONE,  EXTCHA_ABOVE,   EXTCHA_BELOW]
                     if 'bw'=BW_20,  'ext_ch' should be EXTCHA_NONE  or EXTCHA_ABOVE, these two options have the same effect for BW_20.
                  if 'bw'=BW_20,  'ext_ch' should not be EXTCHA_BELOW
    Return Value: 0 is success
    Note:
========================================================================*/
int32 asic_set_channel(uint8 ch, uint8 bw, uint8 ext_ch)
{
    bool bScan =  FALSE;

    rtmp_bbp_set_bw(bw);

    /*  Tx/RX : control channel setting */
    /*set band as 40MHz above or Below*/
    rtmp_bbp_set_ctrlch(ext_ch);
    rtmp_mac_set_ctrlch(ext_ch);

    /* Let BBP register at 20MHz to do scan */
    AsicSwitchChannel(ch, bScan);

    return 0;
}


/*========================================================================
    Routine    Description:
        iot_atcmd_set_channel --  switch channel

    Arguments:
    Return Value: 0 is success
    Note:  In present , default is switch to channel 1~14, when Bandwidth 20MHz.
              if BW_40,   the ext_ch should be adjusted while central channel >= 12, this is reserved for customization
========================================================================*/
void iot_atcmd_set_channel(uint8 Channel)
{
#ifdef CONFIG_SOFTAP
    asic_set_channel(Channel, BW_20, EXTCHA_NONE);
#else
    asic_set_channel(Channel, pIoTStaCfg->Cfg_BW, EXTCHA_ABOVE);
#endif

    return;
}

#endif // __MT7681
