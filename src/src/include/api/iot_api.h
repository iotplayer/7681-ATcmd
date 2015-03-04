#ifndef _IOT_API_H_
#define _IOT_API_H_

#include "config.h"
#include "list.h"
#include "cnm_timer.h"
#include "flash_map.h"
#include "stddef.h"
#include "eeprom.h"
#include "rtmp_dot11.h"
#include "rtmp_general_pub.h"
#include "dot11i_wpa.h"
#include "iot_custom.h"
#include "bmd.h"
#include "crypt_md5.h"
#include "uip.h"
#include "spi-flash.h"

/******************************************************************************
* MODULE NAME:     iot_api.h
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
*   V0.0.1     Jan 2014    - Initial Version V0.0.1
*
*
* SOURCE:
* ISSUES:
*    First Implementation.
* NOTES TO USERS:
*
******************************************************************************/
#define GPIO01_SWAP  /*GPIO0, 1 swap as MT7681 Chip's design*/

/******************************************************************************
 * Macro & Structure
 ******************************************************************************/
#define TX_UNKOWN_FRAME       0x00
#define TX_MCAST_FRAME          0x01
#define TX_LEGACY_FRAME         0x02

#define RX_BLK_SET_FLAG(_pRxBlk, _flag)        (_pRxBlk->Flags |= _flag)
#define RX_BLK_TEST_FLAG(_pRxBlk, _flag)    (_pRxBlk->Flags & _flag)
#define RX_BLK_CLEAR_FLAG(_pRxBlk, _flag)    (_pRxBlk->Flags &= ~(_flag))

#define fRX_WDS            0x0001
#define fRX_AMSDU        0x0002
#define fRX_ARALINK     0x0004
#define fRX_HTC             0x0008
#define fRX_PAD             0x0010
#define fRX_AMPDU        0x0020
#define fRX_QOS             0x0040
#define fRX_INFRA         0x0080
#define fRX_EAP             0x0100
#define fRX_MESH           0x0200
#define fRX_APCLI          0x0400
#define fRX_DLS             0x0800
#define fRX_WPI             0x1000

#ifndef BW_20
#define BW_20        0
#endif
#ifndef BW_40
#define BW_40        1
#endif
#ifndef BW_80
#define BW_80        4
#endif
#ifndef BW_10
#define BW_10        3    /* 802.11j has 10MHz. This definition is for internal usage. doesn't fill in the IE or other field. */
#endif


/* new types for Media Specific Indications */
/* Extension channel offset */

/******************************************************************************
---------40 MHz Above----            ---------40 MHz Below-------
PrimaryCH   2ndCH  CenterCH        PrimaryCH   2ndCH       CenterCH
1              5              3                      1          Not Avaible   Not Avaible
2              6              4                      2          Not Avaible   Not Avaible
3              7              5                      3          Not Avaible   Not Avaible
4              8              6                      4          Not Avaible   Not Avaible
5              9              7                      5                1                  3
6              10            8                      6                2                  4
7              11            9                      7                3                  5
8              12            10                     8                4                 6
9              13            11                     9                5                 7
10    Not Avaible   Not Avaible            10               6                 8
11    Not Avaible   Not Avaible            11               7                 9
12    Not Avaible   Not Avaible            12               8                 10
13    Not Avaible   Not Avaible            13               9                 11
******************************************************************************/
#define EXTCHA_NONE            0
#define EXTCHA_ABOVE        0x1      /*40 MHz Above*/
#define EXTCHA_BELOW        0x3      /*40 MHz Below*/


/*
 *  use in STA power saving mode
 *  when does we enter sleep? below conditions are neccesary
 *  OK,you can also change their value
 */
#if (MT7681_POWER_SAVING == 1)
#define PS_FCE_TXIDLE_PERIOD               (2)            /* COUNT */
#define PS_RXWAIT_TIME                          (10)           /* unit: msec */
#define PS_UART_TRXIDLE_TIME               (1*1000)       /* unit: msec */
#define PS_NULL_ACK_WAIT_TIME            (500)          // unit: usec,watch interval from log
#define PS_NULL_ACK_WAIT_MAX             (3)           /* COUNT */
#endif

#define BEACON_LOST_TIME                      (20*1000)    /* unit: msec */
#define AUTH_TIMEOUT                              (300)        /* unit: msec */
#define NULL_FRAME_TIMEOUT                  (10*1000)    /* unit: msec */
#define BEACON_INTERVAL_TIME               (100)        /* unit: msec */

#define SM_MAX_SCAN_TIMEOUT_COUNT  (10)        /* unit: s  */
#define SM_MAX_SMNT_TIMEOUT_COUNT  (10)        /* unit: cycles    *(SmartConnection maxchanlnum)  */

#define FOUR_WAY_HS_TIMEOUT               (10*1000)   /* unit: msec */
#define MAX_VALID_CHECK_COUNT            (1)            /* unit: conter, start from 0*/

//#define DLS_TIMEOUT                 1200      /* unit: msec */
//#define ASSOC_TIMEOUT               300       /* unit: msec */
//#define JOIN_TIMEOUT                2000      /* unit: msec */
//#define CW_MIN_IN_BITS              4         /* actual CwMin = 2^CW_MIN_IN_BITS - 1 */

/* Length define */
#define MAX_LEN_OF_SSID                                  (32)
#define MAX_LEN_OF_BSS_TABLE                        (1)
#define MAX_LENGTH_OF_SUPPORT_RATES        (12)    // 1, 2, 5.5, 11, 6, 9, 12, 18, 24, 36, 48, 54
#define MAX_LEN_OF_SUPPORTED_RATES          (MAX_LENGTH_OF_SUPPORT_RATES) // 1, 2, 5.5, 11, 6, 9, 12, 18, 24, 36, 48, 54
#define MAX_VIE_LEN                                          (384)   // New for WPA cipher suite variable IE sizes.
#define TIMESTAMP_LEN                                      (8)
#define BEACON_PERIOD_LEN                              (2)
#define CAPABILITYINFO_LEN                              (2)
#define CIPHER_TEXT_LEN                                   (32)

#define MAC_IP_LEN                                  (4)
#define MAC_DOMAIN_NAME_LEN              (128)
#define DESC_MAX_LEN                             (32)
#define IOT_MAGIC_NUMBER                     (0x76814350)
#define DEFAULT_SESSION_ID                  (0xFFFFFFFF)
#define PASSWORD_MAX_LEN                    (4)
#define UART_MAX_DATALEN                    (1060-36-28)//PKTQU1_SINGLE_BUFFER_SIZE-36-28
#define IOT_BUFFER_LEN                          (256)

#define CP_HDR_LEN sizeof(IoTCtrlProtocolHeader)
#define CP_DATA_HDR_LEN sizeof(DataHeader)

/******************************************************************************
 * UART MACRO & STRUCTURE
 ******************************************************************************/
#define     UART_BAUD_AUTO          0
#define     UART_BAUD_75              75
#define     UART_BAUD_150            150
#define     UART_BAUD_300            300
#define     UART_BAUD_600            600
#define     UART_BAUD_1200          1200
#define     UART_BAUD_2400          2400
#define     UART_BAUD_4800          4800
#define     UART_BAUD_7200          7200
#define     UART_BAUD_9600          9600
#define     UART_BAUD_14400         14400
#define     UART_BAUD_19200         19200
#define     UART_BAUD_28800         28800
#define     UART_BAUD_33900         33900
#define     UART_BAUD_38400         38400
#define     UART_BAUD_57600         57600
#define     UART_BAUD_115200        115200
#define     UART_BAUD_230400        230400
#define     UART_BAUD_460800        460800
#define     UART_BAUD_921600        921600
#define     UART_BAUD_3200000       3200000

typedef enum {
    len_5=5,
    len_6,
    len_7,
    len_8
} UART_bitsPerCharacter;

typedef enum {
    sb_1=1,
    sb_2,
    sb_1_5
} UART_stopBits;

typedef enum {
    pa_none=0,
    pa_odd,
    pa_even,
    pa_space
} UART_parity;

/*
 *  packet type
 */
typedef enum {
    PKT_UNKNOWN,
    PKT_ATCMD,
    PKT_IWCMD
} PKT_TYPE;


/*
 * UART RX to Air parameters
 */
#if (UARTRX_TO_AIR_LEVEL == 2)
#define  UART2WIFI_TIMER_INTERVAL    300   //ms
#define  UART2WIFI_LEN_THRESHOLD     10    //bytes

#if (UART_INTERRUPT == 1)
#define  UARTRX_TO_AIR_THRESHOLD   (UARTRX_RING_LEN/2)
#else
#define  UARTRX_TO_AIR_THRESHOLD   (AT_CMD_MAX_LEN/2)
#endif
#endif


#if (UART_INTERRUPT == 1)
//buffer length for uart rx buffer whose data is moved from uart UART HW RX FIFO
#define    UARTRX_RING_LEN    512

//buffer length for uart tx isr buffer whose data is moved to UART HW TX FIFO
#define    UARTTX_RING_LEN    64

//how many packets can be buffered in rxring ,(each packet size need < UARTRX_RING_LEN - 1)
#define    NUM_DESCS               30

//packet header detect buf size
#define    HEAD_SIZE                AT_CMD_PREFIX_MAX_LEN

typedef void (*UART_TX_FUNC)();
typedef void (*UART_RX_FUNC)();

typedef struct {
    PKT_TYPE pkt_type;
    uint16   pkt_len;
} PKT_FIFO;//packet infor is in sequence with index[0,num_desc-1] which mapping the sequence in rx


/*
 * 2014/05/13,terrence
 * struct to descrypt the packet in ring buffer
 * we just need to know the packet number and their lengh in sequence
 */
typedef struct {
    PKT_TYPE  cur_type;               //receiving packet:which packet type is receiving current?
    uint8       cur_num;                 //receiving packet:current index of receiving packet
    uint8       pkt_num;                 //completely packets:packet number in ring buffer
    PKT_FIFO  infor[NUM_DESCS]; //completely packets:FIFO,packet infor in ring
} PKT_DESC;


typedef struct {
    BUFFER_INFO                   Rx_Buffer;  //receive buffer
    PKT_DESC                       Rx_desc;    //description
    BUFFER_INFO                   Tx_Buffer;  //transmit buffer
    /*tx, rx call back function*/
    UART_TX_FUNC                tx_cb;
    UART_RX_FUNC                rx_cb;
} UARTStruct;

#endif


/******************************************************************************
 * uIP MACRO & STRUCTURE
 ******************************************************************************/
struct GNU_PACKED ipv4hdr {

    uint8     version_ihl;
    uint8     tos;
    uint16   tot_len;
    uint16   id;
    uint16   frag_off;
    uint8     ttl;
    uint8     protocol;
    uint8     check[2];
    uint32   saddr;
    uint32   daddr;
    /*The options start here. */
};

struct GNU_PACKED udphdr {
    uint16    source_port;
    uint16    dest_port;
    uint16    len;
    uint8     check[2];
};

struct GNU_PACKED udppseudohdr {
    uint32    saddr;
    uint32    daddr;
    uint8      ttl;
    uint8      protocol;
    uint16    udpdatalen;

    uint16    source_port;
    uint16    dest_port;
    uint16    len;
    uint8      check[2];
};


typedef struct _IOT_CUST_OP_ {
    /*  Customerization access related callback functions */
    void (*IoTCustPreInit)(void);
    void (*IoTCustInit)(void);
    void (*IoTCustWifiSMInit)(void);
    void (*IoTCustWifiSMSmnt)(void);
    void (*IoTCustWifiSMConnect)(void);
    void (*IoTCustSMNTStaChgInit)(void);
    void (*IoTCustSubTask1)(void);

    void (*IoTCustScanDone)(void);
} IOT_CUST_OP;


typedef struct GNU_PACKED _IOT_CUST_CFG_ {
    TIMER_T   custTimer0;
} IOT_CUST_TIMER;


typedef struct GNU_PACKED _IOT_COMMON_CFG_ {
    uint8     BootFWIdx;
    uint8     RecovModeStatus;
    uint8     IOMode;

    uint32   UART_Baudrate;
    uint8     UART_DataBits;
    uint8     UART_Parity;
    uint8     UART_StopBits;

    uint8     TCPUDP_CS;
    uint16    IoT_TCP_Srv_Port;
    uint16    Local_TCP_Srv_Port;
    uint16    IoT_UDP_Srv_Port;
    uint16    Local_UDP_Srv_Port;

    uint8     Use_DHCP;
    uint8     STATIC_IP[MAC_IP_LEN];
    uint8     SubnetMask_IP[MAC_IP_LEN];
    uint8     DNS_IP[MAC_IP_LEN];
    uint8     GateWay_IP[MAC_IP_LEN];
    uint8     IoT_ServeIP[MAC_IP_LEN];
    //uint8    IoT_ServeDomain[MAC_DOMAIN_NAME_LEN];

    uint8     CmdPWD[PASSWORD_MAX_LEN];
} IOT_COM_CFG;


typedef struct GNU_PACKED _IOT_USER_CFG_ {
    uint8 VendorName[FLASH_USR_CFG_VENDOR_NAME_LEN];
    uint8 ProductType[FLASH_USR_CFG_PRODUCT_TYPE_LEN];
    uint8 ProductName[FLASH_USR_CFG_PRODUCT_NAME_LEN];
} IOT_USR_CFG;


typedef struct _IOT_ADAPTER {
    IOT_COM_CFG    ComCfg;
    IOT_USR_CFG     UsrCfg;
    uint8                 flash_rw_buf[RAM_RW_BUF_SIZE];
} IOT_ADAPTER;


/******************************************************************************
 * DATA PARSER  MACRO & STRUCTURE
 ******************************************************************************/
/*protocol related define*/
#ifdef RT_BIG_ENDIAN
typedef    union _IOT_CTRL_SUB_HDR_STRUC {
    struct {
        uint16 SubType:8;               //subtype
        uint16 Type:4;                    //command type. 0: management command 1: function command
        uint16 Flag:4;                     //reserved
    } field;
    uint16 word;
} IOT_CTRL_SUB_HDR_STRUC;
#else
typedef    union _IOT_CTRL_SUB_HDR_STRUC {
    struct {
        uint16 Flag:4;                    //reserved
        uint16 Type:4;                   //command type. 0: management command 1: function command
        uint16 SubType:8;             //subtype
    } field;
    uint16 word;
} IOT_CTRL_SUB_HDR_STRUC;
#endif

typedef struct GNU_PACKED t_IoTCtrlProtocolHeader {
    uint32 Magic;                    //protocol magic number
    uint8 ReceiveMAC[MAC_ADDR_LEN];  //receive mac address
    uint8 SendMAC[MAC_ADDR_LEN];     //sender mac address
    uint32 SessionID;                //reserved
    uint16 Sequence;                 //sequence number
    IOT_CTRL_SUB_HDR_STRUC SubHdr;
    //uint16 Flag:4;                   //reserved
    //uint16 Type:4;                  //command type. 0: management command 1: function command
    //uint16 SubType:8;            //subtype
} IoTCtrlProtocolHeader;

typedef enum t_CommandType {
    MANAGEMENT,
    FUNCTION,
    TYPE_MAX
} CommandType;

typedef enum t_ManagementCommand {
    QUERY_CAPAB_REQUEST = 1,
    QUERY_CAPAB_RESPONSE,
    CONTROL_CLIENT_OFFLINE_REQUEST,
    CONTROL_CLIENT_OFFLINE_RESPONSE,
    CONTROL_PASSWORD_SET_REQUEST,
    CONTROL_PASSWORD_SET_RESPONSE,
    CONTROL_PASSWORD_SET_CONFIRM,
    MANAGEMENT_COMMAND_MAX
} ManagementCommand;

typedef enum t_FunctionCommand {
    GPIO_SET_REQUEST = 1,
    GPIO_SET_RESPONSE,
    GPIO_GET_REQUEST,
    GPIO_GET_RESPONSE,
    UART_SET_REQUEST,
    UART_SET_RESPONSE,
    UART_GET_REQUEST,
    UART_GET_RESPONSE,
    PWM_SET_REQUEST,
    PWM_SET_RESPONSE,
    PWM_GET_REQUEST,
    PWM_GET_RESPONSE,
    FUNCTION_COMMAND_MAX
} FunctionCommand;

typedef enum t_DataType {
    STATUS,
    CLIENT_CAPABILITY,
    UART_INFORMATION,
    GPIO_INFORMATION,
    PWM_INFORMATION,
    CONTROL_PASSWORD
} DataType;

/*Control Data information*/
typedef struct GNU_PACKED t_DataHeader {
    uint16 Type;
    uint16 Length;
} DataHeader;

typedef struct GNU_PACKED t_Status {
    uint8 StatusCode;
} Status;

typedef struct GNU_PACKED t_UART_Information {
    uint8 Data[UART_MAX_DATALEN];
} UART_Information;

typedef struct GNU_PACKED t_GPIO_Information {
    uint32 GPIO_List;
    uint32 GPIO_Value;
} GPIO_Information;

typedef struct GNU_PACKED t_PWM_Information {
    uint16 RedLevel;
    uint16 GreenLevel;
    uint16 BlueLevel;
} PWM_Information;

typedef struct GNU_PACKED t_ClientCapability {
    uint16 VendorNameLen;
    uint8 VendorName[FLASH_USR_CFG_VENDOR_NAME_LEN];
    uint16 ProductTypeLen;
    uint8 ProductType[FLASH_USR_CFG_PRODUCT_TYPE_LEN];
    uint16 ProductNameLen;
    uint8 ProductName[FLASH_USR_CFG_PRODUCT_NAME_LEN];
} ClientCapability;


typedef struct GNU_PACKED t_IoTHardwareRresource {
    uint32 GPIO_Rresource;    /*bitmap for GPIO status:   bit[n] = 1  means GPIO[n] enabled*/
    uint32 UART_Rresource;    /*not used now , for extension*/
    uint32 PWM_Rresource;    /*not used now , for extension*/
    int8 GPIO_Count;
    int8 UART_Count;        /*not used now , for extension*/
    int8 PWM_Count;        /*not used now , for extension*/

} IoTHardwareRresource;

typedef struct GNU_PACKED t_IoTHardwareStatus {
    uint16 RedLevel;
    uint16 GreenLevel;
    uint16 BlueLevel;

} IoTHardwareStatus;

typedef struct GNU_PACKED t_IoTPacketInfo {
    uint32 SessionID;
    uint16 Sequence;
    uint8 ReceiveMAC[6];
    uint8 SendMAC[6];

} IoTPacketInfo;

struct cp_app_conn {
    uint8 soket_num;
    uint16 prev_seq;
};

/******************************************************************************
 * Wifi STATE MACHINE  MACRO & STRUCTURE
 ******************************************************************************/
extern uint8 gCurrentAddress[MAC_ADDR_LEN];

#define MAX_TCP_DATA        16
#define MAX_IP_STRING_LEN    16
#define MAX_SSID_PASS_LEN    32
#define MAX_AUTH_MODE_LEN    16


/*
    STA's AUTHENTICATION state machine: states, evvents, total function #
*/
#define AUTH_REQ_IDLE                   0
#define AUTH_WAIT_SEQ2                  1
#define AUTH_WAIT_SEQ4                  2
#define MAX_AUTH_STATE                  3

/*
    STA's ASSOC state machine: states, events, total function #
*/
#define ASSOC_IDLE                      0
#define ASSOC_WAIT_RSP                  1
#define REASSOC_WAIT_RSP                2
#define DISASSOC_WAIT_RSP               3
#define MAX_ASSOC_STATE                 4

typedef enum _SCAN_STA {
    SCAN_STA_IDLE,
    SCAN_STA_SCAN,
} SCAN_STA;

typedef enum _WIFI_STATE {
    WIFI_STATE_INIT = 0,
    WIFI_STATE_SMTCNT,
    WIFI_STATE_SCAN,
    WIFI_STATE_AUTH,
    WIFI_STATE_ASSOC,
    WIFI_STATE_4WAY,
    WIFI_STATE_CONNED
} WIFI_STATE;

typedef struct GNU_PACKED _STATE_MACHINE {
    uint8                CurrState;
    uint32                RetryCount;
} STATE_MACHINE, *PSTATE_MACHINE;

typedef struct _MLME_STRUCT {
    TIMER_T            nullFrameTimer;
    TIMER_T            BeaconTimer;

    /* STA state machines */
    STATE_MACHINE    CntlMachine;
    STATE_MACHINE    AssocMachine;
    STATE_MACHINE    AuthMachine;

    WIFI_STATE        CurrentWifiState;
    WIFI_STATE        PreWifiState;

    uint8            DataEnable;
    uint8            PMLevel;
    bool            TcpInit;
    bool            ValidFlashStaCfg;
    bool            ATSetSmnt;
} MLME_STRUCT, *PMLME_STRUCT;


/*  Added new encryption types */
typedef enum _NDIS_802_11_WEP_STATUS {
    Ndis802_11WEPEnabled,
    Ndis802_11Encryption1Enabled = Ndis802_11WEPEnabled,
    Ndis802_11WEPDisabled,
    Ndis802_11EncryptionDisabled = Ndis802_11WEPDisabled,
    Ndis802_11WEPKeyAbsent,
    Ndis802_11Encryption1KeyAbsent = Ndis802_11WEPKeyAbsent,
    Ndis802_11WEPNotSupported,
    Ndis802_11EncryptionNotSupported = Ndis802_11WEPNotSupported,
    Ndis802_11Encryption2Enabled,   /* TKIP mode*/
    Ndis802_11Encryption2KeyAbsent,
    Ndis802_11Encryption3Enabled,   /* AES mode*/
    Ndis802_11Encryption3KeyAbsent,
    Ndis802_11Encryption4Enabled,    /* TKIP-AES mix mode*/
    Ndis802_11Encryption4KeyAbsent,
    Ndis802_11GroupWEP40Enabled,
    Ndis802_11GroupWEP104Enabled
} NDIS_802_11_WEP_STATUS, *PNDIS_802_11_WEP_STATUS, NDIS_802_11_ENCRYPTION_STATUS, *PNDIS_802_11_ENCRYPTION_STATUS;

typedef enum _NDIS_802_11_AUTHENTICATION_MODE {
    Ndis802_11AuthModeOpen,
    Ndis802_11AuthModeShared,
    Ndis802_11AuthModeAutoSwitch,
    Ndis802_11AuthModeWPA,
    Ndis802_11AuthModeWPAPSK,
    Ndis802_11AuthModeWPANone,        /*for ad-hoc*/
    Ndis802_11AuthModeWPA2,
    Ndis802_11AuthModeWPA2PSK,
    Ndis802_11AuthModeWPA1WPA2,
    Ndis802_11AuthModeWPA1PSKWPA2PSK,
    Ndis802_11AuthModeMax           // Not a real mode, defined as upper bound
} NDIS_802_11_AUTHENTICATION_MODE, *PNDIS_802_11_AUTHENTICATION_MODE;

//
//    The definition of the cipher combination
//
//      bit3    bit2  bit1   bit0
//    +------------+------------+
//     |      WPA     |       WPA2   |
//    +------+-----+------+-----+
//    | TKIP | AES | TKIP | AES |
//    |    0  |  1  |   1  |  0  | -> 0x06
//    |    0  |  1  |   1  |  1  | -> 0x07
//    |    1  |  0  |   0  |  1  | -> 0x09
//    |    1  |  0  |   1  |  1  | -> 0x0B
//    |    1  |  1  |   0  |  1  | -> 0x0D
//    |    1  |  1  |   1  |  0  | -> 0x0E
//    |    1  |  1  |   1  |  1  | -> 0x0F
//    +------+-----+------+-----+
//
typedef    enum    _WpaMixPairCipher {
    MIX_CIPHER_NOTUSE             = 0x00,
    WPA_NONE_WPA2_TKIPAES  = 0x03,        // WPA2-TKIPAES
    WPA_AES_WPA2_TKIP           = 0x06,
    WPA_AES_WPA2_TKIPAES     = 0x07,
    WPA_TKIP_WPA2_AES           = 0x09,
    WPA_TKIP_WPA2_TKIPAES    = 0x0B,
    WPA_TKIPAES_WPA2_NONE   = 0x0C,        // WPA-TKIPAES
    WPA_TKIPAES_WPA2_AES     = 0x0D,
    WPA_TKIPAES_WPA2_TKIP    = 0x0E,
    WPA_TKIPAES_WPA2_TKIPAES = 0x0F,
}    WPA_MIX_PAIR_CIPHER;

typedef struct {
    unsigned long  LastBeaconRxTime;
    uint16         Aid;
    uint16           BeaconInterval;
    uint8           DtimCount;
    uint8           DtimPeriod;
    uint8           BcastFlag;        /* rx,TRUE if BMC data in this becaon interval */
    uint8           MessageToMe;      /* rx,TRUE if UC  data in this beacon interval */
    uint8           More_BMC;         /* TRUE if more BMC data (MoreData bit) */
    uint8           More_UC;          /* TRUE if more UC  data (MoreData bit) */
    uint8           RxPsReady;        /* rx,whether rx ready to enter power saving mode */
    uint32           FCERxWaitTime;       /* rx jiffies,count begin with rx ready*/
    uint32           FCELastTxTime;    /* tx,tx jiffies*/
    uint32           UARTLastTRTime;   /* uart last active time */
    uint32           LinkPollingTime;  /* ACTIVE NULL to polling link*/
    uint8           PsNotify;         /* whether need to notify AP before we enter sleep mode*/
} STA_PWRSAV;


typedef struct GNU_PACKED _STA_ADMIN_CONFIG {
    /***********************************************/
    /*Reference to station */
    /***********************************************/
    uint16 DefaultListenCount;
    uint8  MyMacAddr[MAC_ADDR_LEN];
    uint8  DA[MAC_ADDR_LEN];

    // station rate config/capability/phymode
    uint8  SupRate[MAX_LEN_OF_SUPPORTED_RATES];
    uint8  ExtRate[MAX_LEN_OF_SUPPORTED_RATES];
    uint8  SupRateLen;
    uint8  ExtRateLen;
    uint16 CapabilityInfo;
    uint8  Sta_PhyMode;     /* Phy mode for current STA */
    bool bClearFrame; /* Tx data is clear frame or encrypted*/

    /***********************************************/
    /*Reference to AP */
    /***********************************************/
    uint8  Bssid[MAC_ADDR_LEN];
    uint8  Ssid[MAX_LEN_OF_SSID+1];
    uint8  SsidLen;
    uint16 Aid;

    uint8  Cfg_BW;                /* BW for current AP */
    uint8  Cfg_Channel;        /* Channel for current AP */
    uint8  ExtChanOffset;     /* EXTCHA_BELOW /EXTCHA_ABOVE /EXTCHA_NONE*/
    uint8  AP_PhyMode;        /* Phy mode for current AP */
    uint8  AP_MCS;                /* The MCS for current AP's Beacon */

    NDIS_802_11_AUTHENTICATION_MODE    AuthMode;    /* This should match to whatever microsoft defined */
    uint8  Passphase[CIPHER_TEXT_LEN];
    uint8  PassphaseLen;
    uint8  DefaultKeyId;
    uint8  StatusCode;
    uint8  ShortGIfor20;
    uint8  ShortGIfor40;
    uint8  HtCapabilityLen;
    
#if 1 //CFG_SUPPORT_4WAY_HS
    NDIS_802_11_ENCRYPTION_STATUS GroupKeyWepStatus; //GroupCipher;    /* Multicast cipher suite */
    NDIS_802_11_ENCRYPTION_STATUS WepStatus;        //paireCipher;        /* Unicast cipher suite */
    bool bMixCipher;                /* Indicate current Pair & Group use different cipher suites */
    uint16 RsnCapability;

    uint8 PMK[LEN_PMK];                   /* WPA PSK mode PMK */
    uint8 PTK[LEN_PTK];                    /* WPA PSK mode PTK */
    uint8 GTK[MAX_LEN_GTK];            /* GTK from authenticator */
    CIPHER_KEY PairwiseKey;
    CIPHER_KEY SharedKey[1][4];        /* STA always use SharedKey[BSS0][0..3] */

    /* For WPA-PSK supplicant state */
    uint8 WpaState;                            /* Default is SS_NOTUSE and handled by microsoft 802.1x */
    uint8 R_Counter[LEN_KEY_DESC_REPLAY]; /* store the R_Counter value in 4way-MSG1 or 4way-MSG3 for build response */
    uint8 ReplayCounter[8];
    uint8 ANonce[32];                       /* ANonce for WPA-PSK from aurhenticator */
    uint8 SNonce[32];                        /* SNonce for WPA-PSK */

    uint8 RSNIE_Len;
    uint8 RSN_IE[MAX_LEN_OF_RSNIE]; /* The content saved here should be little-endian format. */
#endif
    STA_PWRSAV PwrCfg;
} STA_ADMIN_CONFIG, *PSTA_ADMIN_CONFIG;

/******************************************************************************
 * Interface  MACRO & STRUCTURE
 ******************************************************************************/
#if (UARTRX_TO_AIR_LEVEL == 1)
#define UART_RX_RING_BUFF_SIZE  (IOT_BUFFER_LEN - CP_HDR_LEN - CP_DATA_HDR_LEN)
#endif

#define IOT_PWM_TYPE         2   // 1 for Hardware pwm,   2 for Software pwm

#if (IOT_PWM_SUPPORT == 1 && IOT_PWM_TYPE == 1)
#define PWM_HIGHEST_LEVEL     5
#elif (IOT_PWM_SUPPORT == 1 && IOT_PWM_TYPE == 2)
#define PWM_HIGHEST_LEVEL    20    /* PWM Freq = 1000/PWM_HIGHEST_LEVEL  */
#define MAX_PWM_COUNT           5

typedef struct GNU_PACKED t_HWTimerPWMInfo {
    VOLATILE int32 on_time[MAX_PWM_COUNT];
    VOLATILE int32 off_time[MAX_PWM_COUNT];
    VOLATILE int32 on_count[MAX_PWM_COUNT];
    VOLATILE int32 off_count[MAX_PWM_COUNT];
    VOLATILE bool skip_pwm_flag[MAX_PWM_COUNT];
    VOLATILE bool available[MAX_PWM_COUNT];
} HWTimerPWMInfo;
#endif


/******************************************************************************
 * FUNCTIONS DEFINITION  --- SYSTEM
 ******************************************************************************/
void iot_sys_init()          XIP_ATTRIBUTE(".xipsec0");

int32 __romtext memcmp(const void * cs,const void * ct, unsigned long count);
int32 atoi(const int8 *s) XIP_ATTRIBUTE(".xipsec0");
void  printf_high(const char *fmt, ...);

/*Notice:  not allow to use malloc again before free the old one*/
/*example1:  malloc(A)-->free(A)-->malloc(B)-->free(B)   is OK*/
/*example1:  malloc(A)-->malloc(B)-->free(A)-->free(B)   is NG*/
void *malloc(unsigned long size);
void free(void *ptr);

void DelayTick(uint8 Time);
void usecDelay(uint32 micro);
void msecDelay(uint32 milli);

uint32 iot_get_ms_time(void);
pQU_t   iot_get_queue_info(void);
uint32 iot_get_fceint_state(void);

/*if all data is 0xFF or 0x00, we assume it is invalide*/
bool check_data_valid(puchar pdata, uint16 len)   XIP_ATTRIBUTE(".xipsec0");
void dump(puchar pdata, uint16 len);


/******************************************************************************
 * FUNCTIONS DEFINITION  --- MAC
 ******************************************************************************/
void  iot_linkdown(uint16 reason)    XIP_ATTRIBUTE(".xipsec0");
int32 rtmp_bbp_set_bw(uint8 bw) XIP_ATTRIBUTE(".xipsec0");
void  AsicSwitchChannel(uint8 Channel, bool bScan) XIP_ATTRIBUTE(".xipsec0");
int32 asic_set_channel(uint8 ch, uint8 bw, uint8 ext_ch) XIP_ATTRIBUTE(".xipsec0");
void  iot_atcmd_set_channel(uint8 Channel) XIP_ATTRIBUTE(".xipsec0");
int32 rtmp_bbp_set_ctrlch(int32 ext_ch) XIP_ATTRIBUTE(".xipsec0");
int32 rtmp_mac_set_ctrlch(int32 extch) XIP_ATTRIBUTE(".xipsec0");
void iot_sys_reboot(void) XIP_ATTRIBUTE(".xipsec0");
void store_phymode(uint8 PHYMode);

void RTMPMakeRSNIE(
    IN  uint32  AuthMode,
    IN  uint32  WepStatus,
    IN  uint8 apidx)  XIP_ATTRIBUTE(".xipsec0");

#ifdef CONFIG_STATION
void   iot_jtag_mode_switch(uint8 switch_on) XIP_ATTRIBUTE(".xipsec0");
uint32 iot_jtag_mode_get(void) XIP_ATTRIBUTE(".xipsec0");

/* Beacon loss timeout handler */
void iot_bcn_timeout_action(uint32 param, uint32 param2) XIP_ATTRIBUTE(".xipsec0");
void RestartBCNTimer(void) XIP_ATTRIBUTE(".xipsec0");
/* =====================================================================
Description:Parse incoming beacon
Return:  TRUE: The Primary Channel infor in 'HT info' IE of the received Beacon equals current primary channel
             otherwise return FALSE
=======================================================================*/
bool PeerBeaconCHInfoParse(RX_BLK* pRxBlk) XIP_ATTRIBUTE(".xipsec0");
#endif

void APAssocReqActionAndSend(IN RX_BLK *pRxBlk, IN bool isReassoc)  XIP_ATTRIBUTE(".xipsec0");
void APPeerDisassocReqAction(IN RX_BLK *pRxBlk)    XIP_ATTRIBUTE(".xipsec0");


/*========================================================================
    Routine    Description:
        get_clear_frame_flag -- get the clear frame flag
        get_clear_frame_flag=False, after 7681 scanned AP which its AuthMode!=0
        get_clear_frame_flag=False, while 7681 Smnt received PasswordLen!=0,  and scanned AP AuthMode=0
        get_clear_frame_flag=TRUE, in other case
    Arguments:
    Return Value:
            TRUE --Frame should be clear frame , and not be encrypted,
            FALSE--Frame should be encrypted
    Note:
========================================================================*/
bool get_clear_frame_flag() XIP_ATTRIBUTE(".xipsec0");



/******************************************************************************
 * FUNCTIONS DEFINITION  -- UART & Xmodem
 ******************************************************************************/
uint16 uart_get_avail(void);
uint8 uart_rb_pop(void);
void    uart_rb_init(void);
void    UART_PutChar(uint8 ch);
int32   UART_GetChar(uint8* ch);
void    uart_rb_push(uint8 ch);

#if (UART_INTERRUPT == 1)
void   uart_tx_cb(void);
void   uart_rx_cb(void);
void   uart_rx_dispatch(void);
void   uart_rxbuf_init(UARTStruct *qp);
#endif

#if (UART_INTERRUPT == 0)
int16   IoT_uart_input(uint8 *msg, int16 count);
#endif
int16   iot_uart_output(uint8 *msg, int16 count);

#if (ATCMD_UART_FW_SUPPORT == 1)
uint8   spi_flash_update_fw_done(uint8 type);
uint8   spi_flash_update_fw(uint8 type, uint32 offset, uint8 *pdata, uint16 len);
int32   iot_xmodem_update_fw(void);
unsigned short crc_cal_by_bit(const unsigned char* ptr, unsigned char len);
#endif
unsigned int crc32(unsigned char* ptr, unsigned short len);

/******************************************************************************
 * FUNCTIONS DEFINITION  -- GPIO
 ******************************************************************************/
/*Read gpio status:   (*pPolarity 0=output mode, 1=input mode),  (*pVal  0=low,  1=High) */
void  iot_gpio_read(IN int32 gpio_num, OUT uint8* pVal, OUT uint8* pPolarity);
int32 iot_gpio_output(int32 gpio_num,  int32 output);
int32 iot_gpio_input(int32 gpio_num,  uint32 *input);
int32 iot_gpios_output(uint32 output_bitmap, uint32 value_bitmap);
int32 iot_gpios_mode_chg(uint32 output_bitmap);

/*========================================================================
    Routine    Description:
        iot_cust_gpiint_hdlr --  GPIO interrupt handler

    Arguments:
    [IN] : GPI_STS ---- is a bitmap value.
                                   The value of Bit0~bit4 are mapping as Interrupt status of GPIO0~GPIO4
                                   bit[n] =1 means this interrupt event is triggered by GPIO-n
    Return Value:  NONE
    Note:
========================================================================*/
void iot_cust_gpiint_hdlr(IN uint8 GPI_STS);

/*========================================================================
    Routine    Description:
        iot_cust_set_gpiint_mode --  Set a GPIO's interrupt mode

    Arguments:
    [IN] GPIO_Num  -  [0~4]    GPIO0~GPIO4
    [IN] Val -0: no trigger
                  1: falling edge trigger
                  2: rising edge trigger
                  3: both falling and rising edge trigger
    Return Value:  0-success    1-invalid input
    Note:
========================================================================*/
uint8 iot_cust_set_gpiint_mode(IN uint8 GPIO_Num, IN uint8 Val) XIP_ATTRIBUTE(".xipsec0");

/*========================================================================
    Routine    Description:
        iot_cust_get_gpiint_mode --  Get GPIO interrupt mode for GPIO0~4

    Arguments:
    [OUT] pGPI_INT_MODE -[b1:b0]       GPIO1 interrupt mode
                            [b3:b2]      GPIO0 interrupt mode
                             [b5:b4]       GPIO2 interrupt mode
                            [b7:b6]       GPIO3 interrupt mode
                            [b9:b8]       GPIO4 interrupt mode
          For each GPIO's interrupt mode
          0: no trigger
          1: falling edge trigger
          2: rising edge trigger
          3: both falling and rising edge trigger
    Return Value:  None
    Note:
========================================================================*/
void iot_cust_get_gpiint_mode(OUT uint16* pGPI_INT_MODE) XIP_ATTRIBUTE(".xipsec0");

/*========================================================================
    Routine    Description:
        mt7681_int_set --  Set MT7681 interrupt

    Arguments:
    [IN]      INT_STS - [b15]   GPIO interrupt         (0: disable,   1: enable)
                    [b14]   UART-Lite interrupt  (0: disable,   1: enable)
                    [b13]   Timer 1 interrupt      (0: disable,   1: enable)
                    [b12]   Timer 0 interrupt      (0: disable,   1: enable)
                    [b11:b2]Reserved
                    [b1]     FCE interrupt enable (0: disable,   1: enable)
                    [b0]     PBF interrupt enable (0: disable,   1: enable)
    Return Value:
    Note:
========================================================================*/
void mt7681_int_set(IN uint16 INT_MODE)  XIP_ATTRIBUTE(".xipsec0");

/*========================================================================
    Routine    Description:
        mt7681_int_state --  Get MT7681 interrupt status

    Arguments:
    Return Value:  Return a uint16 value,  the bitmap
          INT_STS - [b15]   GPIO interrupt         (0: disable,   1: enable)
                    [b14]   UART-Lite interrupt  (0: disable,   1: enable)
                    [b13]   Timer 1 interrupt      (0: disable,   1: enable)
                    [b12]   Timer 0 interrupt      (0: disable,   1: enable)
                    [b11:b2]Reserved
                    [b1]     FCE interrupt enable (0: disable,   1: enable)
                    [b0]     PBF interrupt enable (0: disable,   1: enable)
    Note:
========================================================================*/
uint16 mt7681_int_state(void)   XIP_ATTRIBUTE(".xipsec0");



/******************************************************************************
 * FUNCTIONS DEFINITION  -- PWM
 ******************************************************************************/
void iot_sw_pwm_hdlr(void);
void iot_sw_pwm_init(void) XIP_ATTRIBUTE(".xipsec0");

#if (IOT_PWM_SUPPORT == 1)
uint16 iot_cust_pwm_def_resolution(void)  XIP_ATTRIBUTE(".xipsec0");
#if (IOT_PWM_TYPE == 1)
int32 IoT_led_pwm(int led_num, int brightness);
#elif (IOT_PWM_TYPE == 2)
int32 iot_sw_pwm_add(uint8 gpio_num, uint16 dutycycle, uint16 resolution)  XIP_ATTRIBUTE(".xipsec0");
int32 iot_sw_pwm_del(uint8 gpio_num)  XIP_ATTRIBUTE(".xipsec0");
#endif
#endif


/******************************************************************************
 * FUNCTIONS DEFINITION  -- DataParser
 ******************************************************************************/
#if ENABLE_DATAPARSING_SEQUENCE_MGMT
int32  iot_app_proc_pkt(uint8 sock_num, puchar packet , uint16 rawpacketlength);
#else
int32  iot_app_proc_pkt(puchar packet , uint16 rawpacketlength);
#endif
void   iot_udp_pkt_send(puchar pSrcAddr, puchar pDstAddr, uint16 SrcPort, uint16 DstPort, puchar pPayload, uint16  PayloadLen) XIP_ATTRIBUTE(".xipsec0");
int32  IoT_build_app_response_header(puchar buffer, uint8 ProtoType, uint8 ProtoSubType, uint16 DataType, uint16 DataLen, struct t_IoTPacketInfo *PacketInfo) XIP_ATTRIBUTE(".xipsec0");
int32  iot_app_proc_func_pkt(DataHeader* Dataheader, uint8 FunctionType, IoTPacketInfo  *pPacketInfo) XIP_ATTRIBUTE(".xipsec0");
int32  iot_app_proc_mgmt_pkt(DataHeader* Dataheader, uint16 ManagementType, IoTPacketInfo  *pPacketInfo) XIP_ATTRIBUTE(".xipsec0");
int32  iot_udp_pkt_send_direct(uip_ipaddr_t *DstAddr, puchar DstMAC, uint16 SrcPort, uint16 DstPort, puchar pPayload, uint16 PayloadLen) XIP_ATTRIBUTE(".xipsec0");
void   IoT_cp_app_connection_init(void);

#if (NO_USED_CODE_REMOVE==0)
int16  IoT_cp_app_connection_connected(uint8 fd, uint16 lport, puchar raddr, uint16 rport);
#else
int16  IoT_cp_app_connection_connected(uint8 fd);
#endif

int16   IoT_cp_app_connection_closed(uint8 fd);
uint16 IoT_cp_app_search_seq(uint8 fd);
int16   IoT_cp_app_set_seq(uint8 fd, uint16 seq);
struct cp_app_conn * IoT_cp_app_search_connection(uint8 fd);
void  uip_send(const void *data, uint16 len) XIP_ATTRIBUTE(".xipsec0");

/******************************************************************************
 * FUNCTIONS DEFINITION  -- MTK SmartConnection
 ******************************************************************************/
#ifdef CONFIG_STATION
#if (CFG_SUPPORT_MTK_SMNT == 1)
void SMTCN_process(void) XIP_ATTRIBUTE(".xipsec0");
void SMTCN_state_chg_init(void) XIP_ATTRIBUTE(".xipsec0");
#endif
#endif

/*
* password - ascii string up to 63 characters in length
* ssid - octet string up to 32 octets
* ssidlength - length of ssid in octets
* output must be 40 octets in length and outputs 256 bits of key
*/
int32  RtmpPasswordHash(pchar password, puchar ssid, int32 ssidlength, puchar output);


void   split_string_cmd(puchar string_cmd, int16 cmdlen, int16 *pargc, char **argv);
uint32 simple_strtoul(const char *cp,char **endp,unsigned int base);
int8   getopt(int16 argc, char *argv[], char *opstring);
void   iot_atcmd_hdlr(void);

int32 iot_atcmd_uart_atcfg(puchar pCmdBuf, int16 AtCmdLen);
int32 iot_tcpip_connect(puchar pCmdBuf, int16 AtCmdLen);
int32 iot_tcpip_send(puchar pCmdBuf, int16 AtCmdLen);
int32 iot_tcpip_listen(puchar pCmdBuf, int16 AtCmdLen);
int32 iot_tcpip_disconnect(puchar pCmdBuf, int16 AtCmdLen);
int32 iot_tcpip_udp_creat(puchar pCmdBuf, int16 AtCmdLen);
int32 iot_tcpip_udp_del(puchar pCmdBuf, int16 AtCmdLen);
int32 iot_tcpip_udp_send(puchar pCmdBuf, int16 AtCmdLen);

int32 iot_listen (uint16 port);
int32 iot_connect(uint8 *dst, uint16 port);
void  iot_disconnect(uint8 fd);
int32 iot_send(uint8 fd, uint8 *buf, uint16 len);
int32 iot_udp_new(uint8 *rip, uint16 lport, uint16 rport);
void  iot_udp_remove(uint8 fd);
int32 iot_udp_send(uint8 fd, uint8 *buf, uint16 len, uint8 *rip, uint16 rport);
int32 iot_netif_cfg(uint8 *ip, uint8 *mask, uint8 * gw, uint8 *dns);



/******************************************************************************
 * FUNCTIONS DEFINITION  -- AES
 ******************************************************************************/
/* AES definition & structure */
#define AES_STATE_ROWS       4    /* Block size: 4*4*8 = 128 bits */
#define AES_STATE_COLUMNS 4
#define AES_BLOCK_SIZES       (AES_STATE_ROWS*AES_STATE_COLUMNS)
#define AES_KEY_ROWS           4
#define AES_KEY_COLUMNS     8    /* Key length: 4*{4,6,8}*8 = 128, 192, 256 bits */
#define AES_KEY128_LENGTH 16
#define AES_KEY192_LENGTH 24
#define AES_KEY256_LENGTH 32
#define AES_CBC_IV_LENGTH 16

typedef struct t_AesCbc
{
  uint8 key[AES_BLOCK_SIZES];
  uint32 keyLen;
  uint8 iv[AES_BLOCK_SIZES];
  uint32 ivLen;
  uint32 outBufSize;
}AesCbc;

/*
========================================================================
Routine Description:
    AES decryption

Arguments:
    CipherBlock      The block of cipher text, 16 bytes(128 bits) each block
    CipherBlockSize  The length of block of cipher text in bytes
    Key              Cipher key, it may be 16, 24, or 32 bytes (128, 192, or 256 bits)
    KeyLength        The length of cipher key in bytes
    PlainBlockSize   The length of allocated plain block in bytes

Return Value:
    PlainBlock       Return plain text
    PlainBlockSize  Return the length of real used plain block in bytes

    This function is located on ROM, no need declare as XIP
========================================================================
*/
void RT_AES_Decrypt (uint8 CipherBlock[], uint32 CipherBlockSize, uint8 Key[], uint32 KeyLength, uint8 PlainBlock[], uint32 *PlainBlockSize);

/*
========================================================================
Routine Description:
    AES encryption

Arguments:
    PlainBlock       The block of plain text, 16 bytes(128 bits) each block
    PlainBlockSize   The length of block of plain text in bytes
    Key              Cipher key, it may be 16, 24, or 32 bytes (128, 192, or 256 bits)
    KeyLength        The length of cipher key in bytes
    CipherBlockSize  The length of allocated cipher block in bytes

Return Value:
    CipherBlock      Return cipher text
    CipherBlockSize  Return the length of real used cipher block in bytes
========================================================================
*/
void RT_AES_Encrypt (uint8 PlainBlock[], uint32 PlainBlockSize, uint8 Key[], uint32 KeyLength, uint8 CipherBlock[], uint32 *CipherBlockSize);

/*
========================================================================
Routine Description:
    AES-CBC encryption

Arguments:
    PlainText        Plain text
    PlainTextLength  The length of plain text in bytes
    Key              Cipher key, it may be 16, 24, or 32 bytes (128, 192, or 256 bits)
    KeyLength        The length of cipher key in bytes
    IV               Initialization vector, it may be 16 bytes (128 bits)
    IVLength         The length of initialization vector in bytes
    CipherTextLength The length of allocated cipher text in bytes

Return Value:
    CipherText       Return cipher text
    CipherTextLength Return the length of real used cipher text in bytes

Note:
    Reference to RFC 3602 and NIST 800-38A
========================================================================
*/
/* AES-CBC operations */
void aes_cbc_encrypt (uint8 PlainText[], uint32  PlainTextLength, uint8 Key[], uint32  KeyLength, uint8 IV[], uint32 IVLength, uint8 CipherText[], uint32 *CipherTextLength);

/*
========================================================================
Routine Description:
    AES-CBC decryption

Arguments:
    CipherText       Cipher text
    CipherTextLength The length of cipher text in bytes
    Key              Cipher key, it may be 16, 24, or 32 bytes (128, 192, or 256 bits)
    KeyLength        The length of cipher key in bytes
    IV               Initialization vector, it may be 16 bytes (128 bits)
    IVLength         The length of initialization vector in bytes
    PlainTextLength  The length of allocated plain text in bytes

Return Value:
    PlainText        Return plain text
    PlainTextLength  Return the length of real used plain text in bytes

Note:
    Reference to RFC 3602 and NIST 800-38A
========================================================================
*/
void aes_cbc_decrypt (uint8 CipherText[], uint32  CipherTextLength, uint8 Key[], uint32  KeyLength, uint8 IV[], uint32 IVLength, uint8 PlainText[], uint32 *PlainTextLength);



/******************************************************************************
 * FUNCTIONS DEFINITION  -- AT Command & ATE Calibration
 ******************************************************************************/
#if (ATCMD_ATE_SUPPORT == 1)
/*ATE*/
void ATESTART(uint8 mode);
bool  Set_ATE_Switch_CH(IN uint8 Channel);
bool  Set_ATE_TX_MODE_Proc(IN uint8 TxWIPHYMODE);
bool  Set_ATE_TX_MCS_Proc(IN uint8 MCS);
bool  Set_ATE_TX_FREQ_OFFSET_Proc(IN uint8 RfFreqOffset);
bool  Set_ATE_TX_GI_Proc(IN uint8 TxWIShortGI);
bool  Set_ATE_TX_COUNT_Proc(IN uint32 TxCount);
bool  Set_ATE_TX_POWER(IN uint32 TxPower);
bool  set_ate_efuse_write(IN uint16 Offset,  IN uint8 data);
#endif

#if ((ATCMD_ATE_SUPPORT == 1) ||(ATCMD_EFUSE_SUPPORT == 1) || (EP_LOAD_SUPPORT == 1))
bool set_ate_efuse_read(IN uint16 Offset, OUT uint8 *pdata) XIP_ATTRIBUTE(".xipsec0");
#endif

#endif /*_IOT_API_H_*/
