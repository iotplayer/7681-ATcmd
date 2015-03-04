#ifndef _AP_PUB_H_
#define _AP_PUB_H_

#include "types.h"
#include "iot_api.h"

/***************************************************************************
  *    Customization Macro
  **************************************************************************/
#define IOT_BCN_TIM_KEEP_ON   /*If open this Macro,  the TIM value in Beacon is 1 for each MAC Entry*/


/***************************************************************************
  *    AP Common Macro
  **************************************************************************/
#define CLIENT_STATUS_SET_FLAG(_pEntry,_F)      ((_pEntry)->ClientStatusFlags |= (_F))
#define CLIENT_STATUS_CLEAR_FLAG(_pEntry,_F)    ((_pEntry)->ClientStatusFlags &= ~(_F))
#define CLIENT_STATUS_TEST_FLAG(_pEntry,_F)     (((_pEntry)->ClientStatusFlags & (_F)) != 0)

/* STA_CSR4.field.TxResult */
#define TX_RESULT_SUCCESS               0
#define TX_RESULT_ZERO_LENGTH           1
#define TX_RESULT_UNDER_RUN             2
#define TX_RESULT_OHY_ERROR             4
#define TX_RESULT_RETRY_FAIL            6


/* MCS for CCK.  BW.SGI.STBC are reserved */
#define MCS_LONGP_RATE_1                0    /* long preamble CCK 1Mbps */
#define MCS_LONGP_RATE_2                1    /* long preamble CCK 1Mbps */
#define MCS_LONGP_RATE_5_5              2
#define MCS_LONGP_RATE_11               3
#define MCS_SHORTP_RATE_1               4    /* long preamble CCK 1Mbps. short is forbidden in 1Mbps */
#define MCS_SHORTP_RATE_2               5    /* short preamble CCK 2Mbps */
#define MCS_SHORTP_RATE_5_5             6
#define MCS_SHORTP_RATE_11              7
/* To send duplicate legacy OFDM. set BW=BW_40.  SGI.STBC are reserved */
#define MCS_RATE_6                      0    /* legacy OFDM */
#define MCS_RATE_9                      1    /* OFDM */
#define MCS_RATE_12                     2    /* OFDM */
#define MCS_RATE_18                     3    /* OFDM */
#define MCS_RATE_24                     4    /* OFDM */
#define MCS_RATE_36                     5    /* OFDM */
#define MCS_RATE_48                     6    /* OFDM */
#define MCS_RATE_54                     7    /* OFDM */
/* HT */
#define MCS_0         0    /* 1S */
#define MCS_1         1
#define MCS_2         2
#define MCS_3         3
#define MCS_4         4
#define MCS_5         5
#define MCS_6         6
#define MCS_7         7
#define MCS_8         8    /* 2S */
#define MCS_9         9
#define MCS_10        10
#define MCS_11        11
#define MCS_12        12
#define MCS_13        13
#define MCS_14        14
#define MCS_15        15
#define MCS_16        16    /* 3*3 */
#define MCS_17        17
#define MCS_18        18
#define MCS_19        19
#define MCS_20        20
#define MCS_21        21
#define MCS_22        22
#define MCS_23        23
#define MCS_32        32
#define MCS_AUTO      33

/* Fixed Tx MODE - HT, CCK or OFDM */
#define FIXED_TXMODE_HT       0
#define FIXED_TXMODE_CCK      1
#define FIXED_TXMODE_OFDM     2
#define FIXED_TXMODE_VHT      3


#define MAX_NUMBER_OF_MAC     3 //32    /* if MAX_MBSSID_NUM is 8, this value can't be larger than 211 */
#define MAX_BEACON_SIZE       512
#define MAX_SSID_LEN              32

#define TBTT_PRELOAD_TIME     384 /* usec. LomgPreamble + 24-byte at 1Mbps */
#define DEFAULT_DTIM_PERIOD   1

#define CAP_GENERATE(ess,  ibss,  priv,  s_pre,  s_slot,  spectrum)  \
    (((ess) ? 0x0001 : 0x0000) | ((ibss) ? 0x0002 : 0x0000) | \
    ((priv) ? 0x0010 : 0x0000) | ((s_pre) ? 0x0020 : 0x0000) | \
    ((s_slot) ? 0x0400 : 0x0000) | ((spectrum) ? 0x0100 : 0x0000))

#define MAX_MBSSID_NUM()      1
#define HW_RESERVED_WCID()    (pIoTApCfg->WcidHwRsvNum)
#define LAST_SPECIFIC_WCID()  (HW_RESERVED_WCID() - 2)

/* TX need WCID to find Cipher Key */
/* these wcid 212 ~ 219 are reserved for bc/mc packets if MAX_MBSSID_NUM is 8. */
#if 0
/*if set wcid as 0,  and call RTMPSetWcidSecurityInfo() in WPAInstallSharedKey(),
  the Broadcast Packet from Peer station can not be decrypted*/
#define GET_GroupKey_WCID(__wcid, __bssidx) \
    {                                        \
        __wcid = __bssidx;    \
    }
#else
#define GET_GroupKey_WCID(__wcid, __bssidx) \
    {                                        \
        __wcid = LAST_SPECIFIC_WCID() - (MAX_MBSSID_NUM()) + __bssidx;    \
    }
#endif
/***************************************************************************
  *    Multiple SSID related data structures
  **************************************************************************/
#define WLAN_MAX_NUM_OF_TIM            ((MAX_LEN_OF_MAC_TABLE >> 3) + 1)    /* /8 + 1 */
#define WLAN_CT_TIM_BCMC_OFFSET        0    /* unit: 32B */

/* clear bcmc TIM bit */
#define WLAN_MR_TIM_BCMC_CLEAR() \
    pIoTApCfg->MBSSID.TimBitmaps[WLAN_CT_TIM_BCMC_OFFSET] &= ~NUM_BIT8[0];

/* set bcmc TIM bit */
#define WLAN_MR_TIM_BCMC_SET() \
    pIoTApCfg->MBSSID.TimBitmaps[WLAN_CT_TIM_BCMC_OFFSET] |= NUM_BIT8[0];

/* clear a station PS TIM bit */
#define WLAN_MR_TIM_BIT_CLEAR(wcid) \
    {    uint8 tim_offset = wcid >> 3; \
        uint8 bit_offset = wcid & 0x7; \
        pIoTApCfg->MBSSID.TimBitmaps[tim_offset] &= (~NUM_BIT8[bit_offset]); \
        APUpdateBeaconFrame(); }

/* set a station PS TIM bit */
#define WLAN_MR_TIM_BIT_SET(wcid) \
    {    uint8 tim_offset = wcid >> 3; \
        uint8 bit_offset = wcid & 0x7; \
        pIoTApCfg->MBSSID.TimBitmaps[tim_offset] |= NUM_BIT8[bit_offset];}


/*============================================================ */
/* ASIC WCID Table definition. */
/*============================================================ */
#define BSSID_WCID        1    /* in infra mode, always put bssid with this WCID */
#define MCAST_WCID        0x0
#define BSS0Mcast_WCID    0x0
#define BSS1Mcast_WCID    0xf8
#define BSS2Mcast_WCID    0xf9
#define BSS3Mcast_WCID    0xfa
#define BSS4Mcast_WCID    0xfb
#define BSS5Mcast_WCID    0xfc
#define BSS6Mcast_WCID    0xfd
#define BSS7Mcast_WCID    0xfe
#define RESERVED_WCID     0xff

#define MAX_LEN_OF_MAC_TABLE          MAX_NUMBER_OF_MAC     /* The MAC Table length in pEntry which in SRAM, start from 0 */
#define MAX_LEN_OF_MAC_TABLE_IN_REG   (MAX_NUMBER_OF_MAC+1)    /* The MAC Table length in Register, start from 1*/
/*#if MAX_LEN_OF_MAC_TABLE>MAX_AVAILABLE_CLIENT_WCID */
/*#error MAX_LEN_OF_MAC_TABLE can not be larger than MAX_AVAILABLE_CLIENT_WCID!!!! */
/*#endif */

#define ENTRY_NONE          0
#define ENTRY_CLIENT        1

#define IS_ENTRY_NONE(_x)       ((_x)->EntryType == ENTRY_NONE)
#define IS_ENTRY_CLIENT(_x)     ((_x)->EntryType == ENTRY_CLIENT)

#define SET_ENTRY_NONE(_x)      ((_x)->EntryType = ENTRY_NONE)
#define SET_ENTRY_CLIENT(_x)    ((_x)->EntryType = ENTRY_CLIENT)

typedef enum _MAC_ENTRY_OP_MODE_ {
    ENTRY_OP_MODE_ERROR   = 0x00,
    ENTRY_OP_MODE_CLI     = 0x01,    /* Sta mode, set this TRUE after Linkup,too. */
    ENTRY_OP_MODE_WDS     = 0x02,    /* This is WDS Entry. only for AP mode. */
    ENTRY_OP_MODE_APCLI   = 0x04,    /* This is a AP-Client entry, only for AP mode which enable AP-Client functions. */
    ENTRY_OP_MODE_MESH    = 0x08,    /* Peer conect with us via mesh. */
    ENTRY_OP_MODE_DLS     = 0x10,    /* This is DLS Entry. only for STA mode. */
    ENTRY_OP_MODE_MAX     = 0x20
} MAC_ENTRY_OP_MODE;

/* Values of LastSecTxRateChangeAction */
#define RATE_NO_CHANGE    0        /* No change in rate */
#define RATE_UP           1        /* Trying higher rate or same rate with different BF */
#define RATE_DOWN         2        /* Trying lower rate */

typedef union _HTTRANSMIT_SETTING {
#ifdef RT_BIG_ENDIAN
    struct {
        uint16 MODE:2;    /* Use definition MODE_xxx. */
        uint16 iTxBF:1;
        uint16 rsv:1;
        uint16 eTxBF:1;
        uint16 STBC:2;    /*SPACE */
        uint16 ShortGI:1;
        uint16 BW:1;      /*channel bandwidth 20MHz or 40 MHz */
        uint16 MCS:7;     /* MCS */
    } field;
#else
    struct {
        uint16 MCS:7;     /* MCS */
        uint16 BW:1;      /*channel bandwidth 20MHz or 40 MHz */
        uint16 ShortGI:1;
        uint16 STBC:2;    /*SPACE */
        uint16 eTxBF:1;
        uint16 rsv:1;
        uint16 iTxBF:1;
        uint16 MODE:2;    /* Use definition MODE_xxx. */
    } field;
#endif
    uint16 word;
} HTTRANSMIT_SETTING, *PHTTRANSMIT_SETTING;


#ifdef RT_BIG_ENDIAN
typedef    union _BCN_TIME_CFG_STRUC {
    struct {
        uint32 TxTimestampCompensate:8;
        uint32 :3;
        uint32 bBeaconGen:1;       /* Enable beacon generator */
        uint32 bTBTTEnable:1;
        uint32 TsfSyncMode:2;      /* Enable TSF sync, 00: disable, 01: infra mode, 10: ad-hoc mode */
        uint32 bTsfTicking:1;      /* Enable TSF auto counting */
        uint32 BeaconInterval:16;  /* in unit of 1/16 TU */
    } field;
    uint32 word;
} BCN_TIME_CFG_STRUC;
#else
typedef union _BCN_TIME_CFG_STRUC {
    struct {
        uint32 BeaconInterval:16;
        uint32 bTsfTicking:1;
        uint32 TsfSyncMode:2;
        uint32 bTBTTEnable:1;
        uint32 bBeaconGen:1;
        uint32 :3;
        uint32 TxTimestampCompensate:8;
    } field;
    uint32 word;
} BCN_TIME_CFG_STRUC;
#endif

typedef struct _QUEUE_ENTRY {
    struct _QUEUE_ENTRY *Next;
} QUEUE_ENTRY, *PQUEUE_ENTRY;

/* Queue structure */
typedef struct _QUEUE_HEADER {
    PQUEUE_ENTRY Head;
    PQUEUE_ENTRY Tail;
    uint32 Number;
} QUEUE_HEADER, *PQUEUE_HEADER;

typedef enum _RT_802_11_PREAMBLE {
    Rt802_11PreambleLong,
    Rt802_11PreambleShort,
    Rt802_11PreambleAuto
} RT_802_11_PREAMBLE, *PRT_802_11_PREAMBLE;

/*for-wpa value domain of pMacEntry->WpaState  802.1i D3   p.114 */
typedef enum _ApWpaState {
    AS_NOTUSE,                      /* 0 */
    AS_DISCONNECT,              /* 1 */
    AS_DISCONNECTED,          /* 2 */
    AS_INITIALIZE,                 /* 3 */
    AS_AUTHENTICATION,      /* 4 */
    AS_AUTHENTICATION2,    /* 5 */
    AS_INITPMK,                     /* 6 */
    AS_INITPSK,                      /* 7 */
    AS_PTKSTART,                   /* 8 */
    AS_PTKINIT_NEGOTIATING, /* 9 */
    AS_PTKINITDONE,             /* 10 */
    AS_UPDATEKEYS,               /* 11 */
    AS_INTEGRITY_FAILURE,   /* 12 */
    AS_KEYUPDATE,                 /* 13 */
} AP_WPA_STATE;


typedef enum _NDIS_802_11_PRIVACY_FILTER {
    Ndis802_11PrivFilterAcceptAll,
    Ndis802_11PrivFilter8021xWEP
} NDIS_802_11_PRIVACY_FILTER, *PNDIS_802_11_PRIVACY_FILTER;

/* Value domain of pMacEntry->Sst */
typedef enum _Sst {
    SST_NOT_AUTH,   /* 0: equivalent to IEEE 802.11/1999 state 1 */
    SST_AUTH,       /* 1: equivalent to IEEE 802.11/1999 state 2 */
    SST_ASSOC       /* 2: equivalent to IEEE 802.11/1999 state 3 */
} SST;

/* value domain of pMacEntry->AuthState */
typedef enum _AuthState {
    AS_NOT_AUTH,
    AS_AUTH_OPEN,       /* STA has been authenticated using OPEN SYSTEM */
    AS_AUTH_KEY,        /* STA has been authenticated using SHARED KEY */
    AS_AUTHENTICATING   /* STA is waiting for AUTH seq#3 using SHARED KEY */
} AUTH_STATE;


/*============================================================ */
/* ASIC WCID Table definition. */
/*============================================================ */
typedef struct _MAC_TABLE_ENTRY {
    /* WPA/WPA2 4-way database */
    TIMER_T EnqueueStartForPSKTimer;        /* A timer which enqueue EAPoL-Start for triggering PSK SM */
    uint8 EnqueueEapolStartTimerRunning;    /* Enqueue EAPoL-Start for triggering EAP SM */
    uint8 EntryType;                        /*0:Invalid,  1: Client*/
    /*jan for wpa */
    uint8 RSNIE_Len;
    uint8 Addr[MAC_ADDR_LEN];
    uint8 ANonce[LEN_KEY_DESC_NONCE];
    uint8 SNonce[LEN_KEY_DESC_NONCE];
    uint8 R_Counter[LEN_KEY_DESC_REPLAY];
    uint8 PTK[64];
    uint8 ReTryCounter;
#ifdef JCB4WAYTIMER
    TIMER_T RetryTimer;
#endif
    NDIS_802_11_AUTHENTICATION_MODE AuthMode;  /* This should match to whatever microsoft defined */
    NDIS_802_11_WEP_STATUS WepStatus;
    NDIS_802_11_WEP_STATUS GroupKeyWepStatus;
    uint8 WpaState;
    uint8 GTKState;
    uint16 PortSecured;
    NDIS_802_11_PRIVACY_FILTER PrivacyFilter;  /* PrivacyFilter enum for 802.1X */
    CIPHER_KEY PairwiseKey;

    SST Sst;
    AUTH_STATE AuthState;           /* for SHARED KEY authentication state machine used only */
    bool IsReassocSta;              /* Indicate whether this is a reassociation procedure */
    uint16 Aid;
    uint16 CapabilityInfo;
    uint32 StaIdleTimeout;          /* idle timeout per entry */
    uint32 NoDataIdleCount;
    uint16 StationKeepAliveCount;   /* unit: second */

    uint8 MaxSupportedRate;
    HTTRANSMIT_SETTING HTPhyMode, MaxHTPhyMode, MinHTPhyMode;    /* For transmit phy setting in TXWI. */
    uint32 AssocDeadLine;
} MAC_TABLE_ENTRY, *PMAC_TABLE_ENTRY;

typedef struct _MAC_TABLE {
    MAC_TABLE_ENTRY Content[MAX_LEN_OF_MAC_TABLE];
    uint16 Size;
} MAC_TABLE, *PMAC_TABLE;


/*AP configure*/
typedef struct GNU_PACKED _COMMON_CONFIG {
    bool bCountryFlag;
    uint8 CountryCode[3];

    uint8 CountryRegion;         /* Enum of country region, 0:FCC, 1:IC, 2:ETSI, 3:SPAIN, 4:France, 5:MKK, 6:MKK1, 7:Israel */
    uint8 CountryRegionForABand; /* Enum of country region for A band */
    uint8 PhyMode;
    uint8 cfg_wmode;
    uint8 SavedPhyMode;
    uint16 Dsifs;                       /* in units of usec */

    uint16 BeaconPeriod;
    uint8 Channel;
    uint8 CentralChannel;        /* Central Channel when using 40MHz is indicating. not real channel. */

    uint8 SupRate[MAX_LEN_OF_SUPPORTED_RATES];
    uint8 SupRateLen;
    uint8 ExtRate[MAX_LEN_OF_SUPPORTED_RATES];
    uint8 ExtRateLen;
    uint8 DesireRate[MAX_LEN_OF_SUPPORTED_RATES];    /* OID_802_11_DESIRED_RATES */
    uint8 MaxDesiredRate;
    uint8 ExpectedACKRate[MAX_LEN_OF_SUPPORTED_RATES];

    uint32 BasicRateBitmap;         /* backup basic ratebitmap */
    uint32 BasicRateBitmapOld;    /* backup basic ratebitmap */

    uint8 TxPower;                        /* in unit of mW */
    uint32 TxPowerPercentage;    /* 0~100 % */
    uint32 TxPowerDefault;           /* keep for TxPowerPercentage */

    uint32 TxPreamble;                 /* Rt802_11PreambleLong, Rt802_11PreambleShort, Rt802_11PreambleAuto */
    bool bUseZeroToDisableFragment;    /* Microsoft use 0 as disable */
    uint32 UseBGProtection;          /* 0: auto, 1: always use, 2: always not use */
    bool bUseShortSlotTime;        /* 0: disable, 1 - use short slot (9us) */
    bool bEnableTxBurst;              /* 1: enble TX PACKET BURST (when BA is established or AP is not a legacy WMM AP), 0: disable TX PACKET BURST */
    bool bAggregationCapable;     /* 1: enable TX aggregation when the peer supports it */
    bool bPiggyBackCapable;        /* 1: enable TX piggy-back according MAC's version */
    bool bIEEE80211H;                  /* 1: enable IEEE802.11h spec. */
    uint8 RDDurRegion;                  /* Region of radar detection */
    uint32 DisableOLBCDetect;       /* 0: enable OLBC detect; 1 disable OLBC detect */
    uint8 PwrConstraint;
#ifdef BNC_UPDATE_PERIOD         /*Default turn off,  will cause system halt*/
    bool IsUpdateBeacon;
    uint32 BeaconAdjust;
    uint32 BeaconFactor;
    uint32 BeaconRemain;
#endif
} COMMON_CONFIG, *PCOMMON_CONFIG;

typedef union _DESIRED_TRANSMIT_SETTING {
#ifdef RT_BIG_ENDIAN
    struct {
        uint16 rsv:3;
        uint16 FixedTxMode:2;    /* If MCS isn't AUTO, fix rate in CCK, OFDM or HT mode. */
        uint16 PhyMode:4;
        uint16 MCS:7;    /* MCS */
    } field;
#else
    struct {
        uint16 MCS:7;    /* MCS */
        uint16 PhyMode:4;
        uint16 FixedTxMode:2;    /* If MCS isn't AUTO, fix rate in CCK, OFDM or HT mode. */
        uint16 rsv:3;
    } field;
#endif
    uint16 word;
} DESIRED_TRANSMIT_SETTING, *PDESIRED_TRANSMIT_SETTING;


typedef struct _MULTISSID_STRUCT {
    uint8 Bssid[MAC_ADDR_LEN];
    uint8 SsidLen;
    uint8 Ssid[MAX_LEN_OF_SSID+1];
    uint16 CapabilityInfo;

    uint8 MaxStaNum;                /* Limit the STA connection number per BSS */
    uint8 StaCount;

    uint8 RSNIE_Len[2];
    uint8 RSN_IE[2][MAX_LEN_OF_RSNIE];

    NDIS_802_11_AUTHENTICATION_MODE AuthMode;
    NDIS_802_11_WEP_STATUS WepStatus;
    NDIS_802_11_WEP_STATUS GroupKeyWepStatus;
    WPA_MIX_PAIR_CIPHER WpaMixPairCipher;

    uint8 Passphase[CIPHER_TEXT_LEN];
    uint8 PassphaseLen;
    uint8 DefaultKeyId;

    uint8 TxRate;                                      /* RATE_1, RATE_2, RATE_5_5, RATE_11, ... */
    uint8 DesiredRates[MAX_LEN_OF_SUPPORTED_RATES];    /* OID_802_11_DESIRED_RATES */
    uint8 DesiredRatesIndex;
    uint8 MaxTxRate;                                   /* RATE_1, RATE_2, RATE_5_5, RATE_11 */
    DESIRED_TRANSMIT_SETTING DesiredTransmitSetting;   /* Desired transmit setting. this is for reading registry setting only. not useful. */
    HTTRANSMIT_SETTING HTPhyMode, MaxHTPhyMode, MinHTPhyMode;  /* For transmit phy setting in TXWI. */
    uint8 TimBitmaps[WLAN_MAX_NUM_OF_TIM];

    /* WPA */
    uint8 GMK[32];
    uint8 PMK[32];
    uint8 GTK[32];
    uint8 GNonce[32];
    uint8 PortSecured;
    CIPHER_KEY PairwiseKey;
    CIPHER_KEY SharedKey[1][4];     /* STA always use SharedKey[BSS0][0..3] */

    int8 BeaconBuf[MAX_BEACON_SIZE];/* NOTE: BeaconBuf should be 4-byte aligned */
    bool bHideSsid;

    /* used in if beacon send or stop */
    bool bBcnSntReq;
    uint8 BcnBufIdx;
    uint8 PhyMode;

    uint8 TimIELocationInBeacon;
    uint8 CapabilityInfoLocationInBeacon;
} MULTISSID_STRUCT, *PMULTISSID_STRUCT;


typedef struct GNU_PACKED _AP_ADMIN_CONFIG {
    /***********************************************/
    /*Reference to BSSID */
    /***********************************************/
    MULTISSID_STRUCT MBSSID;
    COMMON_CONFIG CommonCfg;
    TXWI_STRUC BeaconTxWI;
    uint8 TXWISize;
    uint16 Aid;
    uint8 DtimCount;     /* 0.. DtimPeriod-1 */
    uint8 DtimPeriod;    /* default = 3 */
#ifdef BNC_UPDATE_PERIOD /*Default turn off,  will cause system halt*/
    uint32 TbttTickCount;/* beacon timestamp work-around */
#endif
    /*MAC Table*/
    MAC_TABLE MacTab;    /* ASIC on-chip WCID entry table.  At TX, ASIC always use key according to this on-chip table. */
    /*chip cap*/
    uint16 WcidHwRsvNum;
    uint8  EntryClientCount;
} AP_ADMIN_CONFIG, *PAP_ADMIN_CONFIG;

int32 iot_ap_init(void) XIP_ATTRIBUTE(".xipsec0");
void iot_ap_startup(void) XIP_ATTRIBUTE(".xipsec0");
void iot_ap_pmk_set(void)  XIP_ATTRIBUTE(".xipsec0");
void iot_apcfg_preinit(void)  XIP_ATTRIBUTE(".xipsec0");
void iot_apcfg_update(uint8 *pSSID, uint8 AuthMode, uint8 *pPassword, uint8 Channel) XIP_ATTRIBUTE(".xipsec0");
void set_entry_phycfg(MAC_TABLE_ENTRY *pEntry) XIP_ATTRIBUTE(".xipsec0");
void store_ap_cfg(void);
bool reset_ap_cfg(void);

int32 AsicSetPreTbttInt(bool enable) XIP_ATTRIBUTE(".xipsec0");
void AsicEnableBssSync(void) XIP_ATTRIBUTE(".xipsec0");
void APMakeBssBeacon(void) XIP_ATTRIBUTE(".xipsec0");
void APUpdateBeaconFrame(void) XIP_ATTRIBUTE(".xipsec0");
void APSendNullFrame(IN puchar pAddr, IN uint8 pwrMgmt) XIP_ATTRIBUTE(".xipsec0");

void iot_ap_mactbl_mgmt();
MAC_TABLE_ENTRY *MacTableLookup(puchar pAddr) XIP_ATTRIBUTE(".xipsec0");
bool MacTableDeleteEntry(IN uint16 wcid) XIP_ATTRIBUTE(".xipsec0");
MAC_TABLE_ENTRY *MacTableInsertEntry(IN  puchar    pAddr,IN bool CleanAll) XIP_ATTRIBUTE(".xipsec0");
void MacTableReset(void) XIP_ATTRIBUTE(".xipsec0");

#ifdef BNC_UPDATE_PERIOD /*Default turn off,  will cause system halt*/
void BeaconUpdateExec(IN void* pParm1, IN void* pParm2);
#endif

#endif //_AP_PUB_H_

