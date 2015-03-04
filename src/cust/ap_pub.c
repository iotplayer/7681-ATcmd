#include "stdio.h"
#include "rtmp_general_pub.h"
#include "ap_pub.h"

#ifdef CONFIG_SOFTAP

#define AP_MODE_OPEN                                         0
#define AP_MODE_WPAPSK_TKIP                           1
#define AP_MODE_WPA2PSK_AES                          2
#define AP_MODE_WPA1WPA2PSK_TKIPAESMIX   3

#define DEFAULT_AP_MODE    AP_MODE_WPA1WPA2PSK_TKIPAESMIX
#define Default_Ssid               "MT7681_AP1"
#define Default_Password      "12345678"

extern AP_ADMIN_CONFIG  *pIoTApCfg;
extern IOT_ADAPTER           IoTpAd;
extern MLME_STRUCT         *pIoTMlme;
extern uint8                        BCAST_ADDR[MAC_ADDR_LEN];

/*========================================================================
    Routine    Description:
        load_ap_cfg

    Arguments:
    Return Value:
========================================================================*/
bool load_ap_cfg(void)
{
    bool bFlashInit = FALSE;
    printf_high("load_ap_cfg \n");

#if (ENABLE_FLASH_SETTING == 1)
    /* read settings stored on flash STA CONFIG BLOCK */
    spi_flash_read(FLASH_OFFSET_AP_CFG_START, IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));
    //dump(IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));

    if (IoTpAd.flash_rw_buf[FLASH_AP_CFG_INFO_STORED] == AP_INFO_STORED) {
        /*Bssid shall be loaded from EEPROM region */
        //memcpy(pIoTApCfg->MBSSID.Bssid,         &IoTpAd.flash_rw_buf[FLASH_AP_CFG_BSSID],        FLASH_AP_CFG_BSSID_LEN);

        memcpy(pIoTApCfg->MBSSID.Ssid,          &IoTpAd.flash_rw_buf[FLASH_AP_CFG_SSID],        FLASH_AP_CFG_SSID_LEN);
        memcpy(&pIoTApCfg->CommonCfg.BeaconPeriod,&IoTpAd.flash_rw_buf[FLASH_AP_CFG_BCN_INTERVAL],FLASH_AP_CFG_BCN_INTERVAL_LEN);
        memcpy(pIoTApCfg->MBSSID.Passphase,    &IoTpAd.flash_rw_buf[FLASH_AP_CFG_PASSPHASE],    FLASH_AP_CFG_PASSPHASE_LEN);
        pIoTApCfg->MBSSID.PassphaseLen =  IoTpAd.flash_rw_buf[FLASH_AP_CFG_PASSPHASELEN];

        pIoTApCfg->MBSSID.SsidLen      =  IoTpAd.flash_rw_buf[FLASH_AP_CFG_SSIDLEN];
        pIoTApCfg->MBSSID.AuthMode      =  IoTpAd.flash_rw_buf[FLASH_AP_CFG_AUTH_MODE];
        pIoTApCfg->CommonCfg.Channel =  IoTpAd.flash_rw_buf[FLASH_AP_CFG_AP_CH];
        pIoTApCfg->CommonCfg.PhyMode =  IoTpAd.flash_rw_buf[FLASH_AP_CFG_PHY_MODE];
        pIoTApCfg->DtimPeriod        =  IoTpAd.flash_rw_buf[FLASH_AP_CFG_DTIM_INTERVAL];

        bFlashInit = TRUE;    /*if has valid setting in flash,  direct go Scan state, but not do smart connect*/
    }
#endif

    return bFlashInit;
}

/*========================================================================
    Routine    Description:
        store_ap_cfg

    Arguments:
    Return Value:
========================================================================*/
void store_ap_cfg(void)
{
    printf_high("store_ap_cfg \n");

    /*for shrink code size ,  current we only use 256Byte for  AP_CFG sector */
    memset(IoTpAd.flash_rw_buf, 0xff, sizeof(IoTpAd.flash_rw_buf));

    //memcpy(&IoTpAd.flash_rw_buf[FLASH_AP_CFG_BSSID],          pIoTApCfg->MBSSID.Bssid,    FLASH_AP_CFG_BSSID_LEN);
    memcpy(&IoTpAd.flash_rw_buf[FLASH_AP_CFG_SSID],             pIoTApCfg->MBSSID.Ssid,     FLASH_AP_CFG_SSID_LEN);
    memcpy(&IoTpAd.flash_rw_buf[FLASH_AP_CFG_BCN_INTERVAL], &pIoTApCfg->CommonCfg.BeaconPeriod,     FLASH_AP_CFG_BCN_INTERVAL_LEN);
    memcpy(&IoTpAd.flash_rw_buf[FLASH_AP_CFG_PASSPHASE], &pIoTApCfg->MBSSID.Passphase,     FLASH_AP_CFG_PASSPHASE_LEN);

    IoTpAd.flash_rw_buf[FLASH_AP_CFG_PASSPHASELEN]     = pIoTApCfg->MBSSID.PassphaseLen;
    IoTpAd.flash_rw_buf[FLASH_AP_CFG_AUTH_MODE]     = pIoTApCfg->MBSSID.AuthMode;

    IoTpAd.flash_rw_buf[FLASH_AP_CFG_SSIDLEN]         = pIoTApCfg->MBSSID.SsidLen;
    IoTpAd.flash_rw_buf[FLASH_AP_CFG_AP_CH]         = pIoTApCfg->CommonCfg.Channel;
    IoTpAd.flash_rw_buf[FLASH_AP_CFG_PHY_MODE]         = pIoTApCfg->CommonCfg.PhyMode;
    IoTpAd.flash_rw_buf[FLASH_AP_CFG_DTIM_INTERVAL]    = pIoTApCfg->DtimPeriod;

    /*Indicate the AP config info already be stored on flash*/
    IoTpAd.flash_rw_buf[FLASH_AP_CFG_INFO_STORED] = AP_INFO_STORED;

    spi_flash_write(FLASH_OFFSET_AP_CFG_START, IoTpAd.flash_rw_buf, sizeof(IoTpAd.flash_rw_buf));
}

/*
    ==========================================================================
    Description:
        Initialize AP specific data especially the NDIS packet pool that's
        used for wireless client bridging.
    ==========================================================================
 */
int32 iot_ap_init(void)
{
    NDIS_STATUS     Status = NDIS_STATUS_SUCCESS;

    printf("---> iot_ap_init\n");

    iot_apcfg_preinit();

#ifdef BNC_UPDATE_PERIOD /*Default turn off,  will cause system halt*/
    cnmTimerInitTimer(&pAd.ApCfg.CommonCfg.BeaconUpdateTimer, BeaconUpdateExec, 0, 0);
#endif
    if (load_ap_cfg() == FALSE) {
#if (ENABLE_FLASH_SETTING == 1)
        store_ap_cfg();
#endif
    }

    switch (pIoTApCfg->CommonCfg.PhyMode) {
        case PHY_11B:
            pIoTApCfg->CommonCfg.SupRate[0]  = 0x82;      // 1 mbps, in units of 0.5 Mbps, basic rate
            pIoTApCfg->CommonCfg.SupRate[1]  = 0x84;      // 2 mbps, in units of 0.5 Mbps, basic rate
            pIoTApCfg->CommonCfg.SupRate[2]  = 0x8B;      // 5.5 mbps, in units of 0.5 Mbps, basic rate
            pIoTApCfg->CommonCfg.SupRate[3]  = 0x96;      // 11 mbps, in units of 0.5 Mbps, basic rate
            pIoTApCfg->CommonCfg.SupRateLen  = 4;
            pIoTApCfg->CommonCfg.ExtRateLen  = 0;
            break;

        case PHY_11G:
        case PHY_11BG_MIXED:
        //case PHY_11ABG_MIXED:
        //case PHY_11ABGN_MIXED:
        case PHY_11BGN_MIXED:
        case PHY_11GN_MIXED:
            pIoTApCfg->CommonCfg.SupRate[0]  = 0x82;      // 1 mbps, in units of 0.5 Mbps, basic rate
            pIoTApCfg->CommonCfg.SupRate[1]  = 0x84;      // 2 mbps, in units of 0.5 Mbps, basic rate
            pIoTApCfg->CommonCfg.SupRate[2]  = 0x8B;      // 5.5 mbps, in units of 0.5 Mbps, basic rate
            pIoTApCfg->CommonCfg.SupRate[3]  = 0x96;      // 11 mbps, in units of 0.5 Mbps, basic rate
            pIoTApCfg->CommonCfg.SupRate[4]  = 0x12;      // 9 mbps, in units of 0.5 Mbps
            pIoTApCfg->CommonCfg.SupRate[5]  = 0x24;      // 18 mbps, in units of 0.5 Mbps
            pIoTApCfg->CommonCfg.SupRate[6]  = 0x48;      // 36 mbps, in units of 0.5 Mbps
            pIoTApCfg->CommonCfg.SupRate[7]  = 0x6c;      // 54 mbps, in units of 0.5 Mbps
            pIoTApCfg->CommonCfg.SupRateLen  = 8;
            pIoTApCfg->CommonCfg.ExtRate[0]  = 0x0C;      // 6 mbps, in units of 0.5 Mbps
            pIoTApCfg->CommonCfg.ExtRate[1]  = 0x18;      // 12 mbps, in units of 0.5 Mbps
            pIoTApCfg->CommonCfg.ExtRate[2]  = 0x30;      // 24 mbps, in units of 0.5 Mbps
            pIoTApCfg->CommonCfg.ExtRate[3]  = 0x60;      // 48 mbps, in units of 0.5 Mbps
            pIoTApCfg->CommonCfg.ExtRateLen  = 4;
            break;

        default:
            break;
    }

    return Status;
}

/* This function will be called in iot_ap_startup()*/
void iot_ap_pmk_set(void)
{
    switch (pIoTApCfg->MBSSID.AuthMode)  {
        case Ndis802_11AuthModeOpen:     /*OPEN Mode*/
            pIoTApCfg->MBSSID.WepStatus = Ndis802_11EncryptionDisabled;
            break;
        case Ndis802_11AuthModeWPAPSK:     /*WPA mode,   GTK is TKIP (KeyID=1),    PTK is TKIP */
            pIoTApCfg->MBSSID.WepStatus = Ndis802_11Encryption2Enabled;
            break;
        case Ndis802_11AuthModeWPA2PSK:  /*WPA2 mode,  GTK is AES (KeyID=1),   PTK is AES */
            pIoTApCfg->MBSSID.WepStatus = Ndis802_11Encryption3Enabled;
            break;
        case Ndis802_11AuthModeWPA1PSKWPA2PSK: /*Mixed mode,   GTK is TKIP (KeyID=1),    PTK is AES */
            pIoTApCfg->MBSSID.WepStatus = Ndis802_11Encryption4Enabled;
            break;
        default:
            pIoTApCfg->MBSSID.AuthMode  = Ndis802_11AuthModeOpen;
            pIoTApCfg->MBSSID.WepStatus = Ndis802_11EncryptionDisabled;
            break;
    }
    pIoTApCfg->MBSSID.GroupKeyWepStatus  =  pIoTApCfg->MBSSID.WepStatus;

    if (pIoTApCfg->MBSSID.AuthMode >= Ndis802_11AuthModeWPA)  {
        /*Deriver PMK by AP 's SSID and Password*/
        uint8 keyMaterial[40] = {0};

        printf_high("PMK Updating ...\n");
        RtmpPasswordHash((pchar)pIoTApCfg->MBSSID.Passphase, pIoTApCfg->MBSSID.Ssid,
                         (int32)pIoTApCfg->MBSSID.SsidLen, keyMaterial);
        memcpy(pIoTApCfg->MBSSID.PMK, keyMaterial, LEN_PMK);
    }
}

/*
    ========================================================================

    Routine Description:
        Initialize port configuration structure

    Arguments:
        Adapter                        Pointer to our adapter

    Return Value:
        None

    IRQL = PASSIVE_LEVEL

    Note:

    ========================================================================
*/
void iot_apcfg_preinit(void)
{
    uint32 i=0;
    uint8 apmode = DEFAULT_AP_MODE;

    printf("--> iot_apcfg_preinit\n");

    NdisMoveMemory(pIoTApCfg->MBSSID.Ssid, Default_Ssid, sizeof(Default_Ssid)-1 );
    pIoTApCfg->MBSSID.Ssid[MAX_LEN_OF_SSID]    = '\0';

    pIoTApCfg->MBSSID.SsidLen             = sizeof(Default_Ssid) - 1;
    pIoTApCfg->Aid                                = 1;
    pIoTApCfg->DtimPeriod                     = DEFAULT_DTIM_PERIOD;

    switch (apmode) {
        case AP_MODE_OPEN: /*OPEN Mode*/
            pIoTApCfg->MBSSID.AuthMode               = Ndis802_11AuthModeOpen;
            pIoTApCfg->MBSSID.WepStatus              = Ndis802_11EncryptionDisabled;
            break;
        case AP_MODE_WPAPSK_TKIP:   /*WPA mode,   GTK is TKIP (KeyID=1),   PTK is TKIP */
            pIoTApCfg->MBSSID.AuthMode               = Ndis802_11AuthModeWPAPSK;
            pIoTApCfg->MBSSID.WepStatus              = Ndis802_11Encryption2Enabled;
            break;
        case AP_MODE_WPA2PSK_AES:   /*WPA2 mode,  GTK is AES (KeyID=1),   PTK is AES */
            pIoTApCfg->MBSSID.AuthMode               = Ndis802_11AuthModeWPA2PSK;
            pIoTApCfg->MBSSID.WepStatus              = Ndis802_11Encryption3Enabled;
            break;
        case AP_MODE_WPA1WPA2PSK_TKIPAESMIX: /*Mixed mode,   GTK is TKIP (KeyID=1),   PTK is AES */
            pIoTApCfg->MBSSID.AuthMode               = Ndis802_11AuthModeWPA1PSKWPA2PSK;
            pIoTApCfg->MBSSID.WepStatus              = Ndis802_11Encryption4Enabled;
            break;
        default:
            break;
    }

    pIoTApCfg->MBSSID.GroupKeyWepStatus     = pIoTApCfg->MBSSID.WepStatus;
    pIoTApCfg->MBSSID.DefaultKeyId            = 0;
    memcpy(pIoTApCfg->MBSSID.Passphase, Default_Password, sizeof(Default_Password)-1);
    pIoTApCfg->MBSSID.PassphaseLen = sizeof(Default_Password)-1;

    /*  part I. intialize common configuration*/
    pIoTApCfg->CommonCfg.BasicRateBitmap     = 0xF;
    pIoTApCfg->CommonCfg.BasicRateBitmapOld = 0xF;

    pIoTApCfg->CommonCfg.Dsifs                 = 10;             /* in units of usec */
    pIoTApCfg->CommonCfg.TxPower            = 100;             /* mW*/
    pIoTApCfg->CommonCfg.TxPowerPercentage    = 0xffffffff;    /* AUTO*/
    pIoTApCfg->CommonCfg.TxPowerDefault  = 0xffffffff;     /* AUTO*/
    pIoTApCfg->CommonCfg.TxPreamble       = Rt802_11PreambleLong; /* use Long preamble on TX by defaut*/
    pIoTApCfg->CommonCfg.bUseZeroToDisableFragment = FALSE;
    pIoTApCfg->CommonCfg.UseBGProtection = 0;            /* 0: AUTO*/
    pIoTApCfg->CommonCfg.bEnableTxBurst  = TRUE;            /* 0;        */

    //Arron Added
    pIoTApCfg->CommonCfg.Channel             = 8;
    pIoTApCfg->CommonCfg.bCountryFlag      = FALSE;
    pIoTApCfg->CommonCfg.bUseShortSlotTime     = TRUE;
    /* PHY specification*/
    pIoTApCfg->CommonCfg.PhyMode             = PHY_11BG_MIXED; /* default PHY mode*/
    pIoTApCfg->CommonCfg.SavedPhyMode    = pIoTApCfg->CommonCfg.PhyMode;

    pIoTApCfg->CommonCfg.BeaconPeriod      = 100;             /* in mSec*/

#ifdef BNC_UPDATE_PERIOD  /*Default turn off,  will cause system halt*/
    pIoTApCfg->CommonCfg.BeaconFactor = 0xffffffff / (pIoTApCfg->CommonCfg.BeaconPeriod << 10);
    pIoTApCfg->CommonCfg.BeaconRemain = (0xffffffff % (pIoTApCfg->CommonCfg.BeaconPeriod << 10)) + 1;
#endif

    /*chip cap*/
    pIoTApCfg->WcidHwRsvNum                     = 127;

    //pIoTApCfg->MBSSID.WpaMixPairCipher         = MIX_CIPHER_NOTUSE;

    /* Default MCS as AUTO*/
    pIoTApCfg->MBSSID.DesiredTransmitSetting.field.MCS = MCS_AUTO;  //JCB maybe use fixed MCS

    /* Default is zero. It means no limit.*/
    pIoTApCfg->MBSSID.MaxStaNum             = 0;
    pIoTApCfg->MBSSID.StaCount                 = 0;
    //pIoTApCfg->MBSSID.StationKeepAliveTime  = MAC_TABLE_STA_ALIVE_TIME;

    pIoTApCfg->TXWISize                     = 20;

    for (i = 0; i < WLAN_MAX_NUM_OF_TIM; i ++) {
        pIoTApCfg->MBSSID.TimBitmaps[i] = 0;
    }

}

void iot_apcfg_update(uint8 *pSSID, uint8 AuthMode, uint8 *pPassword, uint8 Channel)
{
    uint8 SSIDLen=0, PSWLen=0;

    /*Delete all MAC Entrys in the Entry Table, and send deauth to the connected Client*/
    MacTableReset();

    /*update ssid*/
    if (NULL != pSSID) {
        SSIDLen = (uint8)strlen((char*)pSSID);
        if (SSIDLen != 0) {
            if (SSIDLen > MAX_SSID_LEN) {
                SSIDLen = MAX_SSID_LEN;
            }
            pIoTApCfg->MBSSID.SsidLen = SSIDLen;
            NdisMoveMemory(pIoTApCfg->MBSSID.Ssid, pSSID, SSIDLen);
        }
    }

    /*updata authmode*/
    if (AuthMode < Ndis802_11AuthModeMax) {
        pIoTApCfg->MBSSID.AuthMode = AuthMode;
    }

    /*updata password*/
    if (NULL != pPassword) {
        PSWLen = strlen((char*)pPassword);
        if (PSWLen != 0) {
            if (PSWLen > MAX_SSID_PASS_LEN) {
                PSWLen = MAX_SSID_PASS_LEN;
            }
            pIoTApCfg->MBSSID.PassphaseLen= PSWLen;

            NdisZeroMemory(pIoTApCfg->MBSSID.Passphase, sizeof(pIoTApCfg->MBSSID.Passphase));
            NdisMoveMemory(pIoTApCfg->MBSSID.Passphase, pPassword, PSWLen);

            //printf_high("%s %d PassphaseLen= %d \n",__FUNCTION__,__LINE__,pIoTApCfg->MBSSID.PassphaseLen);
            //dump(pIoTApCfg->MBSSID.Passphase, pIoTApCfg->MBSSID.PassphaseLen);
        }
    }

    /*updata Channel*/
    if (0 != Channel)  {
        pIoTApCfg->CommonCfg.Channel = Channel;
    }
    
    pIoTMlme->TcpInit = FALSE;  /*in order to do TCP_Init() after do AP config change*/
    iot_ap_startup();
}
#endif   //CONFIG_SOFTAP
