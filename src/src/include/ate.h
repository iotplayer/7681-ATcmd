#ifndef __ATE_H__
#define __ATE_H__

#include "types.h"
#include "rtmp_general_pub.h"

#define ATE_TX_PAYLOAD_LEN         (800)     /*must  <=  1024 - 28 (24Bytes[MAC Header]+4Bytes[FCS])*/
#define ATE_TX_SPEED              (1)     /*unit: 1ms*/
#define ATE_TX_CNT_PER_SPEED      (1)     /*send how many Tx Packet during each ATE_TX_SPEED */
#define ATE_RX_CNT_DURATION         (1000)  /*unit: 1ms*/


/* */
/*  Statistic counter structure */
/* */
typedef struct _COUNTER_802_3 {
    /* General Stats */
    uint32 GoodTransmits;
    uint32 GoodReceives;
    uint32 TxErrors;
    uint32 RxErrors;
    uint32 RxNoBuffer;

    /* Ethernet Stats */
    uint32 RcvAlignmentErrors;
    uint32 OneCollision;
    uint32 MoreCollisions;

} COUNTER_802_3, *PCOUNTER_802_3;

typedef struct _COUNTER_802_11 {
    uint32 Length;
    /*    LARGE_INTEGER   LastTransmittedFragmentCount; */
    LARGE_INTEGER TransmittedFragmentCount;
    LARGE_INTEGER MulticastTransmittedFrameCount;
    LARGE_INTEGER FailedCount;
    LARGE_INTEGER RetryCount;
    LARGE_INTEGER MultipleRetryCount;
    LARGE_INTEGER RTSSuccessCount;
    LARGE_INTEGER RTSFailureCount;
    LARGE_INTEGER ACKFailureCount;
    LARGE_INTEGER FrameDuplicateCount;
    LARGE_INTEGER ReceivedFragmentCount;
    LARGE_INTEGER MulticastReceivedFrameCount;
    LARGE_INTEGER FCSErrorCount;
    LARGE_INTEGER TransmittedFrameCount;
    LARGE_INTEGER WEPUndecryptableCount;
    LARGE_INTEGER TransmitCountFrmOs;
} COUNTER_802_11, *PCOUNTER_802_11;

typedef struct _COUNTER_RALINK {
    uint32 OneSecStart;            /* for one sec count clear use */
    uint32 OneSecBeaconSentCnt;
    uint32 OneSecFalseCCACnt;    /* CCA error count, for debug purpose, might move to global counter */
    uint32 OneSecRxFcsErrCnt;    /* CRC error */
    uint32 OneSecRxOkCnt;        /* RX without error */
    uint32 OneSecTxFailCount;
    uint32 OneSecTxNoRetryOkCount;
    uint32 OneSecTxRetryOkCount;
    uint32 OneSecRxOkDataCnt;    /* unicast-to-me DATA frame count */
    uint32 OneSecTransmittedByteCount;    /* both successful and failure, used to calculate TX throughput */

    uint32 OneSecOsTxCount[NUM_OF_TX_RING];
    uint32 OneSecDmaDoneCount[NUM_OF_TX_RING];
    uint32 OneSecTxDoneCount;
    uint32 OneSecRxCount;
    uint32 OneSecReceivedByteCount;
    uint32 OneSecTxAggregationCount;
    uint32 OneSecRxAggregationCount;
    uint32 OneSecEnd;    /* for one sec count clear use */

    uint32 TransmittedByteCount;    /* both successful and failure, used to calculate TX throughput */
    uint32 ReceivedByteCount;    /* both CRC okay and CRC error, used to calculate RX throughput */
    uint32 BadCQIAutoRecoveryCount;
    uint32 PoorCQIRoamingCount;
    uint32 MgmtRingFullCount;
    uint32 RxCountSinceLastNULL;
    uint32 RxCount;
    uint32 KickTxCount;
    LARGE_INTEGER RealFcsErrCount;
    uint32 PendingNdisPacketCount;
    uint32 FalseCCACnt;              /* CCA error count */

    uint32 LastOneSecTotalTxCount;    /* OneSecTxNoRetryOkCount + OneSecTxRetryOkCount + OneSecTxFailCount */
    uint32 LastOneSecRxOkDataCnt;    /* OneSecRxOkDataCnt */
    uint32 DuplicateRcv;
    uint32 TxAggCount;
    uint32 TxNonAggCount;
    uint32 TxAgg1MPDUCount;
    uint32 TxAgg2MPDUCount;
    uint32 TxAgg3MPDUCount;
    uint32 TxAgg4MPDUCount;
    uint32 TxAgg5MPDUCount;
    uint32 TxAgg6MPDUCount;
    uint32 TxAgg7MPDUCount;
    uint32 TxAgg8MPDUCount;
    uint32 TxAgg9MPDUCount;
    uint32 TxAgg10MPDUCount;
    uint32 TxAgg11MPDUCount;
    uint32 TxAgg12MPDUCount;
    uint32 TxAgg13MPDUCount;
    uint32 TxAgg14MPDUCount;
    uint32 TxAgg15MPDUCount;
    uint32 TxAgg16MPDUCount;

    LARGE_INTEGER TransmittedOctetsInAMSDU;
    LARGE_INTEGER TransmittedAMSDUCount;
    LARGE_INTEGER ReceivedOctesInAMSDUCount;
    LARGE_INTEGER ReceivedAMSDUCount;
    LARGE_INTEGER TransmittedAMPDUCount;
    LARGE_INTEGER TransmittedMPDUsInAMPDUCount;
    LARGE_INTEGER TransmittedOctetsInAMPDUCount;
    LARGE_INTEGER MPDUInReceivedAMPDUCount;
} COUNTER_RALINK, *PCOUNTER_RALINK;

typedef struct _COUNTER_TMP_1S {
    uint32 FalseCCACnt1S;
    uint32 RxCntU2M1S;
    uint32 RxCntBM1S;
    uint32 RxCntMgmtCntl1S;
} COUNTER_TMP_1S,*PCOUNTER_TMP_1S;


typedef struct GNU_PACKED _ATE_INFO {
    uint8  bATEMode;
    uint8  ATECmdFmt;     /*ATE cmd format : 0=AT#ATECAL ,   1=iwpriv ra0*/
    puchar MacSA;
    puchar MacBssid;
    puchar MacDA;
    uint8  Channel;

    /*Tx Test*/
    uint8 RfFreqOffset;
    uint8 TxPower0;
    TXWI_STRUC TxWI;    /* TXWI */
    uint32 TxSpeed;       /*units 1ms - the frequency to do SendProcess*/
    uint32 TxCntPerSpeed; /*How many packet be sent  within each of SendProcess*/
    uint32 TxCount;          /*How many SendProcess be called */
    uint32 PayLoadLen;    /*The payLoad length for Tx packet, not contain Mac Header*/

    /*Rx Test*/
    uint32 RxU2MTotalCnt;
    uint32 RxBMTotalCnt;
    uint32 RxMgmtCntlTotalCnt;
    uint32 RxCntU2MPerSec;
    uint32 RxCntBMPerSec;
    uint32 RxCntMgmtCntlPerSec;
    uint32 RxCntDuration;

    int8 LastSnr0;        /* last received SNR */
    int8 LastSnr1;        /* last received SNR for 2nd  antenna */
    int8 LastSnr2;
    int8 AvgSnr0;
    int8 AvgSnr1;
    int8 AvgSnr2;
    int16 AvgSnr0X8;
    int16 AvgSnr1X8;
    int16 AvgSnr2X8;

    int8 LastRssi0;        /* last received RSSI */
    int8 LastRssi1;        /* last received RSSI for 2nd  antenna */
    int8 LastRssi2;        /* last received RSSI for 3rd  antenna */
    int8 AvgRssi0;        /* last 8 frames' average RSSI */
    int8 AvgRssi1;        /* last 8 frames' average RSSI */
    int8 AvgRssi2;        /* last 8 frames' average RSSI */
    int16 AvgRssi0X8;    /* sum of last 8 frames' RSSI */
    int16 AvgRssi1X8;    /* sum of last 8 frames' RSSI */
    int16 AvgRssi2X8;    /* sum of last 8 frames' RSSI */
    uint32 NumOfAvgRssiSample;

    COUNTER_802_3 Counters8023; /* 802.3 counters */
    COUNTER_802_11 WlanCounters;    /* 802.11 MIB counters */
    COUNTER_RALINK RalinkCounters;    /* Ralink propriety counters */
    COUNTER_TMP_1S OneSecCountersTmp;
} ATE_INFO, *PATE_INFO;


typedef enum {
    ATE_MODE_OFF = 0,
    ATE_MODE_TX,
    ATE_MODE_RX,
    ATE_MODE_MAX
} ATE_MODE;

typedef enum {
    ATE_CMD_TYPE_AT = 0,
    ATE_CMD_TYPE_IWPRIV,
    ATE_CMD_TYPE_MAX
} ATE_CMD_TYPE;


void SendATETxDataFrame(void);
bool    Set_ResetStatCounter_Proc(void);
#endif  //__ATE_H__
