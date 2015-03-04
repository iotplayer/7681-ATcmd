#ifndef _QUEUE_H_
#define _QUEUE_H_
/******************************************************************************
 * MODULE NAME:     queue.h
 * PROJECT CODE:    __MT7681__
 * DESCRIPTION:     The queue must be used with __MT7681__ PDMA.
 * DESIGNER:        Charles Su
 * DATE:            Aug 2014
 *
 * SOURCE CONTROL:
 *
 * LICENSE:
 *     This source code is copyright (c) 2014 Mediatek. Inc.
 *     All rights reserved.
 *
 * REVISION     HISTORY:
 *   V1.0.0     Aug 2014    - Initial Version V1.0
 *
 *
 * SOURCE:
 * ISSUES:
 *    First Implementation.
 * NOTES TO USERS:
 *
 ******************************************************************************/
#include "types.h"
#include "list.h"
#include "xip_ovly.h"

/******************************************************************************
 * CONFIGURATIONS
 ******************************************************************************/
/* Single buffer size of packet queue (WIFI) */
#define PKTQU1_SINGLE_BUFFER_SIZE             (1060) //(1600)    /*jinchuan    reduce Queu buf to shrink code size*/
#define PKTQU1_BUFFER_AMOUNT                  (6)

/* Single buffer size of packet queue (InBand) */ //not used
/* Single buffer size of packet queue (WIFI TX) */ //used
#define PKTQU2_SINGLE_BUFFER_SIZE             (1060)
#define PKTQU2_BUFFER_AMOUNT                  (2)


/******************************************************************************
 * TYPE DEFINITION
 ******************************************************************************/
/* Buffer Descriptor structure */
typedef struct s_buffer_descriptor {

    LIST_ENTRY_t    Entry;
    // The preallocated buffer pointer
    void*           pBuf;
    // Size of buffer
    uint16          Length;
    // First Segment Flag
    uint8           FSF;
    // Last Segment Flag
    uint8           LSF;
    // FCE Info
    uint32               FceInfo;
    // Port which received (0:from H-Port, 1:from W-Port)
    uint8           SrcPort;

    uint8           resv[3];
} BD_t, *pBD_t;

/* Type of mass buffer memory which is defined to reserve a mass of memory to
 * be the buffer.  (WIFI) */
typedef union u_buffer_memory1 {

    uint8           MassMem[PKTQU1_SINGLE_BUFFER_SIZE];
    uint32          __dummy; /* dummy for alignment */

} BUF_MEM_t1, *pBUF_MEM_t1;

/* (Inband) */
typedef union u_buffer_memory2 {

    uint8           MassMem[PKTQU2_SINGLE_BUFFER_SIZE];
    uint32          __dummy; /* dummy for alignment */

} BUF_MEM_t2, *pBUF_MEM_t2;

/* Software Queue Structure */
typedef struct s_sw_queue {

    LIST_ENTRY_t    Head;
    uint32          Amount;
    uint32          Lock;

} QU_t, *pQU_t;



/******************************************************************************
 * EXTERN
 ******************************************************************************/
EXTERN QU_t gFreeQueue1; // WIFI Rx
EXTERN QU_t gFreeQueue2; // WIFI Tx


/******************************************************************************
 * PROTOTYPES
 ******************************************************************************/
void apiQU_Initialize (void) XIP_ATTRIBUTE(".xipsec0");
void apiQU_Enqueue (IN pQU_t pQueue,IN pBD_t pBufDesc);
OUT pBD_t apiQU_Dequeue (IN pQU_t pQueue);
OUT pBD_t apiQU_Touch (IN pQU_t pQueue);

#endif /* _QUEUE_H_ */

