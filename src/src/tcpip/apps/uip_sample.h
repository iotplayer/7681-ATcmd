/******************************************************************************
 * MODULE NAME:     uip_sample.h
 * PROJECT CODE:    __MT7681__
 * DESCRIPTION:
 * DESIGNER:        Jinchuan.bao
 * DATE:            Nov 2014
 *
 * SOURCE CONTROL:
 *
 * LICENSE:
 *     This source code is copyright (c) 2014 Mediatek. Inc.
 *     All rights reserved.
 *
 * REVISION     HISTORY:
 *   V1.0.0     Nov 2014    - Initial Version V1.0
 *
 *
 * SOURCE:
 * ISSUES:
 *    First Implementation.
 * NOTES TO USERS:
 *
 ******************************************************************************/


#ifndef __UIPSAMPLE_H__
#define __UIPSAMPLE_H__

/***  Example for UDP SERVER APP 1 ****/
/*This APP make MT7681 as a UDP server,  it will receive UDP packet from local port7682,
   if received message "ip",  this UDP server will send its own ip address to the UDP clients who send "ip" message,
   For MTK IoTManager.APK demo , this macro need to be enabled*/
#define UDP_SRV_APP1_ENABLE       1


#ifdef CONFIG_SOFTAP
/***  Example for UDP SERVER APP 2 ****/
/*This APP make MT7681 as a UDP server, it will receive UDP packet from local port5555 ,
    and determined the remote port, then send UDP test packet to remote port for all connected stations
    This APP only demo in MT7681 AP mode */
#define UDP_SRV_APP2_ENABLE       0
#endif

/***  Example for UDP CLIENT APP 1 ****/
/*This APP make MT7681 as a UDP client, it will send UDP packet contrains "hello..."
   message from local port5556 to IoT server port8889 every 5sec,
   and dump the received messages from IoTserver port8889*/
#define UDP_CLI_APP1_ENABLE       0


/**************************************************/
/******************UDP SERVER*********************/
/**************************************************/
#if UDP_SRV_APP1_ENABLE
#define UDP_SRV_APP1_LOCAL_PORT     7682
#define UDP_SRV_APP1_REMOTE_PORT    0

extern void udp_srv_app1_init(void)  XIP_ATTRIBUTE(".xipsec0");
extern void handle_udp_srv_app1(void);
#endif


#if UDP_SRV_APP2_ENABLE
#define UDP_SRV_APP2_LOCAL_PORT     5555
#define UDP_SRV_APP2_REMOTE_PORT    0

extern void udp_srv_app2_init(void)  XIP_ATTRIBUTE(".xipsec0");
extern void handle_udp_srv_app2(void);
#endif


/**************************************************/
/******************UDP CLIENT**********************/
/**************************************************/
#if UDP_CLI_APP1_ENABLE
#define UDP_CLI_APP1_LOCAL_PORT     5556  /*The Local UDP Client Port  if 7681 as a UDP client */
#define UDP_CLI_APP1_REMOTE_PORT    8889  /*The Remote UDP Server Port*/
#define UDP_CLI_APP1_IOT_SRV_IP     {192,168,32,100}  /*The Remote UDP Server IP address in WAN/LAN */


extern void udp_cli_app1_init()  XIP_ATTRIBUTE(".xipsec0");
extern void handle_udp_cli_app1(void);
#endif
void udp_client_init();
void udp_client_sample();










/***  Example for TCP SERVER APP 1 ****/
/*This APP make MT7681 as a TCP server,  it will send TCP packet contrains "hello..."
    message from local port9998 to TCP Client every 5sec,
   and dump the received messages */
#define TCP_SRV_APP1_ENABLE       0

/***  Example for TCP CLIENT APP 1 ****/
/*This APP make MT7681 as a TCP Client,  it will send TCP packet contrains "hello..."
    message from local port9999 to IoT server port7777 every 5sec,
   and dump the received messages from IoTserver port7777*/
#define TCP_CLI_APP1_ENABLE       0


/**************************************************/
/******************TCP SERVER**********************/
/**************************************************/
#if TCP_SRV_APP1_ENABLE
#define TCP_SRV_APP1_LOCAL_PORT     9998   /*The Local TCP Server Port if MT7681 as a TCP server*/

void handle_tcp_srv_app1(void);
#endif


/**************************************************/
/******************TCP CLIENT**********************/
/**************************************************/
#if TCP_CLI_APP1_ENABLE
#define TCP_CLI_APP1_LOCAL_PORT       9999   /*The Local TCP Client Port if MT7681 as a TCP client */
#define TCP_CLI_APP1_REMOTE_PORT    7777   /*The Remote TCP Server Port in the WAN/LAN */
#define TCP_CLI_APP1_IOT_SRV_IP        {192,168,32,100}  /*The Remote TCP Server IP Address in the WAN/LAN */

void tcp_cli_app1_init(void);
void handle_tcp_cli_app1(void);
#endif

#endif /* __UIPSAMPLE_H__ */
/** @} */
