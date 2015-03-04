#include "iot_tcp_app.h"
#include "uip.h"
#include "uiplib.h"
#include "iot_api.h"
#include "string.h"
#include "webclient.h"
#include "uip_timer.h"

extern IOT_ADAPTER       IoTpAd;
extern int cli_fd;
extern u8_t gCurrentAddress[];
extern u16_t http_clientPort;
#if UIP_CLOUD_SERVER_SUPPORT
extern u16_t TCP_cloudClientPort;
ClientActivationInfo mt76xx_Activation;
#endif
/*---------------------------------------------------------------------------*/
/*
 * The initialization function. We must explicitly call this function
 * from the system initialization code, some time after uip_init() is
 * called.
 */
void
iot_tcp_app_init(void)
{
    /* We start to listen for connections on TCP port 7681. */
    uip_listen(HTONS(IoTpAd.ComCfg.Local_TCP_Srv_Port));
#if CFG_SUPPORT_TCPIP_ROBUST_TEST
    uip_listen(HTONS(7684));
#endif

#if UIP_HTTP_CLIENT_SUPPORT
    //http client setting
    //webclient_get("192.168.1.100", HTTP_SERVER_DEFAULT_PORT, "/MT7681_sta_header.bin");
    webclient_get("192.168.2.1", HTTP_SERVER_DEFAULT_PORT, "/index.html");
    //printf_high("http client port:%d\n", http_clientPort);
    uip_listen(HTONS(http_clientPort));
#endif

#if TCP_CLI_APP1_ENABLE
    tcp_cli_app1_init();
#endif

#if TCP_SRV_APP1_ENABLE
    uip_listen(HTONS(TCP_SRV_APP1_LOCAL_PORT));
#endif

#if UIP_CLOUD_SERVER_SUPPORT
    uip_listen(HTONS(CLOUD_TCP_SERVER_PORT));
    cloud_para_check_connect();
#endif

}

/*---------------------------------------------------------------------------*/
/*
 * In mt76xx_tcp_app.h we have defined the UIP_APPCALL macro to
 * mt7681_tcp_appcall so that this funcion is uIP's application
 * function. This function is called whenever an uIP event occurs
 * (e.g. when a new connection is established, new data arrives, sent
 * data is acknowledged, data needs to be retransmitted, etc.).
 */
void
iot_tcp_appcall(void)
{
    u16_t lport = HTONS(uip_conn->lport);

#if UIP_HTTP_CLIENT_SUPPORT
    if (lport == http_clientPort) {
        webclient_appcall();
        return;
    }
#endif

#if TCP_CLI_APP1_ENABLE
    if (lport == TCP_CLI_APP1_LOCAL_PORT) {
        handle_tcp_cli_app1();
        return;
    }
#endif

#if TCP_SRV_APP1_ENABLE
    if (lport == TCP_SRV_APP1_LOCAL_PORT) {
        handle_tcp_srv_app1();
        return;
    }
#endif

#if UIP_CLOUD_SERVER_SUPPORT
    if (uip_newdata()) {
        if(lport == CLOUD_TCP_SERVER_PORT){
            if(uip_datalen()%2 == 0){
                cloud_activation_process(uip_appdata, uip_datalen());
            }else{
                printf_high("data length:%d from apk error\n", uip_datalen());
            }
            return;
        }
    }

    if(lport == http_clientPort) {
        webclient_appcall();
        return;
    }

    if(lport == TCP_cloudClientPort) {
        cloudClient_tcpAppcall();
        return;
    }
#endif

    handle_tcp_app();
}


void
handle_tcp_app(void)
{
    /*
    * The uip_conn structure has a field called "appstate" that holds
    * the application state of the connection. We make a pointer to
    * this to access it easier.
    */
    struct iot_tcp_app_state *s = &(uip_conn->appstate);
    u16_t lport = HTONS(uip_conn->lport);

    if (uip_aborted() || uip_timedout() || uip_closed()) {
        switch (lport) {
            case 7682: //IoT as clent.
                cli_fd = -1;
        }
        printf("fd %d uip_aborted.%d\n", uip_conn->fd, HTONS(uip_conn->lport));
#if ENABLE_DATAPARSING_SEQUENCE_MGMT
        IoT_cp_app_connection_closed(uip_conn->fd);
#endif
        s->state = IOT_APP_S_CLOSED;
        s->buf = NULL;
        s->len = 0;
        uip_abort();
    }

    if (uip_connected()) {
        u8_t raddr[16];
        u8_t logon_msg[16] = "userlogon:";

        sprintf((char *)raddr, "%d.%d.%d.%d",
                uip_ipaddr1(uip_conn->ripaddr), uip_ipaddr2(uip_conn->ripaddr),
                uip_ipaddr3(uip_conn->ripaddr), uip_ipaddr4(uip_conn->ripaddr));

        printf_high("Connected fd:%d,lp:%d,ra:%s,rp:%d\n",
                    uip_conn->fd, HTONS(uip_conn->lport), raddr, HTONS(uip_conn->rport));

#if ENABLE_DATAPARSING_SEQUENCE_MGMT
        IoT_cp_app_connection_connected(uip_conn->fd
#if (NO_USED_CODE_REMOVE==0)
                                        ,HTONS(uip_conn->lport),
                                        raddr,
                                        HTONS(uip_conn->rport)
#endif
                                       );
#endif

        s->state = IOT_APP_S_CONNECTED;
        switch (lport) {
            case 7682:
                memcpy(logon_msg+10, gCurrentAddress, 6);
                uip_send(logon_msg, 16);
                break;
        }
    }

    if (uip_acked()) {
        printf("uip_acked.\n");
        s->state = IOT_APP_S_DATA_ACKED;
        s->buf = NULL;
        s->len = 0;
    }

    if (uip_newdata()) {
        printf("RX fd : %d\n", uip_conn->fd);
        if (lport == IoTpAd.ComCfg.Local_TCP_Srv_Port || lport==7682) {
#if ENABLE_DATAPARSING_SEQUENCE_MGMT
            iot_app_proc_pkt(uip_conn->fd, uip_appdata,uip_datalen());
#else
            iot_app_proc_pkt(uip_appdata,uip_datalen());
#endif
#if CFG_SUPPORT_TCPIP_ROBUST_TEST
        } else if (lport==7684) {
            uip_send(uip_appdata, uip_datalen());
#endif
        } else {
#if ATCMD_TCPIP_SUPPORT
            iot_uart_output(uip_appdata, (int16)uip_datalen());
#endif
        }
    }

    /* check if we have data to xmit for this connection.*/
    if (uip_poll()) {
#if ATCMD_TCPIP_SUPPORT
        if (s->state == IOT_APP_S_CLOSED) {
            uip_close();
        } else if (s->len > 0) {
            uip_send(s->buf, s->len);
            s->state = IOT_APP_S_DATA_SEND;
        }
#endif
    }
}



#if TCP_CLI_APP1_ENABLE
void tcp_cli_app1_init(void)
{
    UIP_CONN *tcp_conn=NULL;
    uip_ipaddr_t raddr;
    uint8 iot_srv_ip[MAC_IP_LEN] = TCP_CLI_APP1_IOT_SRV_IP;

    uip_ipaddr(raddr, iot_srv_ip[0],iot_srv_ip[1], iot_srv_ip[2],iot_srv_ip[3]);

    /* Specify remote address and port here. */
    tcp_conn = uip_connect(&raddr, HTONS(TCP_CLI_APP1_REMOTE_PORT));

    if (tcp_conn) {
        tcp_conn->lport = HTONS(TCP_CLI_APP1_LOCAL_PORT);
    } else {
        printf_high("connect fail\n");
    }
}

void handle_tcp_cli_app1(void)
{
    static struct timer user_timer; //create a timer;
    static bool app_init = FALSE;

    if (uip_newdata()) {
        printf_high("TCP Client RX [%d] bytes\n", uip_datalen());
        iot_uart_output(uip_appdata, uip_datalen());
    }

    if (uip_poll()) {
        /* below codes shows how to send data to client  */
        if ((app_init == FALSE) || timer_expired(&user_timer)) {
            printf_high("TCP CLIENT APP1 uip_poll_timer_expired\n");
            uip_send("hello,this is tcp cli...", 24);
            timer_set(&user_timer, 5*CLOCK_SECOND);
            app_init = TRUE;
        }
    }
}
#endif

#if TCP_SRV_APP1_ENABLE
void handle_tcp_srv_app1(void)
{
    static struct timer user_timer; //create a timer;
    static bool app_init = FALSE;

    if (uip_newdata()) {
        printf_high("Server RX [%d] bytes\n", uip_datalen());
        iot_uart_output(uip_appdata, uip_datalen());
    }

    if (uip_poll()) {
        /* below codes shows how to send data to client  */
        if ((app_init == FALSE) || timer_expired(&user_timer)) {
            printf_high("TCP SERVER APP1 uip_poll_timer_expired\n");
            uip_send("hello,this is tcp srv...", 24);
            timer_set(&user_timer, 5*CLOCK_SECOND);
            app_init = TRUE;
        }
    }
}
#endif

