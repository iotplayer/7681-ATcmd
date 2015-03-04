#include "uip.h"
#include "queue.h"
#include "iot_api.h"

EXTERN QU_t gFreeQueue1;
EXTERN QU_t gFreeQueue2;

/*---------------------------------------------------------------------------*/
void
mt76xx_dev_init(void)
{

}
/*---------------------------------------------------------------------------*/
unsigned int
mt76xx_dev_read(u8_t *p, u16_t len)
{
    //if(p[0]==0x01&&p[1]==0&&p[2]==0x5e){
    //    uip_len=0;
    //    return 0;
    //}
#if 0  /*for debug*/
    u8_t i;
    printf_high("read: %d ", len);
    for (i=0; i<14; i++)
        printf_high("0x%02x ", uip_buf[i]);
    printf_high("\n");
    for (i=0; i<20; i++)
        printf_high("0x%02x ", uip_buf[i+14]);
    printf_high("\n");
    for (i=0; i<40; i++)
        printf_high("0x%02x ", uip_buf[i+34]);
    printf_high("\n");
#endif

    memcpy(uip_buf, p, len);
    uip_len = len;

    return 0;
}
/*---------------------------------------------------------------------------*/
void
mt76xx_dev_send(void)
{
    pBD_t pBufDesc;

    //handle_FCE_TxTS_interrupt();
    pBufDesc = apiQU_Dequeue(&gFreeQueue2);

    if (pBufDesc ==NULL) {
        printf("dequeue fail\n");
        return;
    }
#if 0 /*for debug*/
    {
        u8_t i;
        printf_high("write: %d ", uip_len);
        for (i=0; i<14; i++)
            printf_high("0x%02x ", uip_buf[i]);
        printf_high("\n");
        for (i=0; i<20; i++)
            printf_high("0x%02x ", uip_buf[i+14]);
        printf_high("\n");
        for (i=0; i<40; i++)
            printf_high("0x%02x ", uip_buf[i+34]);
        printf_high("\n");
    }
#endif
    memcpy(pBufDesc->pBuf, uip_buf, uip_len);

    sta_legacy_frame_tx(pBufDesc, uip_len, get_clear_frame_flag());
}
/*---------------------------------------------------------------------------*/
