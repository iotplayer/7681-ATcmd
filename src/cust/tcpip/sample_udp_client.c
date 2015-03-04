#include "uip.h"
#include "uip_timer.h"

static struct timer udp_client_timer;

void udp_client_init()
{
    UIP_UDP_CONN *udp_conn=NULL;
    uip_ipaddr_t raddr={0};

    uip_ipaddr(raddr, 255,255,255,255);

    /* Specify remote address and port here. */
    udp_conn = uip_udp_new(&raddr, HTONS(9999));
    if (udp_conn) {
        /* Specify local port here. */
        uip_udp_bind(udp_conn, HTONS(6666));
    }

    /* We start a timer and send "hello world" every 10 SEC. */
    timer_set(&udp_client_timer, 10*CLOCK_SECOND);
}

void udp_client_sample()
{
    if (uip_newdata()) {
        printf_high("Client RX %d bytes\n", uip_datalen());
        /* below codes shows how to read data. */
    }

    if (uip_poll()) {
        /* below codes shows how to send data. */
        if (timer_expired(&udp_client_timer)) {
            uip_send("hello world.", 12);
        }
    }
}


#if UDP_CLI_APP1_ENABLE
void udp_cli_app1_init(void)
{
    UIP_UDP_CONN *udp_conn=NULL;
    uip_ipaddr_t raddr;
    uint8 iot_srv_ip[MAC_IP_LEN] = UDP_CLI_APP1_IOT_SRV_IP;

    uip_ipaddr(raddr, iot_srv_ip[0],iot_srv_ip[1], iot_srv_ip[2],iot_srv_ip[3]);

    /* Specify remote address and port here. */
    udp_conn = uip_udp_new(&raddr, HTONS(UDP_CLI_APP1_REMOTE_PORT));

    if (udp_conn) {
        /* Specify local port here. */
        uip_udp_bind(udp_conn, HTONS(UDP_CLI_APP1_LOCAL_PORT));
    }
}


void handle_udp_cli_app1(void)
{
    static struct timer user_timer; //create a timer;
    static bool app_init = FALSE;

    if (uip_newdata()) {
        printf_high("Server RX %d bytes\n", uip_datalen());
        iot_uart_output(uip_appdata, uip_datalen());
    }

    if (uip_poll()) {
        /* below codes shows how to send data to UDP Server  */
        if ((app_init == FALSE) || timer_expired(&user_timer)) {
            printf_high("UDP CLIENT APP1 uip_poll_timer_expired\n");
            uip_send("hello,this is udp cli...", 24);
            timer_set(&user_timer, 5*CLOCK_SECOND);
            app_init = TRUE;
        }
    }
}
#endif

