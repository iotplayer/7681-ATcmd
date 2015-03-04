#ifndef    __RTMP_GENERAL_PUB_H__
#define    __RTMP_GENERAL_PUB_H__

#include "types.h"
#include "stddef.h"
#include "rtmp_dot11.h"
#include "queue.h"

typedef struct GNU_PACKED _FRAME_802_11 {
    HEADER_802_11   Hdr;
    uint8            Octet[PKTQU1_SINGLE_BUFFER_SIZE-24];     //For klocwork Building4 ID1~15 in RecoveryMode,  24 is 802.11Header length
}   FRAME_802_11, *PFRAME_802_11;

typedef struct GNU_PACKED _NDIS_802_11_FIXED_IEs {
    uint8 Timestamp[8];
    uint16 BeaconInterval;
    uint16 Capabilities;
} NDIS_802_11_FIXED_IEs, *PNDIS_802_11_FIXED_IEs;

typedef struct GNU_PACKED _NDIS_802_11_VARIABLE_IEs {
    uint8 ElementID;
    uint8 Length;    // Number of bytes in data field
    uint8 data[1];
} NDIS_802_11_VARIABLE_IEs, *PNDIS_802_11_VARIABLE_IEs;

//  HT Capability INFO field in HT Cap IE .
typedef struct GNU_PACKED {
#ifdef RT_BIG_ENDIAN
    uint8    rsv:3;//momi power safe
    uint8    MpduDensity:3;
    uint8    MaxRAmpduFactor:2;
#else
    uint8    MaxRAmpduFactor:2;
    uint8    MpduDensity:3;
    uint8    rsv:3;//momi power safe
#endif /* !RT_BIG_ENDIAN */
} HT_CAP_PARM, *PHT_CAP_PARM;

//  HT Capability INFO field in HT Cap IE .
typedef struct GNU_PACKED {
#ifdef RT_BIG_ENDIAN
    uint16    rsv2:4;
    uint16    RDGSupport:1;    //reverse Direction Grant  support
    uint16    PlusHTC:1;    //+HTC control field support
    uint16    MCSFeedback:2;    //0:no MCS feedback, 2:unsolicited MCS feedback, 3:Full MCS feedback,  1:rsv.
    uint16    rsv:5;//momi power safe
    uint16    TranTime:2;
    uint16    Pco:1;
#else
    uint16    Pco:1;
    uint16    TranTime:2;
    uint16    rsv:5;//momi power safe
    uint16    MCSFeedback:2;    //0:no MCS feedback, 2:unsolicited MCS feedback, 3:Full MCS feedback,  1:rsv.
    uint16    PlusHTC:1;    //+HTC control field support
    uint16    RDGSupport:1;    //reverse Direction Grant  support
    uint16    rsv2:4;
#endif /* RT_BIG_ENDIAN */
} EXT_HT_CAP_INFO, *PEXT_HT_CAP_INFO;

//  HT Beamforming field in HT Cap IE .
typedef struct GNU_PACKED _HT_BF_CAP {
#ifdef RT_BIG_ENDIAN
    uint32    rsv:3;
    uint32    ChanEstimation:2;
    uint32    CSIRowBFSup:2;
    uint32    ComSteerBFAntSup:2;
    uint32    NoComSteerBFAntSup:2;
    uint32    CSIBFAntSup:2;
    uint32    MinGrouping:2;
    uint32    ExpComBF:2;
    uint32    ExpNoComBF:2;
    uint32    ExpCSIFbk:2;
    uint32    ExpComSteerCapable:1;
    uint32    ExpNoComSteerCapable:1;
    uint32    ExpCSICapable:1;
    uint32    Calibration:2;
    uint32    ImpTxBFCapable:1;
    uint32    TxNDPCapable:1;
    uint32    RxNDPCapable:1;
    uint32    TxSoundCapable:1;
    uint32    RxSoundCapable:1;
    uint32    TxBFRecCapable:1;
#else
    uint32    TxBFRecCapable:1;
    uint32    RxSoundCapable:1;
    uint32    TxSoundCapable:1;
    uint32    RxNDPCapable:1;
    uint32    TxNDPCapable:1;
    uint32    ImpTxBFCapable:1;
    uint32    Calibration:2;
    uint32    ExpCSICapable:1;
    uint32    ExpNoComSteerCapable:1;
    uint32    ExpComSteerCapable:1;
    uint32    ExpCSIFbk:2;
    uint32    ExpNoComBF:2;
    uint32    ExpComBF:2;
    uint32    MinGrouping:2;
    uint32    CSIBFAntSup:2;
    uint32    NoComSteerBFAntSup:2;
    uint32    ComSteerBFAntSup:2;
    uint32    CSIRowBFSup:2;
    uint32    ChanEstimation:2;
    uint32    rsv:3;
#endif // RT_BIG_ENDIAN //
} HT_BF_CAP, *PHT_BF_CAP;

//  HT antenna selection field in HT Cap IE .
typedef struct GNU_PACKED _HT_AS_CAP {
#ifdef RT_BIG_ENDIAN
    uint8    rsv:1;
    uint8    TxSoundPPDU:1;
    uint8    RxASel:1;
    uint8    AntIndFbk:1;
    uint8    ExpCSIFbk:1;
    uint8    AntIndFbkTxASEL:1;
    uint8    ExpCSIFbkTxASEL:1;
    uint8    AntSelect:1;
#else
    uint8    AntSelect:1;
    uint8    ExpCSIFbkTxASEL:1;
    uint8    AntIndFbkTxASEL:1;
    uint8    ExpCSIFbk:1;
    uint8    AntIndFbk:1;
    uint8    RxASel:1;
    uint8    TxSoundPPDU:1;
    uint8    rsv:1;
#endif // RT_BIG_ENDIAN //
} HT_AS_CAP, *PHT_AS_CAP;

typedef struct GNU_PACKED {
#ifdef RT_BIG_ENDIAN
    uint16    LSIGTxopProSup:1;
    uint16    Forty_Mhz_Intolerant:1;
    uint16    PSMP:1;
    uint16    CCKmodein40:1;
    uint16    AMsduSize:1;
    uint16    DelayedBA:1;    //rt2860c not support
    uint16    RxSTBC:2;
    uint16    TxSTBC:1;
    uint16    ShortGIfor40:1;    //for40MHz
    uint16    ShortGIfor20:1;
    uint16    GF:1;            //green field
    uint16    MimoPs:2;       //momi power safe
    uint16    ChannelWidth:1;
    uint16    AdvCoding:1;
#else
    uint16    AdvCoding:1;
    uint16    ChannelWidth:1;
    uint16    MimoPs:2;       //momi power safe
    uint16    GF:1;            //green field
    uint16    ShortGIfor20:1;
    uint16    ShortGIfor40:1;    //for40MHz
    uint16    TxSTBC:1;
    uint16    RxSTBC:2;
    uint16    DelayedBA:1;    //rt2860c not support
    uint16    AMsduSize:1;    // only support as zero
    uint16    CCKmodein40:1;
    uint16    PSMP:1;
    uint16    Forty_Mhz_Intolerant:1;
    uint16    LSIGTxopProSup:1;
#endif    /* !RT_BIG_ENDIAN */
} HT_CAP_INFO, *PHT_CAP_INFO;

typedef struct GNU_PACKED _HT_CAPABILITY_IE {
    HT_CAP_INFO        HtCapInfo;
    HT_CAP_PARM        HtCapParm;
//    HT_MCS_SET        HtMCSSet;
    uint8            MCSSet[16];
    EXT_HT_CAP_INFO    ExtHtCapInfo;
    HT_BF_CAP        TxBFCap;    // beamforming cap. rt2860c not support beamforming.
    HT_AS_CAP        ASCap;    //antenna selection.
} HT_CAPABILITY_IE, *PHT_CAPABILITY_IE;

typedef struct GNU_PACKED _EXT_CAP_INFO_ELEMENT {
#ifdef RT_BIG_ENDIAN
#ifdef DOT11V_WNM_SUPPORT
    uint32    rsv6:6;
    uint32    DMSSupport:1;
    uint32    rsv5:6;
    uint32    BssTransitionManmt:1;
    uint32    rsv4:1;
    uint32    WNMSleepSupport:1;/*bit 17*/
    uint32    TFSSupport:1;/*bit 16*/
    uint32    rsv3:4;
    uint32    FMSSupport:1;/*bit 11*/
    uint32    rsv2:7;
    uint32    ExtendChannelSwitch:1;
    uint32    rsv:1;
    uint32    BssCoexistMgmtSupport:1;
#else // DOT11V_WNM_SUPPORT //
#ifdef DOT11Z_TDLS_SUPPORT
    uint8    TdlsChannelSwitching:1;
    uint8    TdlsPSM:1;
    uint8    TdlsUAPSD:1;
    uint8    rsv2:2;
#else
    uint8    rsv2:5;
#endif // DOT11Z_TDLS_SUPPORT //
    uint8    ExtendChannelSwitch:1;
    uint8    rsv:1;
    uint8    BssCoexistMgmtSupport:1;
#endif // DOT11V_WNM_SUPPORT //
#else
#ifdef DOT11V_WNM_SUPPORT
    uint32    BssCoexistMgmtSupport:1;
    uint32    rsv:1;
    uint32    ExtendChannelSwitch:1;
    uint32    rsv2:7;
    uint32    FMSSupport:1;/*bit 11*/
    uint32    rsv3:4;
    uint32    TFSSupport:1;/*bit 16*/
    uint32    WNMSleepSupport:1;/*bit 17*/
    uint32    rsv4:1;
    uint32    BssTransitionManmt:1;
    uint32    rsv5:6;
    uint32    DMSSupport:1;
    uint32    rsv6:6;
#else //DOT11V_WNM_SUPPORT
    uint8    BssCoexistMgmtSupport:1;
    uint8    rsv:1;
    uint8    ExtendChannelSwitch:1;
#ifdef DOT11Z_TDLS_SUPPORT
    uint8    rsv2:2;
    uint8    TdlsUAPSD:1;
    uint8    TdlsPSM:1;
    uint8    TdlsChannelSwitching:1;
#else
    uint8    rsv2:5;
#endif // DOT11Z_TDLS_SUPPORT //
#endif // DOT11V_WNM_SUPPORT //
#endif // RT_BIG_ENDIAN //
} EXT_CAP_INFO_ELEMENT, *PEXT_CAP_INFO_ELEMENT;

//   field in Addtional HT Information IE .
typedef struct GNU_PACKED {
#ifdef RT_BIG_ENDIAN
    uint8    SerInterGranu:3;
    uint8    S_PSMPSup:1;
    uint8    RifsMode:1;
    uint8    RecomWidth:1;
    uint8    ExtChanOffset:2;
#else
    uint8    ExtChanOffset:2;
    uint8    RecomWidth:1;
    uint8    RifsMode:1;
    uint8    S_PSMPSup:1;     //Indicate support for scheduled PSMP
    uint8    SerInterGranu:3;     //service interval granularity
#endif
} ADD_HTINFO, *PADD_HTINFO;

typedef struct GNU_PACKED {
#ifdef RT_BIG_ENDIAN
    uint16    rsv2:11;
    uint16    OBSS_NonHTExist:1;
    uint16    rsv:1;
    uint16    NonGfPresent:1;
    uint16    OperaionMode:2;
#else
    uint16    OperaionMode:2;
    uint16    NonGfPresent:1;
    uint16    rsv:1;
    uint16    OBSS_NonHTExist:1;
    uint16    rsv2:11;
#endif
} ADD_HTINFO2, *PADD_HTINFO2;


// TODO: Need sync with spec about the definition of StbcMcs. In Draft 3.03, it's reserved.
typedef struct GNU_PACKED {
#ifdef RT_BIG_ENDIAN
    uint16    rsv:4;
    uint16    PcoPhase:1;
    uint16    PcoActive:1;
    uint16    LsigTxopProt:1;
    uint16    STBCBeacon:1;
    uint16    DualCTSProtect:1;
    uint16    DualBeacon:1;
    uint16    StbcMcs:6;
#else
    uint16    StbcMcs:6;
    uint16    DualBeacon:1;
    uint16    DualCTSProtect:1;
    uint16    STBCBeacon:1;
    uint16    LsigTxopProt:1;    // L-SIG TXOP protection full support
    uint16    PcoActive:1;
    uint16    PcoPhase:1;
    uint16    rsv:4;
#endif // RT_BIG_ENDIAN //
} ADD_HTINFO3, *PADD_HTINFO3;

typedef struct  GNU_PACKED {
    uint8                ControlChan;
    ADD_HTINFO            AddHtInfo;
    ADD_HTINFO2            AddHtInfo2;
    ADD_HTINFO3            AddHtInfo3;
    uint8                MCSSet[16];        // Basic MCS set
} ADD_HT_INFO_IE, *PADD_HT_INFO_IE;

// structure to store channel property
typedef struct GNU_PACKED _CHANNEL_PROPERTY {
    uint8      Channel;
    uint8      Flag;
} CHANNEL_PROPERTY;

typedef struct GNU_PACKED _SECURITY_IE {
    puchar pWpaGroupOui;   /* WPA Group cipher */
    puchar pWpaUcastOui;   /* WPA pairwise cipher */
    puchar pWpaAkmOui;     /* WPA AKM suite */
    uint8 WpaGroupCnt;
    uint8 WpaUcastCnt;
    uint8 WpaAkmCnt;

    puchar pRsnGroupOui;   /* WPA2 Group cipher */
    puchar pRsnUcastOui;   /* WPA2 pairwise cipher */
    puchar pRsnAkmOui;     /* WPA2 AKM suite */
    uint8 RsnGroupCnt;
    uint8 RsnUcastCnt;
    uint8 RsnAkmCnt;
} SECURITY_IE;

/* definition to support multiple BSSID */
#define BSS0                     0

#define MAX_CUSTOM_LEN           128
#define RX_FCEINFO_SIZE          4  /* descriptor 4th DWord */
#define RXINFO_SIZE              4
#define TXINFO_SIZE              4
#define TXWI_SIZE               20
#ifdef __MT7681
#define RXWI_SIZE                 24 // 6 * 4DWORD
#endif

/* All PHY rate summary in TXD */
/* Preamble MODE in TxD */
#define MODE_CCK                 0
#define MODE_OFDM                1
#define MODE_HTMIX               2
#define MODE_HTGREENFIELD        3
#define MODE_VHT                 4

/* definition RSSI Number */
#define RSSI_0                     0
#define RSSI_1                     1
#define RSSI_2                     2


/* value domain of pAd->StaCfg.PortSecured. 802.1X controlled port definition */
#define WPA_802_1X_PORT_SECURED     1
#define WPA_802_1X_PORT_NOT_SECURED 2

/* ================================================================ */
/*  MAC TABLE definition */
/* ================================================================ */
#define HASH_TABLE_SIZE                 256    /* Size of hash tab must be power of 2. */

#define MAC_ADDR_IS_GROUP(Addr)       (((Addr[0]) & 0x01))
#define MAC_ADDR_HASH(Addr)            (Addr[0] ^ Addr[1] ^ Addr[2] ^ Addr[3] ^ Addr[4] ^ Addr[5])
#define MAC_ADDR_HASH_INDEX(Addr)      (MAC_ADDR_HASH(Addr) & (HASH_TABLE_SIZE - 1))
#define TID_MAC_HASH(Addr,TID)         (TID^Addr[0] ^ Addr[1] ^ Addr[2] ^ Addr[3] ^ Addr[4] ^ Addr[5])
#define TID_MAC_HASH_INDEX(Addr,TID)   (TID_MAC_HASH(Addr,TID) & (HASH_TABLE_SIZE - 1))

/* MBSSID definition */
#define ENTRY_NOT_FOUND                0xFF

#define MAC_TABLE_STA_ALIVE_TIME    30            /* unit: sec */

#define MAC_TABLE_AGEOUT_TIME        300            /* unit: sec */
#define MAC_TABLE_MIN_AGEOUT_TIME    60            /* unit: sec */
#define MAC_TABLE_ASSOC_TIMEOUT        5            /* unit: sec */
#define MAC_TABLE_FULL(Tab)            ((Tab).size == MAX_LEN_OF_MAC_TABLE)


#define NDIS_STATUS                    int32
#define NDIS_STATUS_SUCCESS         0x00
#define NDIS_STATUS_FAILURE         0x01
#define NDIS_STATUS_INVALID_DATA    0x02
#define NDIS_STATUS_RESOURCES       0x03


/* value domain of 802.11 MGMT frame's FC.subtype, which is b7..4 of the 1st-byte of MAC header */
#define SUBTYPE_ASSOC_REQ           0
#define SUBTYPE_ASSOC_RSP           1
#define SUBTYPE_REASSOC_REQ         2
#define SUBTYPE_REASSOC_RSP         3
#define SUBTYPE_PROBE_REQ           4
#define SUBTYPE_PROBE_RSP           5
#define SUBTYPE_BEACON              8
#define SUBTYPE_ATIM                9
#define SUBTYPE_DISASSOC            10
#define SUBTYPE_AUTH                11
#define SUBTYPE_DEAUTH              12
#define SUBTYPE_ACTION              13
#define SUBTYPE_ACTION_NO_ACK       14

/* value domain of 802.11 CNTL frame's FC.subtype, which is b7..4 of the 1st-byte of MAC header */
#define SUBTYPE_WRAPPER                7
#define SUBTYPE_BLOCK_ACK_REQ       8
#define SUBTYPE_BLOCK_ACK            9
#define SUBTYPE_PS_POLL             10
#define SUBTYPE_RTS                 11
#define SUBTYPE_CTS                 12
#define SUBTYPE_ACK                 13
#define SUBTYPE_CFEND               14
#define SUBTYPE_CFEND_CFACK         15

/* value domain of 802.11 DATA frame's FC.subtype, which is b7..4 of the 1st-byte of MAC header */
#define SUBTYPE_DATA                0
#define SUBTYPE_DATA_CFACK          1
#define SUBTYPE_DATA_CFPOLL         2
#define SUBTYPE_DATA_CFACK_CFPOLL   3
#define SUBTYPE_NULL_FUNC           4
#define SUBTYPE_CFACK               5
#define SUBTYPE_CFPOLL              6
#define SUBTYPE_CFACK_CFPOLL        7
#define SUBTYPE_QDATA               8
#define SUBTYPE_QDATA_CFACK         9
#define SUBTYPE_QDATA_CFPOLL        10
#define SUBTYPE_QDATA_CFACK_CFPOLL  11
#define SUBTYPE_QOS_NULL            12
#define SUBTYPE_QOS_CFACK           13
#define SUBTYPE_QOS_CFPOLL          14
#define SUBTYPE_QOS_CFACK_CFPOLL    15

/* Reason code definitions */
#define REASON_RESERVED                 0
#define REASON_UNSPECIFY                1
#define REASON_NO_LONGER_VALID          2
#define REASON_DEAUTH_STA_LEAVING       3
#define REASON_DISASSOC_INACTIVE        4
#define REASON_DISASSPC_AP_UNABLE       5
#define REASON_CLS2ERR                  6
#define REASON_CLS3ERR                  7
#define REASON_DISASSOC_STA_LEAVING     8
#define REASON_STA_REQ_ASSOC_NOT_AUTH   9
#define REASON_INVALID_IE               13
#define REASON_MIC_FAILURE              14
#define REASON_4_WAY_TIMEOUT            15
#define REASON_GROUP_KEY_HS_TIMEOUT     16
#define REASON_IE_DIFFERENT             17
#define REASON_MCIPHER_NOT_VALID        18
#define REASON_UCIPHER_NOT_VALID        19
#define REASON_AKMP_NOT_VALID           20
#define REASON_UNSUPPORT_RSNE_VER       21
#define REASON_INVALID_RSNE_CAP         22
#define REASON_8021X_AUTH_FAIL          23
#define REASON_CIPHER_SUITE_REJECTED    24
#define REASON_DECLINED                 37

/* ================================================================ */
/* Tx /    Rx / Mgmt ring descriptor definition */
/* ================================================================ */

/* the following PID values are used to mark outgoing frame type in TXD->PID so that */
/* proper TX statistics can be collected based on these categories */
/* b3-2 of PID field - */
#define PID_MGMT                0x05
#define PID_BEACON                0x0c
#define PID_DATA_NORMALUCAST    0x02
#define PID_DATA_AMPDU             0x04
#define PID_DATA_NO_ACK            0x08
#define PID_DATA_NOT_NORM_ACK    0x03
/* value domain of pTxD->HostQId (4-bit: 0~15) */
#define QID_AC_BK               1   /* meet ACI definition in 802.11e */
#define QID_AC_BE               0   /* meet ACI definition in 802.11e */
#define QID_AC_VI               2
#define QID_AC_VO               3
#define QID_HCCA                4
#define NUM_OF_TX_RING          5
#define QID_CTRL                9
#define QID_MGMT                13
#define QID_RX                  14
#define QID_OTHER               15

//============================================================
// ASIC WCID Table definition.
//============================================================
#define BSSID_WCID            1    // in infra mode, always put bssid with this WCID 
#define MCAST_WCID            0x0
#define RESERVED_WCID        0xff

/* power status related definitions */
#define PWR_ACTIVE                      0
#define PWR_SAVE                        1
#define PWR_MMPS                        2

/* IE code */
#define IE_SSID                         0
#define IE_SUPP_RATES                   1
#define IE_FH_PARM                      2
#define IE_DS_PARM                      3
#define IE_CF_PARM                      4
#define IE_TIM                          5
#define IE_IBSS_PARM                    6
#define IE_COUNTRY                      7    /* 802.11d */
#define IE_802_11D_REQUEST              10    /* 802.11d */
#define IE_QBSS_LOAD                    11    /* 802.11e d9 */
#define IE_EDCA_PARAMETER               12    /* 802.11e d9 */
#define IE_TSPEC                        13    /* 802.11e d9 */
#define IE_TCLAS                        14    /* 802.11e d9 */
#define IE_SCHEDULE                     15    /* 802.11e d9 */
#define IE_CHALLENGE_TEXT               16
#define IE_POWER_CONSTRAINT             32    /* 802.11h d3.3 */
#define IE_POWER_CAPABILITY             33    /* 802.11h d3.3 */
#define IE_TPC_REQUEST                  34    /* 802.11h d3.3 */
#define IE_TPC_REPORT                   35    /* 802.11h d3.3 */
#define IE_SUPP_CHANNELS                36    /* 802.11h d3.3 */
#define IE_CHANNEL_SWITCH_ANNOUNCEMENT  37    /* 802.11h d3.3 */
#define IE_MEASUREMENT_REQUEST          38    /* 802.11h d3.3 */
#define IE_MEASUREMENT_REPORT           39    /* 802.11h d3.3 */
#define IE_QUIET                        40    /* 802.11h d3.3 */
#define IE_IBSS_DFS                     41    /* 802.11h d3.3 */
#define IE_ERP                          42    /* 802.11g */
#define IE_TS_DELAY                     43    /* 802.11e d9 */
#define IE_TCLAS_PROCESSING             44    /* 802.11e d9 */
#define IE_QOS_CAPABILITY               46    /* 802.11e d6 */
#define IE_HT_CAP                       45    /* 802.11n d1. HT CAPABILITY. ELEMENT ID TBD */
#define IE_AP_CHANNEL_REPORT            51    /* 802.11k d6 */
#define IE_HT_CAP2                      52    /* 802.11n d1. HT CAPABILITY. ELEMENT ID TBD */
#define IE_RSN                          48    /* 802.11i d3.0 */
#define IE_WPA2                         48    /* WPA2 */
#define IE_EXT_SUPP_RATES               50    /* 802.11g */
#define IE_TIMEOUT_INTERVAL             56  /* 802.11w */
#define IE_SUPP_REG_CLASS               59    /* 802.11y. Supported regulatory classes. */
#define IE_EXT_CHANNEL_SWITCH_ANNOUNCEMENT    60    /* 802.11n */
#define IE_ADD_HT                       61    /* 802.11n d1. ADDITIONAL HT CAPABILITY. ELEMENT ID TBD */
#define IE_ADD_HT2                      53    /* 802.11n d1. ADDITIONAL HT CAPABILITY. ELEMENT ID TBD */


/* For 802.11n D3.03 */
/*#define IE_NEW_EXT_CHA_OFFSET               62    // 802.11n d1. New extension channel offset elemet */
#define IE_SECONDARY_CH_OFFSET            62    /* 802.11n D3.03        Secondary Channel Offset element */
#define IE_WAPI                            68    /* WAPI information element. Same as Bss Ac Access Dealy Element. */
#define IE_2040_BSS_COEXIST             72    /* 802.11n D3.0.3 */
#define IE_2040_BSS_INTOLERANT_REPORT   73    /* 802.11n D3.03 */
#define IE_OVERLAPBSS_SCAN_PARM         74    /* 802.11n D3.03 */
#define IE_CHANNEL_USAGE                97    /* Cisco advertises suggested channel using this IE. */
#define IE_TIME_ZONE                    98    /* 802.11V */
#define IE_INTERWORKING                    107 /* 802.11u */
#define IE_ADVERTISEMENT_PROTO            108 /* 802.11u */
#define IE_QOS_MAP_SET                    110 /* 802.11u */
#define IE_ROAMING_CONSORTIUM            111 /* 802.11u */

#define IE_EXT_CAPABILITY               127 // 802.11n D3.03 
#define IE_VENDOR_SPECIFIC              221

/* value domain of 802.11 header FC.Tyte, which is b3..b2 of the 1st-byte of MAC header */
#define BTYPE_MGMT                  0
#define BTYPE_CNTL                  1
#define BTYPE_DATA                  2

/* Action Category */
#define CATEGORY_SPECTRUM            0
#define CATEGORY_QOS                 1
#define CATEGORY_DLS                2
#define CATEGORY_BA                    3

/* Spectrum  Action field value 802.11h 7.4.1 */
#define SPEC_MRQ                    0    /* Request */
#define SPEC_MRP                    1    /* Report */
#define SPEC_TPCRQ                    2
#define SPEC_TPCRP                    3
#define SPEC_CHANNEL_SWITCH            4

/* BA  Action field value */
#define ADDBA_REQ                    0
#define ADDBA_RESP                    1
#define DELBA                       2

/* BA Policy subfiled value in ADDBA frame */
#define IMMED_BA     1
#define DELAY_BA    0

#define CIPHER_NONE                 0
#define CIPHER_WEP64                1
#define CIPHER_WEP128               2
#define CIPHER_TKIP                 3
#define CIPHER_AES                  4
#define CIPHER_CKIP64               5
#define CIPHER_CKIP128              6
#define CIPHER_CKIP152              7
#define CIPHER_SMS4                    8

/* Cipher type in WPA/RSN IE */
#define IE_CIPHER_TYPE_TKIP         2
#define IE_CIPHER_TYPE_CCMP         4


typedef struct GNU_PACKED _CIPHER_KEY {
    uint8   Key[16];                // 128 bits max
    uint8   TxMic[8];
    uint8   RxMic[8];
    uint8   TxTsc[16];               // TSC value. Change it from 48bit to 128bit
    uint8   RxTsc[16];               // TSC value. Change it from 48bit to 128bit
    uint8   CipherAlg;               // 0:none, 1:WEP64, 2:WEP128, 3:TKIP, 4:AES, 5:CKIP64, 6:CKIP128
    uint8   KeyLen;                 // Key length for each key, 0: entry is invalid
    uint8   BssId[6];
    uint8   Type;                   // Indicate Pairwise/Group when reporting MIC error
} CIPHER_KEY, *PCIPHER_KEY;



/***************************************************************************
  *    Packet Header definition
  **************************************************************************/
#define SNAP_LENGTH             6
#define UDP_HEADER_LENGTH       8
#define IP_PROTO_TCP            6
#define IP_PROTO_UDP            17
#define IP_PROTO_ICMPv6         58
#define UDP_DPORT_FIELD_OFFSET  2
#define UDP_LENGTH_FIELD_OFFSET 4
#define TCP_SPORT_FIELD_OFFSET  0
#define TCP_DPORT_FIELD_OFFSET  2
#define TCP_FLAG_FIELD_OFFSET   13
#define MAGIC_FRAME_LENGTH      102
#define UDP_MAGIC_FRAME_LENGTH  110
#define MAGIC_FRAME_MAC_TIMES   16
#define ETHER_TYPE_ARP          0x0806
#define ETHER_TYPE_IPv4         0x0800
#define ETHER_TYPE_IPv6         0x86DD
#define LENGTH_802_11           24
#define LENGTH_802_3            14
#define LENGTH_802_3_TYPE       2
#define LENGTH_802_1_H          8
#define LENGTH_EAPOL_H          4

#define PKT_TYPE_NONE           0
#define PKT_TYPE_UCAST          1
#define PKT_TYPE_MCAST          2
#define PKT_TYPE_BCAST          3

#define ARP_OPCODE_OFFSET           6
#define ARP_SA_OFFSET               8
#define ARP_SPA_OFFSET              14
#define ARP_DA_OFFSET               18
#define ARP_TPA_OFFSET              24
#define ARP_MSG_SIZE                28

#define IPV4_VER_IHL_OFFSET         0
#define IPV4_LENGTH_OFFSET            2
#define IPV4_PROTO_OFFSET           9
#define IPV4_SRCIP_OFFSET           12
#define IPV4_DSTIP_OFFSET           16
#define IPV4_HEADER_LEN_MAX         24
#define IPV6_LENGTH_OFFSET            4
#define IPV6_PROTO_OFFSET           6
#define IPV4_HEADER_SIZE              20
#define IPV6_HEADER_SIZE              40
#define IPV6_HEADER_EXT_SIZE        24
#define IPV6_ADDR_LEN               16

/* IPv6 next header define */
#define NEXTHDR_HOP            0    /* Hop-by-hop option header. */
#define NEXTHDR_TCP            6    /* TCP segment. */
#define NEXTHDR_UDP            17    /* UDP message. */
#define NEXTHDR_IPV6        41    /* IPv6 in IPv6 */
#define NEXTHDR_ROUTING        43    /* Routing header. */
#define NEXTHDR_FRAGMENT    44    /* Fragmentation/reassembly header. */
#define NEXTHDR_ESP            50    /* Encapsulating security payload. */
#define NEXTHDR_AUTH        51    /* Authentication header. */
#define NEXTHDR_ICMP        58    /* ICMP for IPv6. */
#define NEXTHDR_NONE        59    /* No next header */
#define NEXTHDR_DEST        60    /* Destination options header. */
#define NEXTHDR_MOBILITY    135    /* Mobility header. */

/* ICMPv6 type */
#define ICMPV6_NEIGHBOR_SOLICITATION     135
#define ICMPV6_NEIGHBOR_ADVERTISEMENT    136

/*ICMPv6 flag define */
#define ICMPV6_FLAG_SOLICITIED           0x40000000

/* TCP flag define */
#define TCP_FLAG_SYN                     0x02




/***************************************************************************
  *    Tx Path software control block related data structures
  **************************************************************************/
typedef struct _TX_BLK_ {
    // Following structure used for the characteristics of a specific packet.
    pBD_t                pPacket;
    puchar                pSrcBufHeader;                // Reference to the head of sk_buff->data
    puchar                pSrcBufData;                // Reference to the sk_buff->data, will changed depends on hanlding progresss
    uint32                SrcBufLen;                    // Length of packet payload which not including Layer 2 header
    //uint8                HeaderBuf[128]                     // TempBuffer for TX_WI + 802.11 Header + padding + AMSDU SubHeader + LLC/SNAP
    uint8                HeaderBuf[152];                // jinchuan.bao if probe reponse length >127 (include WPA, RSN IE). the RSN IE is incorrect in the air

    puchar                pExtraLlcSnapEncap;            // NULL means no extra LLC/SNAP is required
    uint8                CipherAlg;                    // cipher alogrithm
    PCIPHER_KEY            pKey;
    uint8                KeyIdx;                        // Indicate the transmit key index
    uint8                Wcid;                        // The MAC entry associated to this packet
    uint8                MpduHeaderLen;                // 802.11 header length NOT including the padding
    uint8                HdrPadLen;                    // recording Header Padding Length;
} TX_BLK, *PTX_BLK;


#ifdef __MT7681
typedef struct GNU_PACKED _RXWI_STRUC {
    /* Word 0 */
    uint32      WCID:8;
    uint32      KeyIdx:2;
    uint32      BSSIdx:3;
    uint32      UDF:3;
    uint32      MPDUtotalByteCount:12;
    uint32      tid:4;

    /* Word 1 */
    uint32      FRAG:4;
    uint32      SEQUENCE:12;
    uint32      mcs:7;
    uint32      bw:1;
    uint32      sgi:1;
    uint32      stbc:2;
    uint32      eTxBF:1;     /* eTxBF enable */
    uint32      Sounding:1;  /* Sounding enable */
    uint32      iTxBF:1;     /* iTxBF enable */
    uint32      PHYMode:2;   /* 1: this RX frame is unicast to me */

    /*Word2 */
    uint32      RSSI0:8;
    uint32      RSSI1:8;
    uint32      RSSI2:8;
    uint32      rsv1:8;

    /*Word3 */
    uint32      SNR0:8;
    uint32      SNR1:8;
    uint32      SNR2:8;
    uint32      FOFFSET:8;

    /* For Expert Antenna */
    uint32      RX0_RSSI:8;
    uint32      RX1_RSSI:8;
    uint32      STS0_SNR:8;
    uint32      STS1_SNR:8;

    /* Word 5 */
    /* For Exper Antenna */
    uint32      EANT_ID:8;
    uint32      rsv4:24;
}   RXWI_STRUC, *PRXWI_STRUC;

typedef struct GNU_PACKED _TXWI_STRUC {
    /* Word 0 */
    /* ex: 00 03 00 40 means txop = 3, PHYMODE = 1 */
    uint32      FRAG:1;         /* 1 to inform TKIP engine this is a fragment. */
    uint32      MIMOps:1;       /* the remote peer is in dynamic MIMO-PS mode */
    uint32      CFACK:1;
    uint32      TS:1;

    uint32      AMPDU:1;
    uint32      MpduDensity:3;
    uint32      txop:2;         /*FOR "THIS" frame. 0:HT TXOP rule , 1:PIFS TX ,2:Backoff, 3:sifs only when previous frame exchange is successful. */
    uint32      NDPSndRate:2;   /* 0 : MCS0, 1: MCS8, 2: MCS16, 3: reserved */
    uint32      NDPSndBW:1;     /* NDP sounding BW */
    uint32      Autofallback:1; /* TX rate auto fallback disable */
    uint32      TXRPT:1;
    uint32      rsv:1;
    uint32      MCS:7;
    uint32      BW:1;           /*channel bandwidth 20MHz or 40 MHz */
    uint32      ShortGI:1;
    uint32      STBC:2;         /* 1: STBC support MCS =0-7,   2,3 : RESERVE */
    uint32      eTxBF:1;        /* eTxBF enable */
    uint32      Sounding:1;     /* Sounding enable */
    uint32      iTxBF:1;        /* iTxBF enable */
    uint32      PHYMODE:2;

    /* Word1 */
    /* ex:  1c ff 38 00 means ACK=0, BAWinSize=7, MPDUtotalByteCnt = 0x38 */
    uint32      ACK:1;
    uint32      NSEQ:1;
    uint32      BAWinSize:6;
    uint32      WirelessCliID:8;
    uint32      MPDUtotalByteCount:12;
    uint32      PacketId:4;
    /*Word2 */
    uint32      IV;
    /*Word3 */
    uint32      EIV;

    /* Word 4 */
    /* For Expert Antenna */
    uint32      EncodedAntID:8;
    uint32      TxStreamMode:8;
    uint32      TxPwrAdj:4;
    uint32      CCP:1;
    uint32      Reserved:11;
}   TXWI_STRUC, *PTXWI_STRUC;
#endif /* __MT7681 */

typedef    struct    GNU_PACKED _RXINFO_STRUC {
    uint32        BA:1;
    uint32        DATA:1;
    uint32        NULLDATA:1;
    uint32        FRAG:1;
    uint32        U2M:1;              // 1: this RX frame is unicast to me
    uint32        Mcast:1;            // 1: this is a multicast frame
    uint32        Bcast:1;            // 1: this is a broadcast frame
    uint32        MyBss:1;              // 1: this frame belongs to the same BSSID
    uint32        Crc:1;              // 1: CRC error
    uint32        IcvErr:1;
    uint32        MicErr:1;
    uint32        AMSDU:1;            // rx with 802.3 header, not 802.11 header.
    uint32        HTC:1;
    uint32        RSSI:1;
    uint32        L2PAD:1;
    uint32        AMPDU:1;            // To be moved
    uint32        Decrypted:1;
    uint32        Bssidx3:1;
    uint32        WapiKeyId:1;
    uint32        PaddingLen:3;
    uint32        Resrv:6;
    uint32        TcpChksumPass:1;
    uint32        IpChksumPass:1;
    uint32        TcpChksumErr:1;
    uint32        IpChksumErr:1;
}    RXINFO_STRUC, *PRXINFO_STRUC;

/***************************************************************************
  *    Rx Path software control block related data structures
  **************************************************************************/
typedef struct _RX_BLK_ {
    PRXINFO_STRUC        pRxINFO;
    PRXWI_STRUC            pRxWI;
    PHEADER_802_11        pHeader;
    void*                pRxPacket;
    uint8                *pData;
    uint16                DataSize;
    uint16                Flags;
    uint8                UserPriority;    // for calculate TKIP MIC using
} RX_BLK;


typedef struct GNU_PACKED _RX_RSSI_INFO {
    int8 LastRssi0;        /* last received RSSI */
    int8 AvgRssi0;        /* last 8 frames' average RSSI */
    int16 AvgRssi0X8;    /* sum of last 8 frames' RSSI */
} RX_RSSI_INFO, *PRX_RSSI_INFO;


#define NdisMoveMemory(Destination, Source, Length) memmove(Destination, Source, Length)
//#define NdisCopyMemory(Destination, Source, Length) memcpy(Destination, Source, Length)
#define NdisZeroMemory(Destination, Length)         memset(Destination, 0, Length)
#define NdisFillMemory(Destination, Length, Fill)   memset(Destination, Fill, Length)
#define NdisEqualMemory(Source1, Source2, Length)   (!memcmp(Source1, Source2, Length))


#define    CONV_ARRARY_TO_UINT16(_V)    ((_V[0]<<8) | (_V[1]))
#define    SET_UINT16_TO_ARRARY(_V, _LEN)        \
{                                            \
    _V[0] = ((uint16)_LEN) >> 8;            \
    _V[1] = ((uint16)_LEN & 0xFF);            \
}

#define    INC_UINT16_TO_ARRARY(_V, _LEN)        \
{                                            \
    uint16    var_len;                        \
                                            \
    var_len = (_V[0]<<8) | (_V[1]);            \
    var_len += _LEN;                        \
                                            \
    _V[0] = (var_len & 0xFF00) >> 8;        \
    _V[1] = (var_len & 0xFF);                \
}

#define    ADD_ONE_To_64BIT_VAR(_V)        \
{                                        \
    uint8    cnt = LEN_KEY_DESC_REPLAY;    \
    do                                    \
    {                                    \
        cnt--;                            \
        _V[cnt]++;                        \
        if (cnt == 0)                    \
            break;                        \
    }while (_V[cnt] == 0);                \
}

#define INC_TX_TSC(_tsc, _cnt)              \
{                                           \
    int32 i=0;                                \
    while (++_tsc[i] == 0x0)                \
    {                                       \
        i++;                                \
        if (i == (_cnt))                    \
            break;                          \
    }                                       \
}


#define MAKE_802_3_HEADER(_p, _pMac1, _pMac2, _pType)                   \
{                                                                       \
    NdisMoveMemory(_p, _pMac1, MAC_ADDR_LEN);                           \
    NdisMoveMemory((_p + MAC_ADDR_LEN), _pMac2, MAC_ADDR_LEN);          \
    NdisMoveMemory((_p + MAC_ADDR_LEN * 2), _pType, LENGTH_802_3_TYPE); \
}

/* Endian byte swapping codes */
#define SWAP16(x) \
    ((uint16)( \
    (((uint16)(x) & (uint16) 0x00ffU) << 8) | \
    (((uint16)(x) & (uint16) 0xff00U) >> 8) ))

#define SWAP32(x) \
    ((uint32)( \
    (((uint32)(x) & (uint32) 0x000000ffUL) << 24) | \
    (((uint32)(x) & (uint32) 0x0000ff00UL) <<  8) | \
    (((uint32)(x) & (uint32) 0x00ff0000UL) >>  8) | \
    (((uint32)(x) & (uint32) 0xff000000UL) >> 24) ))

#define SWAP64(x) \
    ((uint64)( \
    (uint64)(((uint64)(x) & (uint64) 0x00000000000000ffULL) << 56) | \
    (uint64)(((uint64)(x) & (uint64) 0x000000000000ff00ULL) << 40) | \
    (uint64)(((uint64)(x) & (uint64) 0x0000000000ff0000ULL) << 24) | \
    (uint64)(((uint64)(x) & (uint64) 0x00000000ff000000ULL) <<  8) | \
    (uint64)(((uint64)(x) & (uint64) 0x000000ff00000000ULL) >>  8) | \
    (uint64)(((uint64)(x) & (uint64) 0x0000ff0000000000ULL) >> 24) | \
    (uint64)(((uint64)(x) & (uint64) 0x00ff000000000000ULL) >> 40) | \
    (uint64)(((uint64)(x) & (uint64) 0xff00000000000000ULL) >> 56) ))

#define ROUND_UP(__x, __y) \
    (((uint32)((__x)+((__y)-1))) & ((uint32)~((__y)-1)))

#ifdef RT_BIG_ENDIAN

#define cpu2le64(x) SWAP64((x))
#define le2cpu64(x) SWAP64((x))
#define cpu2le32(x) SWAP32((x))
#define le2cpu32(x) SWAP32((x))
#define cpu2le16(x) SWAP16((x))
#define le2cpu16(x) SWAP16((x))
#define cpu2be64(x) ((uint64)(x))
#define be2cpu64(x) ((uint64)(x))
#define cpu2be32(x) ((uint32)(x))
#define be2cpu32(x) ((uint32)(x))
#define cpu2be16(x) ((uint16)(x))
#define be2cpu16(x) ((uint16)(x))

#else /* Little_Endian */

#define cpu2le64(x) ((uint64)(x))
#define le2cpu64(x) ((uint64)(x))
#define cpu2le32(x) ((uint32)(x))
#define le2cpu32(x) ((uint32)(x))
#define cpu2le16(x) ((uint16)(x))
#define le2cpu16(x) ((uint16)(x))
#define cpu2be64(x) SWAP64((x))
#define be2cpu64(x) SWAP64((x))
#define cpu2be32(x) SWAP32((x))
#define be2cpu32(x) SWAP32((x))
#define cpu2be16(x) SWAP16((x))
#define be2cpu16(x) SWAP16((x))

#endif /* RT_BIG_ENDIAN */




/***************************************************************************
  *    Auth/Scan control block related macro
  **************************************************************************/

/* NLO define */
#define MAX_SSID_ENTRY                     3
#define MAX_CHANNEL_HINT                1

/* AUTH define */
#define AUTH_ALGO_80211_OPEN                  1
#define AUTH_ALGO_80211_SHARED_KEY      2
#define AUTH_ALGO_WPA                               3
#define AUTH_ALGO_WPA_PSK                       4
#define AUTH_ALGO_WPA_NONE                    5
#define AUTH_ALGO_RSNA                              6
#define AUTH_ALGO_RSNA_PSK                      7

/* Auth Key management suite define (AKM) */
#define AKM_SUITE_NONE                  0
#define AKM_SUITE_8021X                 1
#define AKM_SUITE_PSK                     2

/* PHY mode definitions */
#define PHY_11BG_MIXED                  0
#define PHY_11B                                1
#define PHY_11G                                4
#define PHY_11N                                6
#define PHY_11GN_MIXED                  7
#define PHY_11BGN_MIXED                9

/* PHY type definitions */
#define PHY_TYPE_ANY                     0
#define PHY_TYPE_FHSS                   1
#define PHY_TYPE_DSSS                   2
#define PHY_TYPE_IRBASEBAND       3
#define PHY_TYPE_OFDM                  4
#define PHY_TYPE_HRDSSS               5
#define PHY_TYPE_ERP                      6
#define PHY_TYPE_HT                       7


/* power status related definitions */
#define PWR_ACTIVE                      0
#define PWR_SAVE                         1
#define PWR_MMPS                        2            //MIMO power save

/* BSS Type definitions */
#define BSS_INFRA                        1

/* SCAN Type definitions */
#define SCAN_PASSIVE                 18        // scan with no probe request, only wait beacon and probe response
#define SCAN_ACTIVE                   19        // scan with probe request, and wait beacon and probe response

/* IE code */
#define IE_SSID                             0
#define IE_SUPP_RATES                1
#define IE_DS_PARM                      3
#define IE_ERP                              42    // 802.11g
#define IE_HT_CAP                       45    // 802.11n d1. HT CAPABILITY.
#define IE_EXT_SUPP_RATES       50    // 802.11g
#define IE_ADD_HT                       61    // 802.11n d1. ADDITIONAL HT CAPABILITY. 
#define IE_EXT_CAPABILITY        127   // 802.11n D3.03 
#define IE_VENDOR_SPECIFIC      221

/* IE length define */
#define SIZE_HT_CAP_IE                       26
#define SIZE_ADD_HT_INFO_IE            22

/* TIMEOUT definitions */
#define SHORT_CHANNEL_TIME              90        // unit: msec
#define MIN_CHANNEL_TIME                110        // unit: msec, for dual band scan
#define MAX_CHANNEL_TIME                140       // unit: msec, for single band scan

/* Firmware NLO status */
#define FW_NLO_START                    0
#define FW_NLO_STOP                      1

NDIS_STATUS MiniportMMRequest(IN uint8 QueIdx, IN puchar pData, IN uint32 Length, IN  pBD_t pBufDesc) XIP_ATTRIBUTE(".xipsec0");
extern void RTMPWriteTxWI_Mgmt(PTXWI_STRUC    pTxWI,TX_BLK *pTxBlk) XIP_ATTRIBUTE(".xipsec0");
extern void STAFindCipherAlgorithm(TX_BLK    *pTxBlk,bool    bClearFrame) XIP_ATTRIBUTE(".xipsec0");
extern void STABuildCommon802_11Header(TX_BLK *pTxBlk) XIP_ATTRIBUTE(".xipsec0");
extern void RTMPWriteTxWI_Data(PTXWI_STRUC pTxWI, TX_BLK *pTxBlk) XIP_ATTRIBUTE(".xipsec0");

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
NDIS_STATUS RTMPRawDataSanity(IN RX_BLK* pRxBlk);

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
    ========================================================================
*/
extern bool WifiRxDoneInterruptHandle (pBD_t pBufDesc);

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
extern int sta_legacy_frame_tx(pBD_t PktBuff,  uint16  PacketLen, bool    bClearFrame) XIP_ATTRIBUTE(".xipsec0");


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
#ifdef CONFIG_SOFTAP
extern int AP_Legacy_Frame_Forward_Tx(RX_BLK* pRxBlk, bool bClearFrame) XIP_ATTRIBUTE(".xipsec0");
#endif


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
NDIS_STATUS    RTMPCheckRxError(PHEADER_802_11 pHeader, PRXWI_STRUC pRxWI,PRXINFO_STRUC pRxINFO);

#endif /* __RTMP_GENERAL_PUB_H__ */

