#ifndef _AP_AUTH_H_
#define _AP_AUTH_H_

#include <stdio.h>
#include "rtmp_general_pub.h"
#include "queue.h"

/* Auth and Assoc mode related definitions */
#define AUTH_MODE_OPEN              0x00
#define AUTH_MODE_KEY               0x01

void ApSendDeauthAction(IN puchar pAddr, IN uint16 Reason) XIP_ATTRIBUTE(".xipsec0");
void APPeerAuthReqAction(RX_BLK *pRxBlk) XIP_ATTRIBUTE(".xipsec0");
void APSendProbeAction(RX_BLK *pRxBlk) XIP_ATTRIBUTE(".xipsec0");
void APPeerDeauthReqAction(IN RX_BLK *pRxBlk) XIP_ATTRIBUTE(".xipsec0");
void APPeerAuthSimpleRspGenAndSend(
    IN PHEADER_802_11 pHdr,
    IN uint16 Alg,
    IN uint16 Seq,
    IN uint16 StatusCode) XIP_ATTRIBUTE(".xipsec0");

#endif /*_AP_AUTH_H_*/
