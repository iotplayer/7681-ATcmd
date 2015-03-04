#include <stdio.h>
#include "string.h"
#include "types.h"
#include "uart.h"
#include "iot_custom.h"
#include "iot_api.h"
#include "bmd.h"


/******************************************************************************
* MODULE NAME:     iot_uart_rb.c
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
#if (UARTRX_TO_AIR_LEVEL == 2)
#define  INDICATE_GPIO  2

#if (UART_INTERRUPT == 1)
extern UARTStruct UARTPort;
#else
extern uint8 command[];
extern int16 cmd_len;
#endif

TIMER_T       uart2wifi_timer;
uint32     uart2wifi_interval          =   UART2WIFI_TIMER_INTERVAL;
int32       uart2wifi_triger_count   =   UART2WIFI_LEN_THRESHOLD;

UartRxMode  iot_uart_rx_mode          =   UARTRX_PUREDATA_MODE;   //UARTRX_ATCMD_MODE
int32         previous_input                 = -1;


void iot_cust_u2w_init(uint32 interval, int32 triger_count)
{
    uart2wifi_interval     = interval;
    uart2wifi_triger_count = triger_count;

    if (uart2wifi_triger_count > UARTRX_TO_AIR_THRESHOLD) {
        uart2wifi_triger_count = UARTRX_TO_AIR_THRESHOLD;
    }

    cnmTimerInitTimer(&uart2wifi_timer,  iot_cust_u2w_time_act, 0, 0);
}


void iot_cust_u2w_mode_chg(UartRxMode mode)
{
    iot_uart_rx_mode = mode;

    //clear rx buffer
#if (UART_INTERRUPT == 1)
    uart_rxbuf_init((UARTStruct*)(&UARTPort));
#else
    cmd_len = 0;
#endif

    if (mode == UARTRX_PUREDATA_MODE)  {
        printf_high("UARTRX_PUREDATA_MODE\n");
        cnmTimerStartTimer(&uart2wifi_timer, uart2wifi_interval);
    }  else if (mode == UARTRX_ATCMD_MODE)  {
        printf_high("UARTRX_ATCMD_MODE\n");
        cnmTimerStopTimer(&uart2wifi_timer);
    }
    return;
}


void iot_cust_u2w_time_act(uint32 param1, uint32 param2)
{
#if (UART_INTERRUPT == 1)
    int16  i = 0;
    uint8 * pCmdBuf;
    BUFFER_INFO *rx_ring = &(UARTPort.Rx_Buffer);
    int16 rx_len = 0;

    //if it is not in UARTRX_PUREDATA_MODE mode,return directly
    if (iot_uart_rx_mode != UARTRX_PUREDATA_MODE) {
        return;
    }

    Buf_GetBytesAvail(rx_ring, rx_len);

    //printf_high("uart_rb_send_handler:%d\n", uart_content_count);

    pCmdBuf = (uint8 *)malloc(rx_len);

    for (i = 0; i < rx_len; i++) {
        Buf_Pop(rx_ring, pCmdBuf[i]);
    }

    iot_cust_u2w_data_hdlr(pCmdBuf, rx_len);

    free(pCmdBuf);

#else
    int16 i = 0;
    uint8 * pCmdBuf;
    int16 rx_len = 0;

    //if it is not in UARTRX_PUREDATA_MODE mode,return directly
    if (iot_uart_rx_mode != UARTRX_PUREDATA_MODE) {
        return;
    }
    rx_len = cmd_len;

    pCmdBuf = (uint8 *)malloc(rx_len);
    for (i = 0; i < rx_len; i++) {
        pCmdBuf[i] = command[i];
    }
    cmd_len = 0;
    iot_cust_u2w_data_hdlr(pCmdBuf, rx_len);
    
    free(pCmdBuf);
#endif
    cnmTimerStartTimer(&uart2wifi_timer, uart2wifi_interval);

}


void iot_cust_u2w_buf_act(void)
{
#if (UART_INTERRUPT == 1)
    int16    i = 0;
    uint8 * pCmdBuf;
    BUFFER_INFO *rx_ring = &(UARTPort.Rx_Buffer);
    int16    rx_len = 0;

    //if it is not in UARTRX_PUREDATA_MODE mode,return directly
    if (iot_uart_rx_mode != UARTRX_PUREDATA_MODE) {
        return;
    }

    Buf_GetBytesAvail(rx_ring, rx_len);

    if (rx_len < uart2wifi_triger_count) {
        return;
    }
    //printf_high("uart_rb_send_handler:%d\n", uart_content_count);

    pCmdBuf = (uint8 *)malloc(rx_len);

    for (i = 0; i < rx_len; i++) {
        Buf_Pop(rx_ring, pCmdBuf[i]);
    }
    
    iot_cust_u2w_data_hdlr(pCmdBuf, rx_len);
    
    free(pCmdBuf);

#else
    int16    i = 0;
    uint8 * pCmdBuf;
    int16    rx_len = 0;

    //if it is not in UARTRX_PUREDATA_MODE mode,return directly
    if (iot_uart_rx_mode != UARTRX_PUREDATA_MODE) {
        return;
    }
    rx_len = cmd_len;
    if (rx_len < uart2wifi_triger_count) {
        return;
    }

    pCmdBuf = (uint8 *)malloc(rx_len);
    for (i = 0; i < rx_len; i++) {
        pCmdBuf[i] = command[i];
    }
    cmd_len = 0;
    iot_cust_u2w_data_hdlr(pCmdBuf, rx_len);
    free(pCmdBuf);
#endif
}

void iot_cust_u2w_data_hdlr(uint8 *uart_content, int16 uart_content_count)
{
    if ((uart_content_count > 0) && (uart_content != NULL))
        iot_uart_output(uart_content, uart_content_count);

    /*should not call uip_send here, all uip_send need to be implememted
       in iot_udp_appcall() / iot_tcp_appcall(),  as the reason of the uIP app
       management (Connection/Port...) is controlled in the iot_***_appcall()*/
    //uip_send(uart_content, uart_content_count);//mask

    /*here should allocate a buffer or flag,
       Let iot_***_appcall() detected it and call uip_send()*/

    return;
}



int32 iot_cust_u2w_gpio_detect(void)
{
#if 0
    uint32 input, ret;
    iot_gpio_input(INDICATE_GPIO, &input);  /*shall change GPIO to input mode*/
#else
    uint8 input, Polarity, ret;
    iot_gpio_read(INDICATE_GPIO, &input, &Polarity);
#endif
    //printf_high("iot_cust_u2w_gpio_detect:%d\n",input);
    if ((input==1 && previous_input == 0)||
        (input==0 && previous_input == 1)||
        previous_input == -1)
        ret = input;
    else
        ret = -1;

    previous_input = input;
    return ret;

}

void iot_cust_u2w_mode_chg_hdlr(void)
{
    int input_status=0;

    input_status = iot_cust_u2w_gpio_detect();

    if (input_status == 0) {
        iot_cust_u2w_mode_chg(UARTRX_ATCMD_MODE);
    } else if (input_status == 1) {
        iot_cust_u2w_mode_chg(UARTRX_PUREDATA_MODE);
    }
}

#endif /*(UARTRX_TO_AIR_LEVEL == 2)*/
