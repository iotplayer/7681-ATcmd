#ifndef __IOT_TCP_APP__
#define __IOT_TCP_APP__

/* Since this file will be included by uip.h, we cannot include uip.h
   here. But we might need to include uipopt.h if we need the u8_t and
   u16_t datatypes. */
#include "uipopt.h"

//#include "psock.h"

#define IOT_APP_S_CLOSED 0
#define IOT_APP_S_DATA_SEND 1
#define IOT_APP_S_DATA_ACKED 2
#define IOT_APP_S_CONNECTED 3
#define IOT_APP_S_WAIT_SEND 4


/* Next, we define the uip_tcp_appstate_t datatype. This is the state
   of our application, and the memory required for this state is
   allocated together with each TCP connection. One application state
   for each TCP connection. */
typedef struct iot_tcp_app_state {
    u8_t state;
    u8_t *buf;
    u16_t len;
} uip_tcp_appstate_t;

/* Finally we define the application function to be called by uIP. */
void iot_tcp_appcall(void);
#ifndef UIP_APPCALL
#define UIP_APPCALL iot_tcp_appcall
#endif /* UIP_APPCALL */

void iot_tcp_app_init(void)  XIP_ATTRIBUTE(".xipsec0");
void handle_tcp_app(void);

#if UIP_CLOUD_SERVER_SUPPORT
#define CLOUD_TCP_SERVER_PORT 7680
typedef struct t_ClientActivationInfo
{
    u8_t ServerURL[48];
    u8_t DeviceID[16];
    u8_t DeviceKey[24];
    u8_t ProductID[16];
    u8_t ProductKey[16];
    u8_t UserID[12];
    u16_t ForceFlag;
    u16_t ActivedFlag;
}ClientActivationInfo;

typedef struct t_ClientConnectInfo
{
    u8_t ServerIP[18];
    u16_t ServerPort;
    u16_t FirstSendKeepAlive;
    u16_t SendSeqID;
    u16_t ReceiveSeqID;
}ClientConnectInfo;
#endif

#endif
