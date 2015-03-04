#include "uip.h"
#include "uip_timer.h"
#if UIP_CLOUD_SERVER_SUPPORT
extern ClientActivationInfo mt76xx_Activation;
extern uint8 productID[16];
#endif

#if UDP_SRV_APP1_ENABLE
void udp_srv_app1_init()
{
    UIP_UDP_CONN *udp_conn=NULL;

    /* We don't specify a remote address and a remote port,
       that means we can receive data from any address. */
    udp_conn = uip_udp_new(NULL, HTONS(UDP_SRV_APP1_REMOTE_PORT));
    if (udp_conn) {
        /* Bind to local 7682 port. */
        uip_udp_bind(udp_conn, HTONS(UDP_SRV_APP1_LOCAL_PORT));
    }
}

void handle_udp_srv_app1()
{
    u8_t addr[48] = {0};
    u8_t ipStrMaxLen = 16,strTotLen = 0;
    uip_ipaddr_t ip;
    UIP_UDP_CONN *udp_conn = uip_udp_conn;

    if (uip_newdata()) {
        /* Here shows how to get the peer info of the received data. */
        sprintf((char *)addr, "%d.%d.%d.%d",
                uip_ipaddr1(udp_conn->ripaddr),
                uip_ipaddr2(udp_conn->ripaddr),
                uip_ipaddr3(udp_conn->ripaddr),
                uip_ipaddr4(udp_conn->ripaddr));

        printf("UDP Sample server RX: lp:%d,ra:%s,rp:%d,  got:%s\n",
               HTONS(udp_conn->lport), addr, HTONS(udp_conn->rport), uip_appdata);

        /* uip_appdata is a pointer pointed the received data. */
        if (!memcmp(uip_appdata, "ip", 2)) {
            uip_gethostaddr(&ip);
#if UIP_CLOUD_SERVER_SUPPORT
            strTotLen = ipStrMaxLen + 2 + strlen(productID);
            if(sizeof(addr) >= strTotLen){
              sprintf((char *)addr, "%d.%d.%d.%d.%d.%s", 
              uip_ipaddr1(ip), uip_ipaddr2(ip), 
              uip_ipaddr3(ip), uip_ipaddr4(ip), mt76xx_Activation.ActivedFlag, productID);
            }
#else
            strTotLen = ipStrMaxLen;
            if(sizeof(addr) >= strTotLen){
              sprintf((char *)addr, "%d.%d.%d.%d", 
              uip_ipaddr1(ip), uip_ipaddr2(ip), 
              uip_ipaddr3(ip), uip_ipaddr4(ip));
            }
#endif
            uip_send(addr, sizeof(addr));
        }
    }

    if (uip_poll()) {
    }
}
#endif


#if UDP_SRV_APP2_ENABLE
#include "dhcpd.h"
extern struct uip_dhcpd_conn uip_dhpcd_conns[UIP_DHCPD_CONNS];

void udp_srv_app2_init(void)
{
    //UIP_CONN *udp_conn=NULL;
    UIP_UDP_CONN *udp_conn=NULL;

    /* We don't specify a remote address and a remote port,
       that means we can receive data from any address. */
    udp_conn = uip_udp_new(NULL, HTONS(UDP_SRV_APP2_REMOTE_PORT));
    if (udp_conn) {
        /* Bind to local port. */
        uip_udp_bind(udp_conn, HTONS(UDP_SRV_APP2_LOCAL_PORT));
    }
}

void handle_udp_srv_app2(void)
{
    UIP_UDP_CONN *udp_conn = uip_udp_conn;
    u16_t lport, rport;
    u8_t addr[16] = {0};
    uip_ipaddr_t ip;

    static u16_t rporttest=0;
    static u16_t j=0;
    u16_t i=0;
    u8_t testbuf[24] = {0};
    uip_ipaddr_t testip;

    lport=HTONS(udp_conn->lport);
    rport=HTONS(udp_conn->rport);
    uip_gethostaddr(&ip);

    printf_high("%s,%d, lport[%d], rport[%d] \n",__FUNCTION__,__LINE__, lport,rport);

    if (uip_newdata()) {
        rporttest = rport;

        /* Here shows how to get the peer info of the received data. */
        sprintf((char *)addr, "%d.%d.%d.%d",
                uip_ipaddr1(udp_conn->ripaddr), uip_ipaddr2(udp_conn->ripaddr),
                uip_ipaddr3(udp_conn->ripaddr), uip_ipaddr4(udp_conn->ripaddr));

        printf_high("UDP Sample server RX:  LocalPort[%d],RemoteMACAddress[%s], RemotePort[%d], Data[%s]\n",
                    HTONS(udp_conn->lport), addr, HTONS(udp_conn->rport), uip_appdata);

        printf_high("HostIP: %d.%d.%d.%d \n",
                    uip_ipaddr1(ip), uip_ipaddr2(ip), uip_ipaddr3(ip), uip_ipaddr4(ip));
    }

    {
        sprintf((char *)testbuf, "[ID=%d] UDP Test!!! \n", j++);

        /*send UDP packet to all stations connected to MT7681(AP mode),
            Notice: the rporttest is determined by the last received UDP packet*/
        for (i=0; i<UIP_DHCPD_CONNS; i++) {
            if (uip_dhpcd_conns[i].flag == 1) {
                uip_ipaddr(testip,
                           (uip_dhpcd_conns[i].yiaddr & 0x000000ff),
                           (uip_dhpcd_conns[i].yiaddr & 0x0000ff00)>>8,
                           (uip_dhpcd_conns[i].yiaddr & 0x00ff0000)>>16,
                           (uip_dhpcd_conns[i].yiaddr & 0xff000000)>>24);

                iot_udp_pkt_send_direct(testip,
                                        uip_dhpcd_conns[i].chaddr,
                                        UDP_SRV_APP2_LOCAL_PORT,
                                        rporttest,
                                        testbuf,
                                        sizeof(testbuf));
                msecDelay(10);
            }
        }
    }
}
#endif

