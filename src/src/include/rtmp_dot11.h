#ifndef __DOT11_BASE_H__
#define __DOT11_BASE_H__

#include "types.h"

//#define MAC_ADDR_LEN                6

// 4-byte HTC field.  maybe included in any frame except non-QOS data frame.  The Order bit must set 1.
typedef struct GNU_PACKED {
    uint32        rsv:1;    // Reserved
    uint32        TRQ:1;    //sounding request
    uint32        MRQ:1;    //MCS feedback. Request for a MCS feedback
    uint32        MSI:3;    //MCS Request, MRQ Sequence identifier
    uint32        MFSI:3;    //SET to the received value of MRS. 0x111 for unsolicited MFB.
    uint32        MFBorASC:7;    //Link adaptation feedback containing recommended MCS. 0x7f for no feedback or not available
    uint32        CalPos:2;    // calibration position
    uint32        CalSeq:2;  //calibration sequence
    uint32        rsv1:2;    // Reserved
    uint32        CSISTEERING:2;    //CSI/ STEERING
    uint32        NDPAnnouce:1;    // ZLF announcement
    uint32        rsv2:5;  //calibration sequence
    uint32        ACConstraint:1;    //feedback request
    uint32        RDG:1;    //RDG / More PPDU
} HT_CONTROL, *PHT_CONTROL;

// 2-byte QOS CONTROL field
typedef struct GNU_PACKED {
    uint16      TID:4;
    uint16      EOSP:1;
    uint16      AckPolicy:2;  //0: normal ACK 1:No ACK 2:scheduled under MTBA/PSMP  3: BA
    uint16      AMsduPresent:1;
    uint16      Txop_QueueSize:8;
} QOS_CONTROL, *PQOS_CONTROL;


// 2-byte Frame control field
typedef    struct    GNU_PACKED {
    uint16        Ver:2;                // Protocol version
    uint16        Type:2;                // MSDU type
    uint16        SubType:4;            // MSDU subtype
    uint16        ToDs:1;                // To DS indication
    uint16        FrDs:1;                // From DS indication
    uint16        MoreFrag:1;            // More fragment bit
    uint16        Retry:1;            // Retry status bit
    uint16        PwrMgmt:1;            // Power management bit
    uint16        MoreData:1;            // More data bit
    uint16        Wep:1;                // Wep data
    uint16        Order:1;            // Strict order expected
} FRAME_CONTROL, *PFRAME_CONTROL;

typedef    struct    GNU_PACKED _HEADER_802_11    {
    FRAME_CONTROL   FC;
    uint16          Duration;
    uint8           Addr1[MAC_ADDR_LEN];
    uint8           Addr2[MAC_ADDR_LEN];
    uint8            Addr3[MAC_ADDR_LEN];
    uint16            Frag:4;
    uint16            Sequence:12;
    uint8            Octet[0];
}    HEADER_802_11, *PHEADER_802_11;

typedef    struct    GNU_PACKED _RTS_FRAME    {
    FRAME_CONTROL   FC;
    uint16          Duration;
    uint8           Addr1[MAC_ADDR_LEN];
    uint8           Addr2[MAC_ADDR_LEN];
} RTS_FRAME, *PRTS_FRAME;

/* 2-byte BA Starting Seq CONTROL field */
typedef union GNU_PACKED _BASEQ_CONTROL {
    struct GNU_PACKED {
        uint16      FragNum:4;    /* always set to 0 */
        uint16      StartSeq:12;   /* sequence number of the 1st MSDU for which this BAR is sent */
    }   field;
    uint16           word;
} BASEQ_CONTROL, *PBASEQ_CONTROL;

/* 2-byte BA Parameter Set field  in ADDBA frames to signal parm for setting up a BA */
typedef struct GNU_PACKED _BA_PARM {
    uint16      AMSDUSupported:1;    /* 0: not permitted        1: permitted */
    uint16      BAPolicy:1;    /* 1: immediately BA    0:delayed BA */
    uint16      TID:4;    /* value of TC os TS */
    uint16      BufSize:10;    /* number of buffe of size 2304 octetsr */
} BA_PARM, *PBA_PARM;

typedef struct GNU_PACKED _FRAME_ADDBA_REQ {
    HEADER_802_11   Hdr;
    uint8    Category;
    uint8    Action;
    uint8    Token;    /* 1 */
    BA_PARM        BaParm;          /*  2 - 10 */
    uint16        TimeOutValue;    /* 0 - 0 */
    BASEQ_CONTROL    BaStartSeq; /* 0-0 */
}   FRAME_ADDBA_REQ, *PFRAME_ADDBA_REQ;

typedef struct GNU_PACKED _FRAME_ADDBA_RSP {
    HEADER_802_11   Hdr;
    uint8    Category;
    uint8    Action;
    uint8    Token;
    uint16    StatusCode;
    BA_PARM        BaParm; /*0 - 2 */
    uint16        TimeOutValue;
}   FRAME_ADDBA_RSP, *PFRAME_ADDBA_RSP;

typedef struct GNU_PACKED _PSPOLL_FRAME {
    FRAME_CONTROL FC;
    uint16 Aid;
    uint8 Bssid[MAC_ADDR_LEN];
    uint8 Ta[MAC_ADDR_LEN];
} PSPOLL_FRAME, *PPSPOLL_FRAME;

#endif // __DOT11_BASE_H__ //
