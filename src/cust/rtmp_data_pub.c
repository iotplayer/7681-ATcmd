/*
 ***************************************************************************
 * Mediatek.
 * 4F, No. 2 Technology 5th Rd.
 * Science-based Industrial Park
 * Hsin-chu, Taiwan, R.O.C.
 *
 * (c) Copyright 2002-2014 Mediatek, Inc.
 *
 * All rights reserved. Mediatek's source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of Mediatek. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of Mediatek, Inc. is obtained.
 ***************************************************************************

    Module Name:
    rtmp_data.c

    Abstract:
    Data path subroutines

    Revision History:
    Who         When            What
    --------    ----------        ----------------------------------------------
*/
#include <stdio.h>
#include "types.h"
#include "config.h"
#include "queue.h"
#include "tcpip_main.h"
#include "iot_api.h"
#if (ATCMD_ATE_SUPPORT == 1)
#include "ate.h"
#endif
#ifdef CONFIG_SOFTAP
#include "ap_pub.h"
#include "ap_auth.h"
#endif
#include "wpa_cmm.h"
#include "wifi_task.h"

/*****************************************************************
  Defination
******************************************************************/
#if (IOT_SMP_RSSI_COLLECT == 1)
RX_RSSI_INFO gRxRSSIInfo;
void iot_rssi_sample(IN RX_RSSI_INFO *pRssi, IN RXWI_STRUC *pRxWI);
#endif

/*****************************************************************
  Extern Paramter
******************************************************************/
extern MLME_STRUCT      *pIoTMlme;
extern uint8 EAPOL[2];    //  = {0x88, 0x8e};
extern u8_t uip_sourceAddr[6];

#ifdef CONFIG_SOFTAP
extern uint8 NUM_BIT8[8];
extern AP_ADMIN_CONFIG  *pIoTApCfg;
#else
extern STA_ADMIN_CONFIG  *pIoTStaCfg;
#endif

#if (ATCMD_ATE_SUPPORT == 1)
extern ATE_INFO gATEInfo;
#endif

extern uint16 gSequence;
extern uint8  SNAP_802_1H[];
extern uint8  SNAP_BRIDGE_TUNNEL[];
extern volatile uint8 g_tx_pkt_id;

/*****************************************************************
  Extern Function
******************************************************************/
void STAHandleRxMgmtFrame(RX_BLK * pRxBlk);


/*****************************************************************
  Functions
******************************************************************/
void RestartBCNTimer(void)
{
    cnmTimerStopTimer (&pIoTMlme->BeaconTimer);
    cnmTimerStartTimer(&pIoTMlme->BeaconTimer, BEACON_LOST_TIME);
}

/*
    ==========================================================================
    Description:
        function to be executed at timer thread when beacon lost timer expires
    IRQL = DISPATCH_LEVEL
    ==========================================================================
 */
/* Beacon loss timeout handler */
void iot_bcn_timeout_action(uint32 param, uint32 param2)
{
    iot_linkdown(REASON_DISASSPC_AP_UNABLE);
}

#ifdef CONFIG_SOFTAP
void APHandleRxControlFrame(
    IN    RX_BLK            *pRxBlk)
{
    PHEADER_802_11    pHeader = pRxBlk->pHeader;
    PMAC_TABLE_ENTRY  pEntry;

    pEntry = MacTableLookup(pRxBlk->pHeader->Addr2);
    if (pEntry == NULL)
        return;

    switch (pHeader->FC.SubType) {
            /* handle PS-POLL here */
        case SUBTYPE_PS_POLL: {
            uint16 Aid = pHeader->Duration & 0x3fff;

            if (pEntry->Aid != Aid)
                return;        /**/

#if 0
            if (Aid < MAX_LEN_OF_MAC_TABLE)
                APHandleRxPsPoll(pAd, pHeader->Addr2, Aid, FALSE);
#endif

#if 0    /*temprary cancel response NULL, in order to let station keep awake*/
            APSendNullFrame(pEntry->Addr, PWR_ACTIVE);
#endif
            pEntry->NoDataIdleCount = 0;

#ifndef IOT_BCN_TIM_KEEP_ON
            WLAN_MR_TIM_BIT_CLEAR(Aid);
#endif
        }
        break;
        case SUBTYPE_ACK:
        default:
            break;
    }

    return;
}


// All Rx routines use RX_BLK structure to hande rx events
// It is very important to build pRxBlk attributes
//  1. pHeader pointer to 802.11 Header
//  2. pData pointer to payload including LLC (just skip Header)
//  3. set payload size including LLC to DataSize
//  4. set some flags with RX_BLK_SET_FLAG()
void APHandleRxDataFrame(
    RX_BLK*            pRxBlk)
{
    PMAC_TABLE_ENTRY  pEntry = NULL;

    /*Only Handle the MyBSS with ToDS flag enable packets  for packet forwarding to the destination address*/
    if ((pRxBlk->pRxINFO->MyBss == 1) &&
        (pRxBlk->pHeader->FC.ToDs == 1) && (pRxBlk->pHeader->FC.FrDs == 0)) {
        /*if DestAddr(add3) is not same as BSSID MACAddr(add1), forwarding the Rx frame to DestAddr*/
        if (memcmp(pRxBlk->pHeader->Addr1, pRxBlk->pHeader->Addr3, MAC_ADDR_LEN))  {
            /*forward packets*/
            AP_Legacy_Frame_Forward_Tx(pRxBlk, get_clear_frame_flag());

            /*Drop the packets which DA is not Broadcast/Multicast/BSSID address, after packet forwarding*/
            if ((pRxBlk->pHeader->Addr3[0] & 0x1) == 0) {
                printf("Drop packets after forwarding: 0x%x,0x%x,0x%x,0x%x,0x%x,0x%x \n",
                       PRINT_MAC(pRxBlk->pHeader->Addr3));
                return;
            }
        }
    }

    //skip 802.11 and SNAP header
    RTMPRawDataSanity(pRxBlk);

#if CFG_SUPPORT_4WAY_HS
    //check is EAPOL package
    if (!memcmp(pRxBlk->pData, EAPOL, 2)) {
        /* Confirm incoming is EAPOL-KEY frame */
        if ( *(pRxBlk->pData + 3) == EAPOLKey) {
            printf("EAPOL KEY +++\n");

            /*pData include 2Byte Protocol type(0x888E) of 802.2 LLC header, so delete Protocol type  */
            /*and only transfer Eapol data and len to  WpaEAPOLKeyAction()*/
            pRxBlk->pData      += 2;
            pRxBlk->DataSize  -= 2;

            /* process EAPOL KEY 4-way handshake and 2-way group handshake */
            WpaEAPOLKeyAction(pRxBlk);
        }
    } else
#endif
    {
        /* some AP will sent Broadcast/Muliticast Data frame */
        pEntry = MacTableLookup(pRxBlk->pHeader->Addr2);
        if (pEntry == NULL)  {
            /*Assume RX filter drop the notMyBssid Frame*/
            ApSendDeauthAction(pRxBlk->pHeader->Addr2, REASON_CLS3ERR);
            return;
        }

        pEntry->NoDataIdleCount = 0;

#ifndef IOT_BCN_TIM_KEEP_ON
        WLAN_MR_TIM_BIT_CLEAR(pEntry->Aid);
#endif

        if (pEntry->PortSecured == WPA_802_1X_PORT_SECURED)  {
            pRxBlk->pData    -= 6;
            memcpy(pRxBlk->pData, pRxBlk->pHeader->Addr3, 6);
            pRxBlk->DataSize += 6;

            pRxBlk->pData    -= 6;
            memcpy(pRxBlk->pData, pRxBlk->pHeader->Addr1, 6);
            pRxBlk->DataSize += 6;

#if CFG_SUPPORT_TCPIP
            netif_rx(pRxBlk->pData, pRxBlk->DataSize);
#endif
        }
    }

    return;
}

void APHandleRxMgmtFrame(
    RX_BLK*            pRxBlk)
{
    switch (pRxBlk->pHeader->FC.SubType) {
        case SUBTYPE_PROBE_REQ:
            APSendProbeAction(pRxBlk);
            break;
        case SUBTYPE_AUTH:
            APPeerAuthReqAction(pRxBlk);
            printf_high("%s,%d, SUBTYPE_AUTH: \n",__FUNCTION__,__LINE__);
            break;
        case SUBTYPE_DEAUTH:
            APPeerDeauthReqAction(pRxBlk);
            printf_high("%s,%d, SUBTYPE_DEAUTH: \n",__FUNCTION__,__LINE__);
            break;
        case SUBTYPE_ASSOC_REQ:
            APAssocReqActionAndSend(pRxBlk, 0);
            printf_high("%s,%d, SUBTYPE_ASSOC_REQ: \n",__FUNCTION__,__LINE__);
            break;
        case SUBTYPE_REASSOC_REQ:
            APAssocReqActionAndSend(pRxBlk, 1);
            printf_high("%s,%d, SUBTYPE_REASSOC_REQ: \n",__FUNCTION__,__LINE__);
            break;
        case SUBTYPE_DISASSOC:
            APPeerDisassocReqAction(pRxBlk);
            printf_high("%s,%d, SUBTYPE_DISASSOC: \n",__FUNCTION__,__LINE__);
            break;
        default :
            break;
    }
}
#endif

#ifdef CONFIG_STATION
// All Rx routines use RX_BLK structure to hande rx events
// It is very important to build pRxBlk attributes
//  1. pHeader pointer to 802.11 Header
//  2. pData pointer to payload including LLC (just skip Header)
//  3. set payload size including LLC to DataSize
//  4. set some flags with RX_BLK_SET_FLAG()
void STAHandleRxDataFrame(
    RX_BLK*            pRxBlk)
{
#if (CFG_SUPPORT_TCPIP == 0)
    puchar rawpacket;
    uint16 rawpacketlength;
#endif

    /*************************************************
    *data frame parsing only in SMTCN and CONNECTED
    **************************************************/
#if (ATCMD_ATE_SUPPORT == 1)
    /*Do nothing*/

#else
    switch (pIoTMlme->CurrentWifiState) {
        case WIFI_STATE_SMTCNT:
#if (CFG_SUPPORT_MTK_SMNT == 1)
            /*implement MTK smart connection if there is no customer callback for smart connection*/
            SMTCNRXFrameHandle(pRxBlk->pData, pRxBlk->pRxWI->MPDUtotalByteCount);
#else
    {
        //customer smart connect process
        //collect smnt connection packet
    }
#endif
            break;
        case WIFI_STATE_4WAY:
        case WIFI_STATE_CONNED:
            //skip 802.11 and SNAP header
            RTMPRawDataSanity(pRxBlk);
#if CFG_SUPPORT_4WAY_HS
            //check is EAPOL package
            if (!memcmp(pRxBlk->pData, EAPOL, 2))  {
                /* Confirm incoming is EAPOL-KEY frame */
                if ( *(pRxBlk->pData + 3) == EAPOLKey)  {
                    printf("EAPOL KEY +++\n");

                    /*pData include 2Byte Protocol type(0x888E) of 802.2 LLC header */
                    /*Only transfer Eapol data and len to  WpaEAPOLKeyAction()*/
                    pRxBlk->pData      += 2;
                    pRxBlk->DataSize  -= 2;

                    /* process EAPOL KEY 4-way handshake and 2-way group handshake */
                    WpaEAPOLKeyAction(pRxBlk);
                }
                break;
            }
#endif
            /*
             * 2014/05/23,terrence,MT7681 STA power saving mode
             * here,we check the received UC/BMC packet which parepared for power saving
             */
            if (pIoTMlme->CurrentWifiState == WIFI_STATE_CONNED)  {
#if (MT7681_POWER_SAVING == 1)
                STACheckRxDataForPS(pRxBlk);
#endif
#if CFG_SUPPORT_TCPIP
                pRxBlk->pData    -= 6;
                memcpy(pRxBlk->pData, pRxBlk->pHeader->Addr3, 6);
                pRxBlk->DataSize += 6;

                pRxBlk->pData    -= 6;
                memcpy(pRxBlk->pData, pRxBlk->pHeader->Addr1, 6);
                pRxBlk->DataSize += 6;

                netif_rx(pRxBlk->pData, pRxBlk->DataSize);
#else
    if (pRxBlk->pRxINFO->Bcast) {
        rawpacket = ParsingBCData(pRxBlk, &rawpacketlength);
        if (rawpacket != NULL)  {
            iot_app_proc_pkt(rawpacket, rawpacketlength);
        }
    }
#endif
            }
            break;
        default:
            break;
    }
#endif

    return;
}

void STAHandleRxMgmtFrame(
    RX_BLK*            pRxBlk)
{
#if (ATCMD_ATE_SUPPORT == 1)
    /*Do nothing*/

#else
    switch (pIoTMlme->CurrentWifiState) {
            /*Scan SM*/
        case WIFI_STATE_SCAN:
            ScanProcessMgmt(pRxBlk);
            break;
            /*Auth SM*/
        case WIFI_STATE_AUTH:
            MlmeAuthProcess(pRxBlk);
            break;
            /*Assoc SM*/
        case WIFI_STATE_ASSOC:
            MlmeAssocProcess(pRxBlk);
            break;
            /*Connected SM*/
        case WIFI_STATE_CONNED:
            if (pRxBlk->pRxINFO->U2M) {
                if (pRxBlk->pHeader->FC.SubType == SUBTYPE_DEAUTH) {
                    //keep awake
                    //printf("PeerDeauthAction2\n");
                    wifi_state_chg(WIFI_STATE_INIT, 0);
                }
                if (pRxBlk->pHeader->FC.SubType == SUBTYPE_DISASSOC) {
                    //keep awake
                    //printf("PeerDISASSOCAction2\n");
                    PeerDisassocAction(pRxBlk,
                                       pRxBlk->pData,
                                       pRxBlk->DataSize,
                                       pRxBlk->pHeader->FC.SubType);
                }
            }

            if (pRxBlk->pRxINFO->MyBss) {
                if (pRxBlk->pHeader->FC.SubType == SUBTYPE_BEACON) {
                    //printf("beacon received,time:%d\n",kal_get_systime());
                    /* Go initial state if the PrimaryCH information in receiving Beacon is not equal to current primary channel*/
                    if (PeerBeaconCHInfoParse(pRxBlk)) {
                        RestartBCNTimer();
                    } else {
                        iot_bcn_timeout_action(0,0);
                    }

                    /*
                    * 2014/05/23,terrence,MT7681 STA power saving mode
                    * here,we check the received beacon packet which parepared for power saving
                    */
#if (MT7681_POWER_SAVING == 1)
                    STACheckRxBeaconForPS(pRxBlk);
#endif
                }
            }
            break;
        default:
            break;
    }
#endif
}

#endif /*CONFIG_STATION*/

/*
    ========================================================================

    Routine Description:
        Process RxDone interrupt, running in DPC level

    Arguments:
        pAd Pointer to our adapter

    Return Value:
        None

    IRQL = DISPATCH_LEVEL

    Note:
        This routine has to maintain Rx ring read pointer.
        Need to consider QOS DATA format when converting to 802.3
    ========================================================================
*/
bool WifiRxDoneInterruptHandle (
    pBD_t pBufDesc)
{
    PHEADER_802_11  pHeader;
    RX_BLK                RxCell;
    PRXWI_STRUC      pRxWI;
    PRXINFO_STRUC   pRxINFO;
    puchar                pBuff;

#ifdef CONFIG_SOFTAP
    /* NULL packet(length=52) , PS-Poll Packet(Length=44) are the normal packets in AP mode*/
    if (pBufDesc->Length < sizeof(HEADER_802_11) + RXINFO_SIZE + RXWI_SIZE)  {
        if (pBufDesc->Length != (16 + RXINFO_SIZE + RXWI_SIZE))  {
            /*not  NULL  or PS-Poll  Packet,  and invalid packet length */
            //printf_high("Err len=%d\n", pBufDesc->Length);
            return FALSE;
        }
        /*PS-Poll Packet should be deal with in APHandleRxControlFrame()*/
    }
#else
    if (pBufDesc->Length <= sizeof(HEADER_802_11) +RXINFO_SIZE +RXWI_SIZE) {
        // invalid packet length
        //printf_high("Err len=%d\n", pBufDesc->Length);
        return FALSE;
    }
#endif

    /* RX_FCEINFO  */
    /* RXINFO      */
    /* RXWI        */
    /* Payload     */

    /* check MIC error by pRxD attribute */
    /* single frame MIC is calculated by HW */
    pBuff      = (puchar)pBufDesc->pBuf;
    pRxINFO = (PRXINFO_STRUC)(pBuff);
    pRxWI    = (PRXWI_STRUC) (pBuff +RXINFO_SIZE);
    pHeader  = (PHEADER_802_11) (pBuff +RXINFO_SIZE +RXWI_SIZE);
    store_phymode(pRxWI->PHYMode);

    /*move RTMPCheckRxError() into WifiRxFsIntFilterOut() which be implemented in ISR */

    /* fill RxBLK */
    RxCell.pRxINFO = pRxINFO;
    RxCell.pRxWI   = pRxWI;
    RxCell.pHeader = pHeader;
    RxCell.pRxPacket = pBufDesc;
    RxCell.pData = (uint8 *) pHeader;
    RxCell.DataSize = pRxWI->MPDUtotalByteCount;
    RxCell.Flags = 0;

#if (RX_CLASS123 == 1)
    if (!STARxFrameClassVfy(&RxCell, pHeader->FC.Type)) {
        return FALSE;
    }
#endif

#if (ATCMD_ATE_SUPPORT == 1)
    /* Increase Total receive byte counter after real data received no mater any error or not */
    gATEInfo.WlanCounters.ReceivedFragmentCount.QuadPart++;

    if ((RxCell.pRxINFO->Bcast) || (RxCell.pRxINFO->Mcast))  {
        gATEInfo.RxBMTotalCnt++;
        gATEInfo.OneSecCountersTmp.RxCntBM1S++;
    } else  {
        gATEInfo.RxU2MTotalCnt++;
        gATEInfo.OneSecCountersTmp.RxCntU2M1S++;
    }

    /*only summary RSSI for u2m packet*/
    //ATESampleRssi(RxCell.pRxWI);     /*The old method for ATE, no exactly*/
    Update_Rssi_Sample(&gATEInfo, RxCell.pRxWI);

    if ((pHeader->FC.Type == BTYPE_MGMT) ||
        (pHeader->FC.Type == BTYPE_CNTL)) {
        gATEInfo.RxMgmtCntlTotalCnt++;
        gATEInfo.OneSecCountersTmp.RxCntMgmtCntl1S++;
    }
#endif

#if (IOT_SMP_RSSI_COLLECT == 1)
    iot_rssi_sample(&gRxRSSIInfo, RxCell.pRxWI);
#endif

    //printf_high("%s,%d    BufDescLen(%d) , Type/SubType:(0x%x,0x%x) \n",
    //    __FUNCTION__,__LINE__,pBufDesc->Length, pHeader->FC.Type, pHeader->FC.SubType);

    switch (pHeader->FC.Type) {
            /* CASE I, receive a MANAGEMENT frame */
        case BTYPE_MGMT:
            /* process Management frame */
#ifdef CONFIG_STATION
            STAHandleRxMgmtFrame(&RxCell);
#else
            APHandleRxMgmtFrame(&RxCell);
#endif
            break;

            /* CASE II, receive a DATA frame */
        case BTYPE_DATA:
            if ((pHeader->FC.ToDs == 0) && (pHeader->FC.FrDs == 1))
                memcpy(uip_sourceAddr, pHeader->Addr3, MAC_ADDR_LEN);
#ifdef CONFIG_STATION
            //Only deal with Data frame in scan or Connected State, otherwise we need response Deauth
            if (pIoTMlme->DataEnable == 1)
                STAHandleRxDataFrame(&RxCell);
#else
            APHandleRxDataFrame(&RxCell);
#endif
            break;

            /* CASE III, receive a CONTROL frame */
        case BTYPE_CNTL:
#ifdef CONFIG_SOFTAP
            APHandleRxControlFrame(&RxCell);
#endif
            break;

        default:
            /* free control */
            break;
    }

    return TRUE;
}

/*
 * ISR context
 * SW Filter to exclude unexpected packets
 * be carefully,that API is called in FCE RX ISR handler to filter out packet
 * TRUE:  discard the packet directly in FCE RX interrupt handler
 * FALSE: allow to receive the packet in FCE RX interrupt handler
 */
bool WifiRxFsIntFilterOut(pBD_t RxpBufDesc)
{
    puchar          pBuff;
    PRXINFO_STRUC   pRxINFO;
    PRXWI_STRUC     pRxWI;
    PHEADER_802_11  pHeader;
#if (MT7681_POWER_SAVING == 1)
    RX_BLK            RxBlk;
#endif
    uint8           type;
    uint8           subtype;

    pBuff       = (puchar)RxpBufDesc->pBuf;
    pRxINFO  = (PRXINFO_STRUC)(pBuff);
    pRxWI     = (PRXWI_STRUC)(pBuff + RXINFO_SIZE);
    pHeader   = (PHEADER_802_11)(pBuff + RXINFO_SIZE + RXWI_SIZE);
    type         = pHeader->FC.Type;
    subtype    = pHeader->FC.SubType;

    if (RTMPCheckRxError(pHeader, pRxWI, pRxINFO) == NDIS_STATUS_FAILURE) {
        return TRUE;  /* free packet */
    }

#ifdef CONFIG_STATION
    switch  (pIoTMlme->CurrentWifiState) {
        case WIFI_STATE_INIT:
        case WIFI_STATE_SMTCNT:
            if (type == BTYPE_DATA)  {
                if ((subtype == SUBTYPE_NULL_FUNC) || (subtype == SUBTYPE_QOS_NULL))
                    return TRUE;
            }
#if (CFG_SUPPORT_MTK_SMNT == 1)
            else if (type == BTYPE_MGMT) {
                if (subtype == SUBTYPE_BEACON)
                    return TRUE;
            }
#endif
            break;
        case WIFI_STATE_CONNED:
            /*The Mcast, Bcast bits shall be set as 1, if Received Packet is Broadcast  */
            /*The Mcast shall be set as 1, Bcast bit shall be set as 0, if Received Packet is Multicast  */
            if ((pRxINFO->Mcast) && (pRxINFO->Bcast == 0)) {
                /*
                 * 2014/05/23,terrence,MT7681 STA power saving mode
                 * here,we check the received UC/BMC packet which parepared for power saving
                 */
#if (MT7681_POWER_SAVING == 1)
                /* fill RxBLK */
                RxBlk.pRxINFO = pRxINFO;
                RxBlk.pRxWI   = pRxWI;
                RxBlk.pHeader = pHeader;
                RxBlk.pRxPacket = RxpBufDesc;
                RxBlk.pData = (uint8 *) pHeader;
                //RxBlk.DataSize = pRxWI->MPDUtotalByteCount;
                //RxBlk.Flags = 0;

                STACheckRxDataForPS(&RxBlk);
#endif
                /*Improve Rx Performance by drop Multicast frame while MT7681 connected with AP router */
                return TRUE;
            }
            break;
        default:
            break;
    }
#endif

#ifdef CONFIG_SOFTAP
    if (type == BTYPE_MGMT)  {
        /*There are too many Beacons from other AP routers, or management frame not send to MyBSS */
        /*Drop notMyBss management frame, except ProbeReq for response Active scan from STA*/
        if ((pRxINFO->MyBss == 0) && (subtype != SUBTYPE_PROBE_REQ))
            return TRUE;
    }
#endif

    return FALSE;
}

#if (IOT_SMP_RSSI_COLLECT == 1)
void iot_rssi_sample(
    IN RX_RSSI_INFO *pRssi,
    IN RXWI_STRUC *pRxWI)
{
    int8 rssi[3];
    bool bInitial = FALSE;

    if (!(pRssi->AvgRssi0 | pRssi->AvgRssi0X8 | pRssi->LastRssi0)) {
        bInitial = TRUE;
    }

    if (pRxWI->SNR2 != 0) {
        pRssi->LastRssi0 = ConvertToRssi((int8)(pRxWI->SNR2), RSSI_0, pRxWI->SNR1, pRxWI->bw);
        if (bInitial)  {
            pRssi->AvgRssi0X8 = pRssi->LastRssi0 << 3;
            pRssi->AvgRssi0  = pRssi->LastRssi0;
        } else {
            pRssi->AvgRssi0X8 = (pRssi->AvgRssi0X8 - pRssi->AvgRssi0) + pRssi->LastRssi0;
        }
        pRssi->AvgRssi0 = pRssi->AvgRssi0X8 >> 3;
    }
}
#endif

#if 1 //jinchuan 20141124 move to rtmp_data_pub.c
/*
    ========================================================================
    Routine    Description:
        Process the rx data, skip the 802.11 and snap header
    Arguments:
        pRxBlk        our adapter
    Return Value:
        NDIS_STATUS_SUCCESS        No err
        NDIS_STATUS_FAILURE        Error
    Note:
    ========================================================================
*/
NDIS_STATUS RTMPRawDataSanity(
    IN    RX_BLK* pRxBlk)
{
    if ((pRxBlk->pHeader->FC.SubType & 0x04))  { // bit 2 : no DATA
        printf("no data +++\n");
        return NDIS_STATUS_FAILURE;
    }

    pRxBlk->pData    += LENGTH_802_11;
    pRxBlk->DataSize -= LENGTH_802_11;
    //check if contains 4 mac address
    if (pRxBlk->pHeader->FC.ToDs&&pRxBlk->pHeader->FC.FrDs) {
        pRxBlk->pData    += MAC_ADDR_LEN;
        pRxBlk->DataSize -= MAC_ADDR_LEN;
    }
    if (pRxBlk->pHeader->FC.SubType & 0x08) {  //QoS data
        RX_BLK_SET_FLAG(pRxBlk, fRX_QOS);
        /* skip QOS contorl field */
        pRxBlk->pData += 2;
        pRxBlk->DataSize -=2;
    }
    if (pRxBlk->pRxINFO->HTC) {
        RX_BLK_SET_FLAG(pRxBlk, fRX_HTC);
        // skip HTC contorl field
        pRxBlk->pData += 4;
        pRxBlk->DataSize -= 4;
        printf("+++HTC \n");
    }
    if (pRxBlk->pRxINFO->L2PAD) {
        // because DataSize excluding HW padding
        RX_BLK_SET_FLAG(pRxBlk, fRX_PAD);
        pRxBlk->pData += 2;
        printf("+++Padd \n");
    }

    /* Skip LLC header */
    if (!memcmp(pRxBlk->pData,SNAP_802_1H,  SNAP_LENGTH) ||
        // Cisco 1200 AP may send packet with SNAP_BRIDGE_TUNNEL
        !memcmp(pRxBlk->pData,SNAP_BRIDGE_TUNNEL,  SNAP_LENGTH)) {
        pRxBlk->pData    += SNAP_LENGTH;
        pRxBlk->DataSize -= SNAP_LENGTH;
    } else {
        /* Drop data LLC not match.  */
        /* this is for drop data not descrypted correctly by me */
        printf("LLC not match\n");
        return NDIS_STATUS_FAILURE;
    }
    return NDIS_STATUS_FAILURE;
}

/*
    ========================================================================
    Routine    Description:
        Check Rx descriptor, return NDIS_STATUS_FAILURE if any error dound
    Arguments:
        pRxD        Pointer    to the Rx descriptor
    Return Value:
        NDIS_STATUS_SUCCESS        No err
        NDIS_STATUS_FAILURE        Error
    Note:
    ========================================================================
*/
NDIS_STATUS    RTMPCheckRxError(
    PHEADER_802_11    pHeader,
    PRXWI_STRUC    pRxWI,
    PRXINFO_STRUC    pRxINFO)
{
    uint8     bUnicast;

    if (pRxINFO == NULL) {
        printf("pRxINFO is NULL \n");
        return (NDIS_STATUS_FAILURE);
    }
    /* Phy errors & CRC errors */
    if (pRxINFO->Crc) {
        printf("+++ Crc error \n");
        return (NDIS_STATUS_FAILURE);
    }
    /* drop MIC / ICV error */
    if (pRxINFO->IcvErr) {
        DBGPRINT(RT_DEBUG_TRACE,("IcvErr \n"));
        return (NDIS_STATUS_FAILURE);
    }
    if (pRxINFO->MicErr) {
        /* key might not be stored yet */
        /* determine unicast by keyid, 0: ucast, 1-3: bcast */
        if (pRxWI->KeyIdx ==0)
            bUnicast =1;
        else
            bUnicast =0;
        DBGPRINT(RT_DEBUG_TRACE,("MicErr \n"));
        return (NDIS_STATUS_FAILURE);
    }

    return (NDIS_STATUS_SUCCESS);
}

/* --------------------------------------------------------
*  FIND ENCRYPT KEY AND DECIDE CIPHER ALGORITHM
*        Find the WPA key, either Group or Pairwise Key
*        LEAP + TKIP also use WPA key.
* Decide WEP bit and cipher suite to be used.
* Same cipher suite should be used for whole fragment burst
*
*   bClearFrame: FALSE => encryption when sending
*                TRUE  => no encryption when sending
* -------------------------------------------------------- */
void STAFindCipherAlgorithm(
    TX_BLK    *pTxBlk,
    bool    bClearFrame)
{
    uint8                            CipherAlg = CIPHER_NONE;        // cipher alogrithm
    uint8                            KeyIdx = 0xff;
    PCIPHER_KEY               pKey   = NULL;
    puchar                         pdata;
#ifdef CONFIG_SOFTAP
    PMAC_TABLE_ENTRY     pEntry;
#else
    //PSTA_ADMIN_CONFIG   pEntry;
    uint8                            TxFrameType = TX_UNKOWN_FRAME;
#endif

    pdata = (puchar)pTxBlk->pPacket->pBuf;

#ifdef CONFIG_SOFTAP
    pEntry = MacTableLookup(pdata);
#endif

    /* GTK-rekey are encryption */
    /* Probe request, 4way handshaking are no encryption */
    if (bClearFrame) {
        KeyIdx    = 0xff;
        CipherAlg = CIPHER_NONE;
    } else  {
#if CFG_SUPPORT_4WAY_HS
#ifdef CONFIG_STATION
        if (pdata[0] & 0x01)
            TxFrameType = TX_MCAST_FRAME;
        else
            TxFrameType = TX_LEGACY_FRAME;

        // 4-way handshaking frame must be clear
        if ((pIoTStaCfg->SharedKey[BSS0][0].CipherAlg) && (pIoTStaCfg->SharedKey[BSS0][0].KeyLen)) {
            KeyIdx     = 0;
            CipherAlg = pIoTStaCfg->SharedKey[BSS0][0].CipherAlg;
            if (CipherAlg)
                pKey  = &pIoTStaCfg->SharedKey[BSS0][0];
        } else {
            switch (TxFrameType)  {
                case TX_LEGACY_FRAME: // uinicast frame
                    KeyIdx    = 0;
                    CipherAlg = pIoTStaCfg->PairwiseKey.CipherAlg;
                    if (CipherAlg)
                        pKey  = &pIoTStaCfg->PairwiseKey;
                    break;
                case TX_MCAST_FRAME:  //muticast and broadcast frame
                    KeyIdx    = pIoTStaCfg->DefaultKeyId;
                    CipherAlg = pIoTStaCfg->SharedKey[BSS0][KeyIdx].CipherAlg;
                    if (CipherAlg)
                        pKey  = &pIoTStaCfg->SharedKey[BSS0][KeyIdx];
                    break;
                default:
                    KeyIdx    = 0xff;
                    CipherAlg = CIPHER_NONE;
                    break;
            }
        }
#endif /*CONFIG_STATION*/

#ifdef CONFIG_SOFTAP
        if (!pEntry ||  /* M/BCAST to local BSS, use default key in shared key table */
            (pIoTApCfg->MBSSID.WepStatus == Ndis802_11Encryption1Enabled)) { /* WEP always uses shared key table */
            KeyIdx     = pIoTApCfg->MBSSID.DefaultKeyId;
            CipherAlg = pIoTApCfg->MBSSID.SharedKey[BSS0][KeyIdx].CipherAlg;
            if (CipherAlg)
                pKey  = &pIoTApCfg->MBSSID.SharedKey[BSS0][KeyIdx];
            /* update Hardware Group Key Index */
            /* use Wcid as Hardware Key Index */
            GET_GroupKey_WCID(pTxBlk->Wcid, BSS0);
        } else {
            /* unicast to local BSS */
            KeyIdx      = 0;
            CipherAlg = pEntry->PairwiseKey.CipherAlg;
            if (CipherAlg)
                pKey  = &pEntry->PairwiseKey;
            pTxBlk->Wcid = pEntry->Aid;
        }
#endif      /*CONFIG_SOFTAP*/
#endif  /*CFG_SUPPORT_4WAY_HS*/
    }

    pTxBlk->CipherAlg = CipherAlg;
    pTxBlk->pKey = pKey;
    pTxBlk->KeyIdx = KeyIdx;

    DBGPRINT(RT_DEBUG_INFO, ("%s %d, CipherAlg:%d, keyIdx:%d,  pTxBlk->Wcid = %d\n",
                             __FUNCTION__, __LINE__,pTxBlk->CipherAlg, pTxBlk->KeyIdx, pTxBlk->Wcid));
}

void STABuildCommon802_11Header(
    TX_BLK          *pTxBlk)
{
    HEADER_802_11    *pHeader_802_11;

    /*******************************/
    /* MAKE A COMMON 802.11 HEADER */
    /*******************************/

    /* normal wlan header size : 24 octets */
    pTxBlk->MpduHeaderLen = sizeof(HEADER_802_11);

    pHeader_802_11 = (HEADER_802_11 *) &pTxBlk->HeaderBuf[TXWI_SIZE];
    NdisZeroMemory(pHeader_802_11, sizeof(HEADER_802_11));

    pHeader_802_11->FC.Type = BTYPE_DATA;
    pHeader_802_11->FC.SubType = SUBTYPE_DATA;
#ifdef CONFIG_STATION
    pHeader_802_11->FC.FrDs = 0;
    pHeader_802_11->FC.ToDs = 1;
#else
    pHeader_802_11->FC.FrDs = 1;
    pHeader_802_11->FC.ToDs = 0;
#endif
    pHeader_802_11->Sequence = gSequence++;
    pHeader_802_11->Frag = 0;
    pHeader_802_11->FC.MoreData = 0;
    pHeader_802_11->FC.MoreFrag = 0;

#ifdef CONFIG_STATION
#if (ATCMD_ATE_SUPPORT == 1)
    if (gATEInfo.bATEMode == ATE_MODE_TX) {
        memcpy(pHeader_802_11->Addr1, gATEInfo.MacBssid, 6);   /* Addr1: BSSID */
        memcpy(pHeader_802_11->Addr2, gATEInfo.MacSA, 6);       /* Addr2: SA */
        memcpy(pHeader_802_11->Addr3, gATEInfo.MacDA, 6);       /* Addr3: DA */
    } else
#endif
    {
        memcpy(pHeader_802_11->Addr1, pIoTStaCfg->Bssid, 6);     /* Addr1: BSSID */
        memcpy(pHeader_802_11->Addr2, pIoTStaCfg->MyMacAddr, 6); /* Addr2: SA */
        memcpy(pHeader_802_11->Addr3, pTxBlk->pSrcBufHeader, 6);/* Addr3: DA */
    }
#else
    memcpy(pHeader_802_11->Addr1, pTxBlk->pSrcBufHeader, 6); /* Addr1: DA */
    memcpy(pHeader_802_11->Addr2, pIoTApCfg->MBSSID.Bssid, 6);/* Addr2: BSSID */
    memcpy(pHeader_802_11->Addr3, pIoTApCfg->MBSSID.Bssid, 6);/* Addr3: SA */
#endif /*CONFIG_STATION*/

    if (pTxBlk->CipherAlg != CIPHER_NONE)
        pHeader_802_11->FC.Wep = 1;

    // -----------------------------------------------------------------
    // STEP 2. MAKE A COMMON 802.11 HEADER SHARED BY ENTIRE FRAGMENT BURST. Fill sequence later.
    // -----------------------------------------------------------------
    pHeader_802_11->FC.PwrMgmt = 0;
}

/*
========================================================================
    Routine Description:
        This routine is used to fill TX_BLK packet attribute.
        TXINFO and TXWI is fill in other function
    Arguments:
        PktBuff     Pointer to send packet (mac802_3 + payload)
        PacketLen    Len of packet (include mac802_3 and payload length)
        bClearFrame: FALSE => encryption when sending
                             TRUE  => no encryption when sending
    Return Value:
        NDIS_STATUS_SUCCESS            If succes to queue the packet into TxSwQueue.
        NDIS_STATUS_FAILURE            If failed to do en-queue.
    Note:
    ========================================================================
*/
int sta_legacy_frame_tx(
    pBD_t PktBuff,
    uint16  PacketLen,
    bool    bClearFrame)
{
    HEADER_802_11    *pHeader_802_11;
    puchar            pHeaderBufPtr;
    TX_BLK            TxBlk;
    puchar            pPacket;

    pPacket = PktBuff->pBuf;
    memset(&TxBlk, 0, sizeof(TX_BLK));

    /* save the origin pointer for free mem */
    TxBlk.pPacket = PktBuff;

    /* skip 802.3 header */
    /* SrcBufData point to the payload */
    /* SrcBufLen  indicate the length of payload */
    TxBlk.pSrcBufHeader = pPacket;

#if (ATCMD_ATE_SUPPORT == 1)
    if (gATEInfo.bATEMode == ATE_MODE_TX) {
        TxBlk.pSrcBufData   = pPacket;
        TxBlk.SrcBufLen     = (uint32)PacketLen;
    } else
#endif
    {
        TxBlk.pSrcBufData   = pPacket   + LENGTH_802_3;
        TxBlk.SrcBufLen     = (uint32)(PacketLen - LENGTH_802_3);
    }
    TxBlk.Wcid          = BSSID_WCID;

    /* Finc encyption */
    /* Fill 802_11 hdr in TxBlk.HeaderBuf[] */
    STAFindCipherAlgorithm(&TxBlk, bClearFrame);
    STABuildCommon802_11Header(&TxBlk);

    /* skip common header, pHeaderBufPtr point to start of LLC */
    pHeaderBufPtr = &TxBlk.HeaderBuf[TXWI_SIZE];
    pHeader_802_11 = (HEADER_802_11 *) pHeaderBufPtr;
    pHeaderBufPtr += TxBlk.MpduHeaderLen;

    /* No WMM */
    /* The remaining content of MPDU should locate at 4-octets aligment */
    TxBlk.HdrPadLen = (uint32)pHeaderBufPtr;
    pHeaderBufPtr = (puchar)ROUND_UP(pHeaderBufPtr, 4);
    TxBlk.HdrPadLen = (uint32)(pHeaderBufPtr - TxBlk.HdrPadLen);

#if (ATCMD_ATE_SUPPORT == 1)
    if (gATEInfo.bATEMode == ATE_MODE_OFF)
#endif
    {
        /* Insert LLC-SNAP encapsulation - 8 octets */
        // if original Ethernet frame contains no LLC/SNAP,  then an extra LLC/SNAP encap is required
        memcpy(pHeaderBufPtr, SNAP_802_1H, 6);
        pHeaderBufPtr += 6;

        /* No vlan tag */
        // get 2 octets (TypeofLen)
        memcpy(pHeaderBufPtr, TxBlk.pSrcBufHeader+12, 2);
        pHeaderBufPtr += 2;
        TxBlk.MpduHeaderLen += LENGTH_802_1_H;
    }
    /* prepare for TXWI */
    RTMPWriteTxWI_Data((PTXWI_STRUC)(&TxBlk.HeaderBuf[0]), &TxBlk);
    /* Fill TXD content */
    WriteSingleTxResource(&TxBlk);

    /*2014/05/23,terrence
     * save the lastest tx time in Connected state,we use the tx idle duation to
     * deciede whether tx path ready to sleep */
#if (MT7681_POWER_SAVING == 1)
    pIoTStaCfg->PwrCfg.FCELastTxTime = kal_get_systime();
#endif

    return 0;
}


#ifdef CONFIG_SOFTAP
/* --------------------------------------------------------
*  FIND ENCRYPT KEY AND DECIDE CIPHER ALGORITHM
*        Find the WPA key, either Group or Pairwise Key
*        LEAP + TKIP also use WPA key.
* Decide WEP bit and cipher suite to be used.
* Same cipher suite should be used for whole fragment burst
*
*   bClearFrame: FALSE => encryption when sending
*                TRUE  => no encryption when sending
* -------------------------------------------------------- */
void APForwardFindCipherAlgorithm(
    TX_BLK    *pTxBlk,
    PMAC_TABLE_ENTRY  pEntry,
    bool    bClearFrame)
{
    uint8                            CipherAlg = CIPHER_NONE;        // cipher alogrithm
    uint8                            KeyIdx = 0xff;
    PCIPHER_KEY                     pKey   = NULL;

    /* GTK-rekey are encryption */
    /* Probe request, 4way handshaking are no encryption */
    if (bClearFrame) {
        KeyIdx    = 0xff;
        CipherAlg = CIPHER_NONE;
    } else {
#if CFG_SUPPORT_4WAY_HS
#ifdef CONFIG_SOFTAP
        if (!pEntry ||           /* M/BCAST to local BSS, use default key in shared key table */
            (pIoTApCfg->MBSSID.WepStatus == Ndis802_11Encryption1Enabled))  {  /* WEP always uses shared key table */
            KeyIdx     = pIoTApCfg->MBSSID.DefaultKeyId;
            CipherAlg = pIoTApCfg->MBSSID.SharedKey[BSS0][KeyIdx].CipherAlg;
            if (CipherAlg)
                pKey  = &pIoTApCfg->MBSSID.SharedKey[BSS0][KeyIdx];
            /* update Hardware Group Key Index */
            /* use Wcid as Hardware Key Index */
            GET_GroupKey_WCID(pTxBlk->Wcid, BSS0);
        } else  {
            /* unicast to local BSS */
            KeyIdx      = 0;
            CipherAlg = pEntry->PairwiseKey.CipherAlg;
            if (CipherAlg)
                pKey  = &pEntry->PairwiseKey;
            pTxBlk->Wcid = pEntry->Aid;
        }
#endif  /*CONFIG_SOFTAP*/
#endif  /*CFG_SUPPORT_4WAY_HS*/
    }

    pTxBlk->CipherAlg = CipherAlg;
    pTxBlk->pKey = pKey;
    pTxBlk->KeyIdx = KeyIdx;

    DBGPRINT(RT_DEBUG_INFO, ("%s %d, CipherAlg:%d, keyIdx:%d,  pTxBlk->Wcid:%d,  pEntry(%s)\n",
                             __FUNCTION__, __LINE__,pTxBlk->CipherAlg, pTxBlk->KeyIdx, pTxBlk->Wcid, (pEntry?"TRUE":"NULL")));
}


void APForwardBuildCommon802_11Header(
    TX_BLK          *pTxBlk,
    RX_BLK            *pRxBlk)
{
    HEADER_802_11    *pHeader_802_11;

    /*******************************/
    /* MAKE A COMMON 802.11 HEADER */
    /*******************************/

    /* normal wlan header size : 24 octets */
    pTxBlk->MpduHeaderLen = sizeof(HEADER_802_11);

    pHeader_802_11 = (HEADER_802_11 *) &pTxBlk->HeaderBuf[TXWI_SIZE];
    NdisZeroMemory(pHeader_802_11, sizeof(HEADER_802_11));

    pHeader_802_11->FC.Type     = pRxBlk->pHeader->FC.Type;
    pHeader_802_11->FC.SubType    = pRxBlk->pHeader->FC.SubType;
    pHeader_802_11->FC.FrDs = 1;
    pHeader_802_11->FC.ToDs = 0;
    pHeader_802_11->Sequence = gSequence++;
    pHeader_802_11->Frag = 0;
    pHeader_802_11->FC.MoreData = 0;
    pHeader_802_11->FC.MoreFrag = 0;

    memcpy(pHeader_802_11->Addr1, pRxBlk->pHeader->Addr3, MAC_ADDR_LEN); /* Addr1: DA */
    memcpy(pHeader_802_11->Addr2, pIoTApCfg->MBSSID.Bssid, 6);                      /* Addr2: BSSID */
    memcpy(pHeader_802_11->Addr3, pRxBlk->pHeader->Addr2, MAC_ADDR_LEN); /* Addr3: SA */

    if (pTxBlk->CipherAlg != CIPHER_NONE)
        pHeader_802_11->FC.Wep = 1;

    // -----------------------------------------------------------------
    // STEP 2. MAKE A COMMON 802.11 HEADER SHARED BY ENTIRE FRAGMENT BURST. Fill sequence later.
    // -----------------------------------------------------------------
    pHeader_802_11->FC.PwrMgmt = 0;
}

/*
========================================================================
    Routine Description:
        This routine is used to fill TX_BLK packet attribute with RX_BLK.
        TXINFO and TXWI is fill in other function
    Arguments:
            pRX_BLK:     Pointer to RX_BLK  including RxPacket (SNAP + payload), and RxPacketLen
            bClearFrame: FALSE => encryption when sending
                                 TRUE  => no encryption when sending
    Return Value:
        NDIS_STATUS_SUCCESS            If succes to queue the packet into TxSwQueue.
        NDIS_STATUS_FAILURE            If failed to do en-queue.
    Note:
        You only can put OS-indepened & STA related code in here.
    ========================================================================
*/
int AP_Legacy_Frame_Forward_Tx(
    RX_BLK* pRxBlk,
    bool    bClearFrame)
{
    HEADER_802_11    *pHeader_802_11;
    puchar            pHeaderBufPtr;
    TX_BLK           TxBlk;
    pBD_t             pBufDesc = NULL;
    PMAC_TABLE_ENTRY pEntry = NULL;

    pEntry = MacTableLookup(pRxBlk->pHeader->Addr3);
    if ((pRxBlk->pHeader->Addr3[0] & 0x1) == 0) { /*not multicast/broadcast frame*/
        /*if the DA is not in the MACEntery Table or this DA secureport is not open,  give up forwarding */
        if ((pEntry == NULL) || (pEntry->PortSecured == WPA_802_1X_PORT_NOT_SECURED)) {
            printf("%s,%d  not legal Entry, give up forwarding \n", __FUNCTION__,__LINE__);
            return NDIS_STATUS_FAILURE;
        }
    }

    //handle_FCE_TxTS_interrupt();
    pBufDesc = apiQU_Dequeue(&gFreeQueue2);
    if (pBufDesc == NULL) {
        printf("dequeue fail\n");
        return NDIS_STATUS_FAILURE;
    }

    /*skip 802.11 Header,  copy LLC+payload  to pBuf */
    memcpy(pBufDesc->pBuf,
           pRxBlk->pData + sizeof(HEADER_802_11),
           pRxBlk->DataSize - sizeof(HEADER_802_11));

    memset(&TxBlk, 0, sizeof(TX_BLK));
    /* save the origin pointer for free mem */
    TxBlk.pPacket = pBufDesc;

    /* SrcBufData point to the payload */
    /* SrcBufLen  indicate the length of payload */
    TxBlk.pSrcBufHeader= NULL;
    TxBlk.pSrcBufData   = pBufDesc->pBuf;     //pData;
    TxBlk.SrcBufLen       = pRxBlk->DataSize; //Length;
    TxBlk.Wcid              = BSSID_WCID;

    /* Finc encyption */
    /* Fill 802_11 hdr in TxBlk.HeaderBuf[] */
    APForwardFindCipherAlgorithm(&TxBlk, pEntry, bClearFrame);
    APForwardBuildCommon802_11Header(&TxBlk, pRxBlk);

    /* skip common header, pHeaderBufPtr point to start of LLC */
    pHeaderBufPtr = &TxBlk.HeaderBuf[TXWI_SIZE];
    pHeader_802_11 = (HEADER_802_11 *) pHeaderBufPtr;
    pHeaderBufPtr += TxBlk.MpduHeaderLen;

    /* No WMM */
    /* The remaining content of MPDU should locate at 4-octets aligment */
    TxBlk.HdrPadLen = (uint32)pHeaderBufPtr;
    pHeaderBufPtr = (puchar)ROUND_UP(pHeaderBufPtr, 4);
    TxBlk.HdrPadLen = (uint32)(pHeaderBufPtr - TxBlk.HdrPadLen);

    /* prepare for TXWI */
    RTMPWriteTxWI_Data((PTXWI_STRUC)(&TxBlk.HeaderBuf[0]), &TxBlk);
    /* Fill TXD content */
    WriteSingleTxResource(&TxBlk);

    return NDIS_STATUS_SUCCESS;
}
#endif


NDIS_STATUS MiniportMMRequest(
    IN  uint8           QueIdx,
    IN  puchar          pData,
    IN  uint32            Length,
    IN  pBD_t           pBufDesc)
{
    puchar         pHeaderBufPtr;
    TX_BLK        TxBlk;
    puchar          pOutBuffer = NULL;
    PHEADER_802_11  pHdr80211  = NULL;
    pOutBuffer = pBufDesc->pBuf;
    pHdr80211 = (PHEADER_802_11)pOutBuffer;

    /*2014/05/23,terrence
     * save the lastest tx time in Connected state,we use the tx idle duation to
     * deciede whether tx path ready to sleep */
#if (MT7681_POWER_SAVING == 1)
    if ((pHdr80211->FC.Type != BTYPE_DATA) && (pHdr80211->FC.SubType != SUBTYPE_NULL_FUNC))
        pIoTStaCfg->PwrCfg.FCELastTxTime = kal_get_systime();
#endif

    memset(&TxBlk, 0, sizeof(TX_BLK));

    /* save the origin pointer for free mem */
    TxBlk.pPacket = pBufDesc;

    /* SrcBufData point to the payload */
    /* SrcBufLen  point to the length of payload */
    TxBlk.pSrcBufData   = NULL;    //pData;
    TxBlk.SrcBufLen     = 0;       //Length;
    TxBlk.pSrcBufHeader = pData;
    TxBlk.Wcid          = RESERVED_WCID;

    /* copy common header */
    pHeaderBufPtr = &TxBlk.HeaderBuf[TXWI_SIZE];
    memcpy(pHeaderBufPtr, pData, Length);
    TxBlk.MpduHeaderLen = Length;
    pHeaderBufPtr += TxBlk.MpduHeaderLen;

    /* The remaining content of MPDU should locate at 4-octets aligment */
    TxBlk.HdrPadLen = (uint32)pHeaderBufPtr;
    pHeaderBufPtr = (puchar)ROUND_UP(pHeaderBufPtr, 4);
    TxBlk.HdrPadLen = (uint32)(pHeaderBufPtr - TxBlk.HdrPadLen);

    /* prepare for TXWI */
    /* use Wcid as Key Index */
    if (pHdr80211->FC.Type == BTYPE_MGMT) {
        RTMPWriteTxWI_Mgmt((PTXWI_STRUC)(&TxBlk.HeaderBuf[0]), &TxBlk);
    } else if (pHdr80211->FC.Type == BTYPE_DATA) { //For NULL data
        RTMPWriteTxWI_Data((PTXWI_STRUC)(&TxBlk.HeaderBuf[0]), &TxBlk);
    }

    WriteSingleTxResource(&TxBlk);
    return TRUE;
}
#endif

void RTMPWriteTxWI_Data(
    PTXWI_STRUC        pTxWI,
    TX_BLK            *pTxBlk)
{
    // Always use Long preamble before verifiation short preamble functionality works well.
    // Todo: remove the following line if short preamble functionality works
    NdisZeroMemory(pTxWI, TXWI_SIZE);
    pTxWI->FRAG            = 0;
#if (ATCMD_ATE_SUPPORT == 1)
    pTxWI->ACK            = 0;
#else
    pTxWI->ACK            = 1;
#endif
    pTxWI->txop            = 0; // IFS_HTTXOP
    pTxWI->WirelessCliID= pTxBlk->Wcid;
    pTxWI->MPDUtotalByteCount    = pTxBlk->MpduHeaderLen + pTxBlk->SrcBufLen;
    pTxWI->CFACK        = 0;
#ifdef CONFIG_SOFTAP
    //HW will add SEQ Number
    pTxWI->NSEQ = 1;
#endif
    pTxWI->AMPDU         = FALSE;
    pTxWI->BAWinSize    = 7;
    pTxWI->STBC           = 0;
    pTxWI->MIMOps        = 0;
    pTxWI->MpduDensity = 0; // need check pMacEntry->MpduDensity;

    //terrence,2014/06/29,set Packet ID
    g_tx_pkt_id++;
    if (g_tx_pkt_id >= 16) {
        g_tx_pkt_id = 1;
    }
    pTxWI->PacketId = g_tx_pkt_id;

#if (ATCMD_ATE_SUPPORT == 1)
    if (gATEInfo.bATEMode == ATE_MODE_TX) {
        // If CCK or OFDM, BW must be 20
        pTxWI->BW            = gATEInfo.TxWI.BW;
        pTxWI->ShortGI        = gATEInfo.TxWI.ShortGI;
        pTxWI->MCS            = gATEInfo.TxWI.MCS;
        pTxWI->PHYMODE        = gATEInfo.TxWI.PHYMODE;
    } else
#endif

#ifdef CONFIG_SOFTAP
    {
        // If CCK or OFDM, BW must be 20
        pTxWI->BW            = 0;
        pTxWI->ShortGI        = 0;
        pTxWI->MCS            = 0;
        /* Bennett +++ debug 20120308 */
        //pTxWI->PHYMODE = pAd.phyMode;
        pTxWI->PHYMODE        = 1; //0:CCK  1:OFDM
        /* debug finish */
    }
#else
    {
        pTxWI->BW           = pIoTStaCfg->Cfg_BW;
        pTxWI->ShortGI     = 0;
        if (pIoTStaCfg->Sta_PhyMode>= PHY_11N) {
            if(pIoTStaCfg->HtCapabilityLen) {
                if(!pIoTStaCfg->Cfg_BW) //BW_20
                    pTxWI->ShortGI   = pIoTStaCfg->ShortGIfor20;
                else
                    pTxWI->ShortGI   = pIoTStaCfg->ShortGIfor40;
            }
        }
        pTxWI->MCS             = pIoTStaCfg->AP_MCS;
        pTxWI->PHYMODE	= pIoTStaCfg->AP_PhyMode; //0:CCK  1:OFDM  2:mix  3:greenfield
    }
#endif
}

void RTMPWriteTxWI_Mgmt(
    PTXWI_STRUC        pTxWI,
    TX_BLK            *pTxBlk)
{
    // Always use Long preamble before verifiation short preamble functionality works well.
    // Todo: remove the following line if short preamble functionality works
    NdisZeroMemory(pTxWI, TXWI_SIZE);

    pTxWI->FRAG        = 0;
#if (ATCMD_ATE_SUPPORT == 1)
    pTxWI->ACK        = 0;
#else
    pTxWI->ACK        = 1;
#endif
    pTxWI->txop        = 0; // IFS_HTTXOP
    pTxWI->WirelessCliID        = pTxBlk->Wcid;
    pTxWI->MPDUtotalByteCount    = pTxBlk->MpduHeaderLen + pTxBlk->SrcBufLen;
    pTxWI->CFACK       = 0;
    // If CCK or OFDM, BW must be 20
    pTxWI->BW            = 0;
    pTxWI->AMPDU       = FALSE;
#ifdef CONFIG_SOFTAP
    //HW will add SEQ Number
    pTxWI->NSEQ        = 1;
#endif
    pTxWI->BAWinSize = 7;
    pTxWI->ShortGI     = 0;
    pTxWI->STBC        = 0;
    pTxWI->MCS          = 0;     /* rate1: 0 */
    pTxWI->PHYMODE  = 0;      /* CCK:0, OFDM:1 */
    pTxWI->MIMOps      = 0;
    pTxWI->MpduDensity = 0; // need check pMacEntry->MpduDensity;

    //terrence,2014/06/29,set Packet ID
    g_tx_pkt_id++;
    if (g_tx_pkt_id >= 16) {
        g_tx_pkt_id = 1;
    }
    pTxWI->PacketId = g_tx_pkt_id;

#ifdef CONFIG_SOFTAP
    if (SUBTYPE_PROBE_RSP == ((HEADER_802_11 *)pTxBlk->pSrcBufHeader)->FC.SubType) {
        pTxWI->TS    = 1;
        pTxWI->ACK = 0;
    }
#endif
}


uint8 get_cent_ch_by_htinfo(
    ADD_HT_INFO_IE *ht_op,
    HT_CAPABILITY_IE *ht_cap)
{
    uint8 cent_ch;

    if ((ht_op->ControlChan > 2)&&
        (ht_op->AddHtInfo.ExtChanOffset == EXTCHA_BELOW) &&
        (ht_cap->HtCapInfo.ChannelWidth == BW_40))
        cent_ch = ht_op->ControlChan - 2;
    else if ((ht_op->AddHtInfo.ExtChanOffset == EXTCHA_ABOVE) &&
             (ht_cap->HtCapInfo.ChannelWidth == BW_40))
        cent_ch = ht_op->ControlChan + 2;
    else
        cent_ch = ht_op->ControlChan;

    return cent_ch;
}


uint8 get_primary_ch_by_cent_ch(
    uint8  cent_ch,
    uint8  ExtChanOffset,
    uint8  cfg_bw)
{
    uint8 primary_ch = cent_ch;

    if ((cfg_bw == BW_20) || (ExtChanOffset == EXTCHA_NONE)) {
        primary_ch = cent_ch;
    } else {
        if ((cent_ch < 3) || (cent_ch > 11))
            return 0;

        if (ExtChanOffset == EXTCHA_BELOW) {
            primary_ch = cent_ch + 2;
        } else if (ExtChanOffset == EXTCHA_ABOVE) {
            primary_ch = cent_ch - 2;
        }
    }

    return primary_ch;
}

/*
==========================================================================
Description:
    Parse incoming beacon
Return:
       TRUE: The Primary Channel infor in 'HT info' IE of the received Beacon equals current primary channel
       otherwise return FALSE
==========================================================================
 */
bool PeerBeaconCHInfoParse(RX_BLK* pRxBlk)
{
#ifdef CONFIG_STATION
    uint8           Bssid[MAC_ADDR_LEN], Addr2[MAC_ADDR_LEN];
    uint8           Ssid[MAX_LEN_OF_SSID+1], Channel = 0, SsidLen;
    uint16         BeaconPeriod, CapabilityInfo;
    PFRAME_802_11   pFrame;
    uint8           SupRate[MAX_LEN_OF_SUPPORTED_RATES], ExtRate[MAX_LEN_OF_SUPPORTED_RATES];
    uint8           SupRateLen, ExtRateLen;
    uint16         LenVIE;
    uint8           VarIE[MAX_VIE_LEN];        // Total VIE length = MAX_VIE_LEN - -5
    NDIS_802_11_VARIABLE_IEs    *pVIE = NULL;
    HT_CAPABILITY_IE    HtCapability;
    ADD_HT_INFO_IE       AddHtInfo;        // AP might use this additional ht info IE
    uint8           HtCapabilityLen = 0;
    uint8           AddHtInfoLen = 0;
    EXT_CAP_INFO_ELEMENT    ExtCapInfo;
    SECURITY_IE             SecurityInfo;
    bool            flag = FALSE;

    printf("PeerBeaconCHInfo: seq=%d \n",pRxBlk->pHeader->Sequence);

    NdisZeroMemory(Ssid, MAX_LEN_OF_SSID+1); /* the last byte '\0' for debug */
    Ssid[MAX_LEN_OF_SSID] = '\0';
    pFrame = (PFRAME_802_11) pRxBlk->pData;

    // Init Variable IE structure
    pVIE = (PNDIS_802_11_VARIABLE_IEs) VarIE;
    pVIE->Length = 0;
    RTMPZeroMemory(&HtCapability, sizeof(HtCapability));
    RTMPZeroMemory(&AddHtInfo, sizeof(ADD_HT_INFO_IE));
    RTMPZeroMemory(&SecurityInfo, sizeof(SECURITY_IE));

    if (overlayload(3)) {
        flag = PeerBeaconAndProbeRspSanity(
                   IN    pRxBlk->pData,
                   IN    pRxBlk->DataSize,
                   OUT Addr2,
                   OUT Bssid,
                   OUT (int8*)Ssid,
                   OUT &SsidLen,
                   OUT &BeaconPeriod,
                   OUT &Channel,
                   OUT &CapabilityInfo,
                   OUT SupRate,
                   OUT &SupRateLen,
                   OUT ExtRate,
                   OUT &ExtRateLen,
                   OUT &HtCapabilityLen,
                   OUT &HtCapability,
                   OUT &ExtCapInfo,
                   OUT &AddHtInfoLen,
                   OUT &AddHtInfo,
                   OUT &SecurityInfo,
                   OUT &LenVIE,
                   OUT pVIE);
        overlayunload(3);
    }

    /* AP TP-Link:TL-WR841N  has a issue which */
    /* The IE value of AddHTinfo.ExtChanOffset, AddHTinfo.RecomWidth  maybe changed between (0,0) and (3, 1)*/
    /* This will cause MT7681 try to disconnect AP with blow implementation by judge center channel */
    /* Thus there is a new method to judge primary channel to avoid this IOT issue */
#if 0
    if (AddHtInfoLen) {
        if ((pIoTStaCfg->ExtChanOffset != AddHtInfo.AddHtInfo.ExtChanOffset)  ||
            (pIoTStaCfg->Cfg_Channel != get_cent_ch_by_htinfo(&AddHtInfo, &HtCapability))) {
            printf("Return False: ExtCH=%d, PrimaryCH=%d \n",
                   AddHtInfo.AddHtInfo.ExtChanOffset, AddHtInfo.ControlChan);
            return FALSE;
        }
    }
#else
    if (AddHtInfoLen) {
        uint8 ret;
        ret = get_primary_ch_by_cent_ch(pIoTStaCfg->Cfg_Channel,
                                            pIoTStaCfg->ExtChanOffset,
                                            pIoTStaCfg->Cfg_BW);

#if 0
        printf_high("pIoTExtChanOffset= %d, pIoTCfg_Channel=%d, pIoTCfg_BW=%d\n",
                    pIoTStaCfg->ExtChanOffset,  pIoTStaCfg->Cfg_Channel, pIoTStaCfg->Cfg_BW);

        printf_high("ExtChanOffset=%d, ControlChan=%d , get_primary_ch=%d ,ChWidth=%d \n",
                    AddHtInfo.AddHtInfo.ExtChanOffset,
                    AddHtInfo.ControlChan,
                    ret,
                    HtCapability.HtCapInfo.ChannelWidth);
#endif

        if ((ret != 0) && (ret != AddHtInfo.ControlChan)) {
            printf("Return False: ExtCH=%d, PrimaryCH=%d \n",
                   AddHtInfo.AddHtInfo.ExtChanOffset, AddHtInfo.ControlChan);
            return FALSE;
        }
    }
#endif
#endif

    return TRUE;
}

