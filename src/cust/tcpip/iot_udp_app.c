#include <stdio.h>
#include <string.h>

#include "iot_udp_app.h"
#include "dhcpc.h"
#include "uip.h"
#include "uiplib.h"
#include "iot_api.h"
#if CFG_SUPPORT_DNS
#include "resolv.h"
#endif
#ifdef CONFIG_SOFTAP
#include "dhcpd.h"
#endif

extern uint8 gCurrentAddress[];
extern IOT_ADAPTER       IoTpAd;
struct iot_udp_app_state udp_app_state;

#ifdef CONFIG_SOFTAP
extern struct uip_dhcpd_conn uip_dhpcd_conns[UIP_DHCPD_CONNS];
#endif


void
iot_udp_app_init(void)
{
#ifdef CONFIG_SOFTAP
    dhcpd_init();
#else
    dhcpc_init(gCurrentAddress, 6);
#endif

#if CFG_SUPPORT_DNS
    /* DNS client. */
    resolv_init();
#endif

    /********* Customer APP start. **********/
    /* udp_client_init(); */

#if UDP_SRV_APP1_ENABLE
    udp_srv_app1_init();
#endif
#if UDP_SRV_APP2_ENABLE
    udp_srv_app2_init();
#endif
#if UDP_CLI_APP1_ENABLE
    udp_cli_app1_init();
#endif

    /********* Customer APP end. **********/

    return;
}

void
iot_udp_appcall(void)
{
    UIP_UDP_CONN *udp_conn = uip_udp_conn;
    u16_t lport, rport;

    lport=HTONS(udp_conn->lport);
    rport=HTONS(udp_conn->rport);

    if (lport == DHCPC_CLIENT_PORT) {
        handle_dhcp();
    }
#if CFG_SUPPORT_DNS
    else if (rport == DNS_SERVER_PORT) {
        handle_resolv();
    }
#endif
    /* Customer APP start. */
#if UDP_SRV_APP1_ENABLE
    else if (lport == UDP_SRV_APP1_LOCAL_PORT) {
        handle_udp_srv_app1();
    }
#endif
#if UDP_SRV_APP2_ENABLE
    else if (lport == UDP_SRV_APP2_LOCAL_PORT) {
        handle_udp_srv_app2();
    }
#endif
    /* else if (lport == 6666) {
        udp_client_sample();
    } else if (lport == 8888) {
        resolv_usage_sample();
    }*/
#ifdef CONFIG_SOFTAP
    else if (lport == DHCPC_SERVER_PORT) {
        handle_dhcpd();
    }
#endif
#if UDP_CLI_APP1_ENABLE
    else if (lport == UDP_CLI_APP1_LOCAL_PORT) {
        handle_udp_cli_app1();
    }
#endif
    /* Customer APP end. */

    return;
}

