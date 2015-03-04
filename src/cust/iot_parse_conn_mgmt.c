#include <stdio.h>
#include "string.h"
#include "types.h"
#include "iot_api.h"
#include "config.h"
#include "flash_map.h"
#include "uip-conf.h"

/******************************************************************************
* MODULE NAME:     iot_parse_conn_mgmt.c
* PROJECT CODE:    __MT7681__
* DESCRIPTION:
* DESIGNER:
* DATE:            Nov 2014
*
* SOURCE CONTROL:
*
* LICENSE:
*     This source code is copyright (c) 2014 Mediatek Tech. Inc.
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
#if ENABLE_DATAPARSING_SEQUENCE_MGMT

struct cp_app_conn cp_app_conns[UIP_CONF_MAX_CONNECTIONS];
#define INVALID_SOCK_NUM 255

void IoT_cp_app_connection_init(void)
{
    uint8 i = 0;
    //printf_high("IoT_cp_app_connection_init\n");

    for (i=0; i<UIP_CONF_MAX_CONNECTIONS; i++)  {
        cp_app_conns[i].soket_num = INVALID_SOCK_NUM;
        cp_app_conns[i].prev_seq = 0;
    }
    return;
}

int16 IoT_cp_app_connection_connected(uint8 fd
#if (NO_USED_CODE_REMOVE==0)
                                      ,uint16 lport,
                                      puchar raddr,
                                      uint16 rport
#endif
                                     )
{
    uint8 i = 0;
    //printf_high("IoT_cp_app_connection_connected:%d\n",fd);

    for (i=0; i<UIP_CONF_MAX_CONNECTIONS; i++)  {
        if (cp_app_conns[i].soket_num == INVALID_SOCK_NUM)  {
            cp_app_conns[i].soket_num = fd;
            cp_app_conns[i].prev_seq = 0;
            return 0;
        }
    }

    return -1;
}

int16 IoT_cp_app_connection_closed(uint8 fd)
{
    uint8 i = 0;
    //printf_high("IoT_cp_app_connection_closed:%d\n",fd);

    for (i=0; i<UIP_CONF_MAX_CONNECTIONS; i++) {
        if (cp_app_conns[i].soket_num == fd) {
            cp_app_conns[i].soket_num = INVALID_SOCK_NUM;
            cp_app_conns[i].prev_seq = 0;
            return 0;
        }
    }

    return -1;
}

uint16 IoT_cp_app_search_seq(uint8 fd)
{
    uint8 i = 0;
    uint16 seq = 0;
    //printf_high("IoT_cp_app_search_seq:%d\n",fd);

    for (i=0; i<UIP_CONF_MAX_CONNECTIONS; i++)  {
        if (cp_app_conns[i].soket_num == fd) {
            seq = cp_app_conns[i].prev_seq;
            return seq;
        }
    }

    return 0;
}

int16 IoT_cp_app_set_seq(uint8 fd, uint16 seq)
{
    uint8 i = 0;
    //printf_high("IoT_cp_app_set_seq:%d,%d\n", fd, seq);

    for (i=0; i<UIP_CONF_MAX_CONNECTIONS; i++)  {
        if (cp_app_conns[i].soket_num == fd)  {
            cp_app_conns[i].prev_seq = seq;
            return 0;
        }
    }

    return -1;
}

struct cp_app_conn * IoT_cp_app_search_connection(uint8 fd) {
    uint8 i = 0;
    struct cp_app_conn *conn = NULL;

    for (i=0; i<UIP_CONF_MAX_CONNECTIONS; i++) {
        if (cp_app_conns[i].soket_num == fd) {
            conn = &cp_app_conns[i];
            return conn;;
        }
    }

    return NULL;
}
#endif
