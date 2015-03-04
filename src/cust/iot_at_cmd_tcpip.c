#include <types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "iot_api.h"

/******************************************************************************
* MODULE NAME:     iot_at_cmd_tcpip.c
* PROJECT CODE:    __MT7681__
* DESCRIPTION:
* DESIGNER:
* DATE:            Jan 2014
*
* SOURCE CONTROL:
*
* LICENSE:
*     This source code is copyright (c) 2014 Mediatek. Inc.
*     All rights reserved.
*
* REVISION     HISTORY:
*   V1.0.0     Jan 2014    - Initial Version V1.0
*
*
* SOURCE:
* ISSUES:
*    First Implementation.
* NOTES TO USERS:
*
******************************************************************************/

#if (ATCMD_TCPIP_SUPPORT == 1) && (CFG_SUPPORT_TCPIP == 1)

extern int16 optind;
extern char *optarg;

int32 iot_tcpip_connect(pchar pCmdBuf, int16 AtCmdLen)
{

    int16 argc = 0;
    char *argv[MAX_OPTION_COUNT];
    char *opString = "a:p:?";
    char opt;
    char *endptr;

    uint8  ip_addr[MAX_IP_STRING_LEN];
    uint16 port=0;
    uint16 ip_str_len;
    uint8 content=0;
    uint8 socket=0;

    memset(argv,0,4*MAX_OPTION_COUNT);
    memset(ip_addr,0,MAX_IP_STRING_LEN);
    split_string_cmd(pCmdBuf, AtCmdLen, &argc, argv);

    //printf("argc14:%d,%s,%s,%s\n",argc,argv[0],argv[1],argv[2],argv[3]);
    /*move to split_string_cmd for code slim*/
    //optind = 1;
    //optarg = NULL;

    opt = getopt(argc, argv, opString);

    while (opt != -1) {
        switch (opt) {
            case 'a':
                //printf_high("2:%s\n", optarg);
                ip_str_len =  (uint16)strlen(optarg);
                if (ip_str_len >= MAX_IP_STRING_LEN) {
                    //printf_high("ip_str_len error,%d\n",ip_str_len);
                    return -1;
                }
                memcpy(ip_addr, optarg, ip_str_len);
                content++;
                break;
            case 'p':
                port =  (uint16)simple_strtoul(optarg,&endptr,10);
                //printf_high("3:%s\n", optarg);
                content++;
                break;
            case '?':
            default:
                break;
        }
        opt = getopt(argc, argv, opString);
    }

    // iot_connect(6, "192.168.1.131", 1234);
    if (content == 2) {
        socket = iot_connect(ip_addr, port);
        printf_high("SOCKET NO:%d\n", socket);
    } else {
        //printf_high("please check content, %d\n", content);
    }

    return 0;
}


int32 iot_tcpip_send(pchar pCmdBuf, int16 AtCmdLen)
{
    int16 argc = 0;
    char *argv[MAX_OPTION_COUNT];
    char *opString = "s:d:?";
    char opt;
    char *endptr;

    uint8 send_data[MAX_TCP_DATA];
    uint8 socket=0;
    uint16 data_len;
    uint8 content=0;

    memset(argv,0,4*MAX_OPTION_COUNT);
    memset(send_data,0, MAX_TCP_DATA);
    split_string_cmd(pCmdBuf, AtCmdLen, &argc, argv);

    //printf("argc3:%d,%s,%s\n",argc,argv[0],argv[1]);
    /*move to split_string_cmd for code slim*/
    //optind = 1;
    //optarg = NULL;

    opt = getopt(argc, argv, opString);

    while (opt != -1) {
        switch (opt) {
            case 's':
                socket =  (uint8) simple_strtoul(optarg,&endptr,10);
                //printf_high("1:%s,%d\n",optarg, socket);
                content++;
                break;
            case 'd':
                //printf_high("2:%s\n", optarg);
                data_len =  (uint16) strlen(optarg);
                if (data_len >= MAX_TCP_DATA) {
                    //printf_high("data_len error,%d\n",data_len);
                    return -1;
                }
                memcpy(send_data, optarg, data_len);
                content++;
                break;
            case '?':
            default:
                break;
        }
        opt = getopt(argc, argv, opString);
    }

    if (content == 2)
        iot_send(socket, send_data, strlen((char *)send_data));
    else {
        //printf_high("please check content, %d\n", content);
    }

    return 0;
}


int32 iot_tcpip_listen(pchar pCmdBuf, int16 AtCmdLen)
{
    int16 argc = 0;
    char *argv[MAX_OPTION_COUNT];
    char *opString = "p:?";
    char opt;
    char *endptr;

    uint16 port=0;
    uint8 content=0;

    memset(argv,0,4*MAX_OPTION_COUNT);
    split_string_cmd(pCmdBuf, AtCmdLen, &argc, argv);

    //printf("argc3:%d,%s,%s\n",argc,argv[0],argv[1]);
    /*move to split_string_cmd for code slim*/
    //optind = 1;
    //optarg = NULL;

    opt = getopt(argc, argv, opString);

    while (opt != -1) {
        switch (opt) {
            case 'p':
                port =  (uint16)simple_strtoul(optarg,&endptr,10);
                //printf_high("1:%s,%d\n",optarg, port);
                content++;
                break;
            case '?':
            default:
                break;
        }
        opt = getopt(argc, argv, opString);
    }

    if (content == 1)
        iot_listen (port);
    else {
        //printf_high("please check content, %d\n", content);
    }

    return 0;
}



int32 iot_tcpip_disconnect(pchar pCmdBuf, int16 AtCmdLen)
{
    int16 argc = 0;
    char *argv[MAX_OPTION_COUNT];
    char *opString = "s:?";
    char opt;
    char *endptr;

    uint8 content=0;
    uint8 socket=0;

    memset(argv,0,4*MAX_OPTION_COUNT);
    split_string_cmd(pCmdBuf, AtCmdLen, &argc, argv);

    //printf("argc3:%d,%s,%s\n",argc,argv[0],argv[1]);
    /*move to split_string_cmd for code slim*/
    //optind = 1;
    //optarg = NULL;

    opt = getopt(argc, argv, opString);

    while (opt != -1) {
        switch (opt) {
            case 's':
                socket =  (uint8)simple_strtoul(optarg,&endptr,10);
                //printf_high("1:%s,%d\n",optarg, socket);
                content++;
                break;
            case '?':
            default:
                break;
        }
        opt = getopt(argc, argv, opString);
    }

    if (content == 1)
        iot_disconnect(socket);
    else {
        //printf_high("please check content, %d\n", content);
    }

    return 0;
}



int32 iot_tcpip_udp_creat(pchar pCmdBuf, int16 AtCmdLen)
{
    int16 argc = 0;
    char *argv[MAX_OPTION_COUNT];
    char *opString = "a:r:l:?";
    char opt = 0;
    char *endptr;

    uint8    ip_addr[MAX_IP_STRING_LEN];
    uint16 lport=0;
    uint16 rport=0;
    uint16 ip_str_len;

    uint8 content=0;
    uint8 socket=0;

    memset(argv,0,4*MAX_OPTION_COUNT);
    memset(ip_addr,0,MAX_IP_STRING_LEN);
    split_string_cmd(pCmdBuf, AtCmdLen, &argc, argv);

    //printf_high("argc14:%d,%s,%s,%s\n",argc,argv[0],argv[1],argv[2],argv[3]);
    /*move to split_string_cmd for code slim*/
    //optind = 1;
    //optarg = NULL;

    opt = getopt(argc, argv, opString);

    while (opt != -1) {
        switch (opt) {
            case 'a':
                //printf_high("1:%s\n", optarg);
                ip_str_len =    (uint16)strlen(optarg);
                if (ip_str_len >= MAX_IP_STRING_LEN) {
                    //printf_high("ip_str_len error,%d\n",ip_str_len);
                    return -1;
                }
                memcpy(ip_addr, optarg, ip_str_len);
                content++;
                break;
            case 'r':
                rport =    simple_strtoul(optarg,&endptr,10);
                //printf_high("2:%s,%d\n",optarg, rport);
                content++;
                break;

            case 'l':
                lport =  simple_strtoul(optarg,&endptr,10);
                //printf_high("3:%s\n", optarg);
                content++;
                break;
            case '?':
            default:
                break;
        }
        opt = getopt(argc, argv, opString);
    }

    if (content == 3) {
        socket = iot_udp_new(ip_addr, lport, rport);
        printf_high("SOCKET NO:%d\n", socket);
    } else {
        //printf_high("please check content, %d\n", content);
    }

    return 0;

}

int32 iot_tcpip_udp_del(pchar pCmdBuf, int16 AtCmdLen)
{
    int16 argc = 0;
    char *argv[MAX_OPTION_COUNT];
    char *opString = "s:?";
    char opt=0;
    char *endptr;

    uint8 socket=0;
    uint8 content=0;

    memset(argv,0,4*MAX_OPTION_COUNT);

    split_string_cmd(pCmdBuf, AtCmdLen, &argc, argv);

    //printf("argc3:%d,%s,%s\n",argc,argv[0],argv[1]);
    /*move to split_string_cmd for code slim*/
    //optind = 1;
    //optarg = NULL;
    opt = getopt(argc, argv, opString);

    while (opt != -1) {
        switch (opt) {
            case 's':
                socket =  (uint8) simple_strtoul(optarg,&endptr,10);
                //printf_high("1:%s,%d\n",optarg, socket);
                content++;
                break;
            case '?':
            default:
                break;
        }
        opt = getopt(argc, argv, opString);
    }

    if (content == 1)
        iot_udp_remove(socket);
    else {
        //printf_high("please check content, %d\n", content);
    }

    return 0;

}



int32 iot_tcpip_udp_send(pchar pCmdBuf, int16 AtCmdLen)
{
    int16 argc = 0;
    char *argv[MAX_OPTION_COUNT];
    char *opString = "s:d:a:r:?";
    char opt = 0;
    char *endptr;

    uint8 send_data[MAX_TCP_DATA];
    uint8 socket=0;
    uint16 data_len;
    uint8    ip_addr[MAX_IP_STRING_LEN];
    uint16 rport=0;
    uint16 ip_str_len;

    uint8 content=0;

    memset(argv,0,4*MAX_OPTION_COUNT);
    memset(send_data,0, MAX_TCP_DATA);
    memset(ip_addr,0,MAX_IP_STRING_LEN);

    split_string_cmd(pCmdBuf, AtCmdLen, &argc, argv);

    //printf_high("argc:%d,%s,%s,%s\n",argc,argv[0],argv[1],argv[2],argv[3]);
    /*move to split_string_cmd for code slim*/
    //optind = 1;
    //optarg = NULL;

    opt = getopt(argc, argv, opString);

    while (opt != -1) {
        switch (opt) {
            case 's':
                socket =  (uint8)simple_strtoul(optarg,&endptr,10);
                //printf_high("1f:%s,%d\n",optarg, socket);
                content++;
                break;
            case 'd':
                //printf_high("2:%s\n", optarg);
                data_len =  (uint16)strlen(optarg);
                if (data_len >= MAX_TCP_DATA) {
                    //printf_high("data_len error,%d\n",data_len);
                    return -1;
                }
                memcpy(send_data, optarg, data_len);
                content++;
                break;
            case 'a':
                //printf_high("3:%s\n", optarg);
                ip_str_len =    (uint16)strlen(optarg);
                if (ip_str_len >= MAX_IP_STRING_LEN) {
                    //printf_high("ip_str_len error,%d\n",ip_str_len);
                    return -1;
                }
                memcpy(ip_addr, optarg, ip_str_len);
                content++;
                break;
            case 'r':
                rport = (uint16)simple_strtoul(optarg,&endptr,10);
                //printf_high("4:%s,%d\n",optarg,rport);
                content++;
                break;
            case '?':
            default:
                break;
        }
        opt = getopt(argc, argv, opString);
    }

    if (content == 4)
        iot_udp_send(socket, send_data, strlen((char *)send_data), ip_addr, rport);
    else {
        //printf_high("please check content, %d\n", content);
    }

    return 0;
}

int16 iot_tcpip_set_static_ip(pchar pCmdBuf, int16 AtCmdLen)
{
    char *argv[MAX_OPTION_COUNT];
    char *opString = "i:m:g:d:?";
    //char *endptr = NULL;
    //long num = 0;
    //int i=0;
    int16 argc = 0;
    char opt = 0;

    uint16 ip_str_len;
    uint8 local_ip[MAX_IP_STRING_LEN];
    uint8 ip_mask[MAX_IP_STRING_LEN];
    uint8 gateway_ip[MAX_IP_STRING_LEN];
    uint8 dns_ip[MAX_IP_STRING_LEN];

    puchar pLocal_ip = NULL;
    puchar pIp_mask = NULL;
    puchar pGateway_ip = NULL;
    puchar pDns_ip = NULL;

    memset(local_ip,0,MAX_IP_STRING_LEN);
    memset(ip_mask,0,MAX_IP_STRING_LEN);
    memset(gateway_ip,0,MAX_IP_STRING_LEN);
    memset(dns_ip,0,MAX_IP_STRING_LEN);

    split_string_cmd(pCmdBuf, AtCmdLen, &argc, argv);

    //printf_high("argc1=%d , long=%d, int=%d, int16=%d  char=%d\n",
    //    argc, sizeof(long), sizeof(int),sizeof(int16), sizeof(char));
    //for(i=0; i<argc; i++)
    //    printf_high(" argv[%d] = %s \n",i, argv[i]);

    opt = getopt(argc, argv, opString);

    while (opt != -1) {
        switch (opt) {
            case 'i':
                ip_str_len =  (uint16)strlen(optarg);
                if (ip_str_len >= MAX_IP_STRING_LEN) {
                    //printf_high("ip_str_len error,%d\n",ip_str_len);
                    return -1;
                }
                memcpy(local_ip, optarg, ip_str_len);
                pLocal_ip = local_ip;
                break;
            case 'm':
                ip_str_len =  (uint16)strlen(optarg);
                if (ip_str_len >= MAX_IP_STRING_LEN) {
                    //printf_high("ip_str_len error,%d\n",ip_str_len);
                    return -1;
                }
                memcpy(ip_mask, optarg, ip_str_len);
                pIp_mask = ip_mask;
                break;
            case 'g':
                ip_str_len =  (uint16)strlen(optarg);
                if (ip_str_len >= MAX_IP_STRING_LEN) {
                    //printf_high("ip_str_len error,%d\n",ip_str_len);
                    return -1;
                }
                memcpy(gateway_ip, optarg, ip_str_len);
                pGateway_ip = gateway_ip;
                break;
            case 'd':
                ip_str_len =  (uint16)strlen(optarg);
                if (ip_str_len >= MAX_IP_STRING_LEN) {
                    //printf_high("ip_str_len error,%d\n",ip_str_len);
                    return -1;
                }
                memcpy(dns_ip, optarg, ip_str_len);
                pDns_ip = dns_ip;
                break;
            case '?':
            default:
                break;
        }
        opt = getopt(argc, argv, opString);
    }
    //printf_high("%s,%s,%s,%s",pLocal_ip, pIp_mask, pGateway_ip, pDns_ip);
    iot_netif_cfg(pLocal_ip, pIp_mask, pGateway_ip, pDns_ip);

    return 0;
}


int16 iot_tcpip_netstat(pchar pCmdBuf, int16 AtCmdLen)
{
    printf_high("iot_tcpip_netstat\n");
    return 0;
}
#endif

