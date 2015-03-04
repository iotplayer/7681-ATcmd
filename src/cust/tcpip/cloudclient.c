#include "uip-conf.h"
#include "uip.h"
#include "webclient.h"
#include "cloudclient.h"
#include "iot_api.h"
#include "crypt_md5.h"

#if (ATCMD_RECOVERY_SUPPORT==0)
#if UIP_CLOUD_SERVER_SUPPORT
#define pageSize        128
#define bufSize         128
#define imgMajorVersion 1
#define imgMinorVersion 60
uint8 productID[16]= {0};
uint8 productKey[16]= {0};

uint8 HWID[16]={77,84,75,48,48,48,48,48,50,48,49,52,48,56,49,53};
uint8 AES_CBC_IV[16] = {0x61,0x33,0x46,0x68,0x55,0x38,0x31,0x43,0x77,0x68,0x36,0x33,0x50,0x76,0x33,0x46};
uint8 FOTA_AES_CBC_IV[16] = {0};
extern ClientActivationInfo mt76xx_Activation;
ClientConnectInfo mt76xx_Connection;
uint16 mt76xx_cloudClientState = 0;
uint16 TCP_cloudClientPort = 0;
#define CTRHEADER_NOENCRYPT_LEN 12
uint16 keepAlivePeriod = 80; //every 40s
uint16 gpioUpdatePeriod = 60;
uint16 keepAliveTimeCnt = 0;
uint16 keepAliveResTimeOut = 0;
uint16 keepAliveRepeatCnt = 0;
uint16 TcpConnectTimeCnt = 0;
uint16 gpioUpdateTimeCnt = 15;
uint16 FOTAUpdateTimeCnt = 0;
#if (IOT_PWM_SUPPORT == 1 && IOT_PWM_TYPE == 2)
extern HWTimerPWMInfo mt7681_PWMInfo;
#endif
FOTAInfo MT7681_FOTAInfo;
MD5_CTX_STRUC FOTA_md5_ctx;
uint8 FOTA_updateOk[FLASH_COM_CFG_CLOUD_FOTA_UPDATEFLAG_LEN] = {0x22,0x33,0x44,0x55};
uint8 encryptBuf[bufSize]={0};
uint8 FOTA_DigestMessage[16];

#define MT76XX_CLOUD_DEBUG 0
#if MT76XX_CLOUD_DEBUG
#define Cloud_Debug printf_high
#else
#define Cloud_Debug(...)
#endif

uint32 string_to_hex(uint8 *inData, uint32 inDataLen, uint8 *outData, uint32 *outDataLen) XIP_ATTRIBUTE(".xipsec1");
uint32 hex_to_string(uint8 *inData, uint32 inDataLen, uint8 *outData) XIP_ATTRIBUTE(".xipsec1");
void cloud_client_apk_encrypt (uint8 *inData,uint32 inDataLen,uint8 *outData,uint32 *outDataLen) XIP_ATTRIBUTE(".xipsec1");
void cloud_client_apk_decrypt (uint8 *inData,uint32 inDataLen,uint8 *outData,uint32 *outDataLen) XIP_ATTRIBUTE(".xipsec1");
void cloud_client_server_encrypt (uint8 *inData,uint32 inDataLen,uint8 *outData,uint32 *outDataLen) XIP_ATTRIBUTE(".xipsec1");
void cloud_client_server_decrypt (uint8 *inData,uint32 inDataLen,uint8 *outData,uint32 *outDataLen) XIP_ATTRIBUTE(".xipsec1");
void cloud_fota_decrypt (uint8 *inData,uint32 inDataLen,uint8 *outData,uint32 *outDataLen) XIP_ATTRIBUTE(".xipsec1");
uint16 cloud_command_parse(char *data, uint16 length) XIP_ATTRIBUTE(".xipsec1");
uint16 cloud_client_resp_apk(uint16 ret) XIP_ATTRIBUTE(".xipsec1");
uint8 cloud_tcp_connect(char *host, uint16 port) XIP_ATTRIBUTE(".xipsec1");
uint16 cloud_http_get(uint8 *action, uint8 *message) XIP_ATTRIBUTE(".xipsec1");
uint32 cloud_connect_process(void) XIP_ATTRIBUTE(".xipsec1");
void cloud_client_ctr_header(char *data,uint32 type,uint32 subtype, uint16 sequence) XIP_ATTRIBUTE(".xipsec1");
void cloud_client_ctr_body(char *data,uint8 dataType) XIP_ATTRIBUTE(".xipsec1");
void cloud_client_send_keepalive(void) XIP_ATTRIBUTE(".xipsec1");
void cloud_client_gpio_autoupdate(void) XIP_ATTRIBUTE(".xipsec1");
void cloud_client_uart_autoupdate(void) XIP_ATTRIBUTE(".xipsec1");

void cloud_client_resp_state(uint8 type, uint8 subtype,uint16 dataType,uint16 dataLen,uint8* result) XIP_ATTRIBUTE(".xipsec1");
uint32 cloud_client_function_packet(DataHeader* Dataheader, uint8 subType) XIP_ATTRIBUTE(".xipsec1");
uint32 cloud_client_manage_packet(DataHeader* Dataheader, uint8 subType) XIP_ATTRIBUTE(".xipsec1");
uint32 cloud_client_data_handler(char *data, uint16 len) XIP_ATTRIBUTE(".xipsec1");
void cloudClient_tcpAppcall(void) XIP_ATTRIBUTE(".xipsec1");

uint32 string_to_hex(uint8 *inData, uint32 inDataLen, uint8 *outData, uint32 *outDataLen) {
    uint8 *inPointer = inData;
    uint8 *outPointer = outData;
    uint8 tmp[4] = {0};
    char *endptr;
    uint32 count = 0;
    uint32 length = inDataLen;
    while ((*inPointer != '\0')&&(*(inPointer+1) != '\0')&&length) {
        strncpy(tmp, inPointer, 2);
        *outPointer = (uint8)simple_strtoul(tmp, &endptr, 16);
        inPointer += 2;
        length -= 2;
        outPointer ++;
        count ++;
    }
    *outDataLen = count;
    *outPointer = '\0';
    return 0;
}

uint32 hex_to_string(uint8 *inData, uint32 inDataLen, uint8 *outData) {
    uint8 * inPointer = inData;
    uint8 * outPointer = outData;
    uint32 number;
    for (number = 0; number < inDataLen; number++) {
        sprintf(outPointer, "%02x", *(inPointer+number));
        outPointer += 2;
    }
    *outPointer = '\0';
    return 0;
}

void cloud_client_apk_encrypt (uint8 *inData,uint32 inDataLen,uint8 *outData,uint32 *outDataLen) {
    aes_cbc_encrypt(inData, inDataLen, HWID, sizeof(HWID), AES_CBC_IV, sizeof(AES_CBC_IV), outData, outDataLen);
}

void cloud_client_apk_decrypt (uint8 *inData,uint32 inDataLen,uint8 *outData,uint32 *outDataLen) {
    aes_cbc_decrypt(inData, inDataLen, HWID, sizeof(HWID), AES_CBC_IV, sizeof(AES_CBC_IV), outData, outDataLen);
}

void cloud_client_server_encrypt (uint8 *inData,uint32 inDataLen,uint8 *outData,uint32 *outDataLen) {
    aes_cbc_encrypt(inData, inDataLen, mt76xx_Activation.DeviceKey, 16, AES_CBC_IV, sizeof(AES_CBC_IV), outData, outDataLen);
}

void cloud_client_server_decrypt (uint8 *inData,uint32 inDataLen,uint8 *outData,uint32 *outDataLen) {
    aes_cbc_decrypt(inData, inDataLen, mt76xx_Activation.DeviceKey, 16, AES_CBC_IV, sizeof(AES_CBC_IV), outData, outDataLen);
}

void cloud_fota_decrypt (uint8 *inData,uint32 inDataLen,uint8 *outData,uint32 *outDataLen) {
    aes_cbc_decrypt(inData, inDataLen, mt76xx_Activation.DeviceKey, 16, FOTA_AES_CBC_IV, sizeof(FOTA_AES_CBC_IV), outData, outDataLen);
}

uint16 cloud_command_parse(char *data, uint16 length) {
    char *pBegin;
    char *pEnd;
    uint16 commaCount = 0,dataCount = 0;
    char tmp[96];
    char activeCmd[]="Activation";
    char activeCommand[16] = {0};
    char *endptr;
    if (!data || !length) {
        Cloud_Debug("%s,parameter error!\n",__func__);
        return 3;
    }
    pBegin = data;
    for (pEnd = data; (*pEnd != ',')&&(*pEnd != '\0'); pEnd++)
        dataCount ++;
    if (*pEnd == ',') {
        commaCount ++;
        strncpy(activeCommand, pBegin, dataCount);
        Cloud_Debug("ActiveCommand:%s\n",activeCommand);
        if (memcmp(activeCommand, activeCmd, sizeof(activeCmd))) {
            Cloud_Debug("error,activecommand:%s\n", activeCommand);
            return 1;
        }
    } else if (*pEnd == '\0') {
        goto end;
    }
    pEnd += 1;
    pBegin = pEnd;
    dataCount = 0;
    for (; (*pEnd != ',')&&(*pEnd != '\0'); pEnd++)
        dataCount ++;
    if (!dataCount) {
        Cloud_Debug("no deviceID\n");
        return 2;
    }
    if (*pEnd == ',') {
        commaCount ++;
        strncpy(mt76xx_Activation.DeviceID, pBegin, dataCount);
        Cloud_Debug("DeviceID:%s\n", mt76xx_Activation.DeviceID);
    } else if (*pEnd == '\0') {
        goto end;
    }
    pEnd += 1;
    pBegin = pEnd;
    dataCount = 0;
    for (; (*pEnd != ',')&&(*pEnd != '\0'); pEnd++)
        dataCount ++;
    if (*pEnd == ',') {
        commaCount ++;
        strncpy(mt76xx_Activation.ServerURL, pBegin, dataCount);
        Cloud_Debug("ServerURL:%s\n",mt76xx_Activation.ServerURL);
    } else if (*pEnd == '\0') {
        goto end;
    }
    pEnd += 1;
    pBegin = pEnd;
    dataCount = 0;
    for (; (*pEnd != ',')&&(*pEnd != '\0'); pEnd++)
        dataCount ++;
    if (!dataCount) {
        Cloud_Debug("no deviceKey\n");
        return 2;
    }
    if (*pEnd == ',') {
        commaCount ++;
        strncpy(mt76xx_Activation.DeviceKey, pBegin, dataCount);
        Cloud_Debug("deviceKey:%s\n", mt76xx_Activation.DeviceKey);
    } else if (*pEnd == '\0') {
        goto end;
    }
    pEnd += 1;
    pBegin = pEnd;
    dataCount = 0;
    for (; (*pEnd != ',')&&(*pEnd != '\0'); pEnd++)
        dataCount ++;
    if (*pEnd == ',') {
        commaCount ++;
        strncpy(mt76xx_Activation.UserID, pBegin, dataCount);
        Cloud_Debug("UserID:%s\n", mt76xx_Activation.UserID);
    }
    if (*pEnd == '\0') {
        goto end;
    }
    pEnd += 1;
    pBegin = pEnd;
    dataCount = 0;
    for (; (*pEnd != ',')&&(*pEnd != '\0'); pEnd++) {
        dataCount ++;
    }
    if (*pEnd == '\0') {
        strncpy(tmp, pBegin, dataCount);
        mt76xx_Activation.ForceFlag = (uint16)simple_strtoul(tmp, &endptr, 10);
        Cloud_Debug("ForceFlag:%d,dataCount:%d\n", mt76xx_Activation.ForceFlag, dataCount);
    } else if (*pEnd == ',') {
        commaCount ++;
        pEnd += 1;
        for (; *pEnd != '\0'; pEnd++) {
            if (*pEnd == ',')
                commaCount ++;
        }
    }
end:
    if (commaCount != 5) {
        Cloud_Debug("error, comma count:%d\n", commaCount);
        return 1;
    }
    return 0;
}

uint16 cloud_client_resp_apk(uint16 ret) {
    uint8 result[8] = {0};
    uint8 output[64] = {0};
    uint32 outputLen = 64;
    uint8 outHexString[64] = {0};
    uint8 *pointer;
    uint16 number1 = 0;
    if (!ret)
        result[0] = '1';
    else if (ret == 1)
        strncpy(result, "0,01", 4);
    else if (ret == 2)
        strncpy(result, "0,02", 4);
    else if (ret == 3)
        strncpy(result, "0,03", 4);
    else
        return 1;
    cloud_client_apk_encrypt(result, strlen(result), output, &outputLen);
    /*
    for(number1 = 0; number1<outputLen; number1++)
    {
      printf_high("%02x ", *(output+number1));
      if((number1+1)%16 == 0)
        printf_high("\n");
    }
    printf_high("\n");
     */
    pointer = outHexString;
    for (number1 = 0; number1 <outputLen; number1++) {
        sprintf(pointer, "%02x", *(output+number1));
        pointer += 2;
    }
    *pointer = '\0';
    uip_send(outHexString, strlen(outHexString));
    return 0;
}

uint8 cloud_tcp_connect(char *host, uint16 port) {
    UIP_CONN *conn;
    uip_ipaddr_t *ipaddr;
    static uip_ipaddr_t addr;
    /* First check if the host is an IP address. */
    ipaddr = &addr;
    if (uiplib_ipaddrconv(host, (unsigned char *)addr) == 0) {
        ipaddr = (uip_ipaddr_t *)resolv_lookup(host);
        if (ipaddr == NULL) {
            return 0;
        }
    }
    conn = uip_connect(ipaddr, htons(port));
    if (conn == NULL) {
        Cloud_Debug("cloud client connect fail!\n");
        return 0;
    } else {
        TCP_cloudClientPort = (uint16)iot_get_ms_time();
        conn->lport = htons(TCP_cloudClientPort);
    }
    uip_listen(HTONS(TCP_cloudClientPort));
    return 1;
}

uint16 cloud_set_activation_para(void) {
    uint8 *pointer;
    uint8 activationPara[96] = {0};
    uint16 num = 0;
    mt76xx_Activation.ActivedFlag = 1;
    pointer = activationPara;
    memcpy(pointer, mt76xx_Activation.DeviceID, FLASH_COM_CFG_CLOUD_DEVID_LEN);
    pointer += FLASH_COM_CFG_CLOUD_DEVID_LEN;
    memcpy(pointer, mt76xx_Activation.DeviceKey, FLASH_COM_CFG_CLOUD_DEVKEY_LEN);
    //dump(activationPara, sizeof(activationPara));
    pointer += FLASH_COM_CFG_CLOUD_DEVKEY_LEN;
    memcpy(pointer, &mt76xx_Activation.ActivedFlag, FLASH_COM_CFG_CLOUD_ACTIVE_LEN);
    pointer += FLASH_COM_CFG_CLOUD_ACTIVE_LEN;
    memcpy(pointer, &mt76xx_Activation.ServerURL, FLASH_COM_CFG_CLOUD_SERVER_URL_LEN);
    num = FLASH_COM_CFG_CLOUD_DEVID_LEN+FLASH_COM_CFG_CLOUD_DEVKEY_LEN
          +FLASH_COM_CFG_CLOUD_ACTIVE_LEN+FLASH_COM_CFG_CLOUD_SERVER_URL_LEN;
    spi_flash_write(FLASH_COM_CFG_BASE+FLASH_COM_CFG_CLOUD_DEVID, activationPara, num);
}

uint16 cloud_http_get(uint8 *action, uint8 *message) {
    uint8 activationDir[256]= {0};
    uint8 inData[32]= {0};
    uint32 inDataLen = 32;
    uint8 outData[64]= {0};
    uint32 outDataLen = 64;
    uint8 outDataString[128]= {0};
    
    sprintf(inData, "%s,%d,%d", message, 1, 2);
    cloud_client_server_encrypt(inData, sizeof(inData)-1, outData, &outDataLen);
    hex_to_string(outData, outDataLen,outDataString);
    sprintf(activationDir, "/devcloud/device/%s/%s/%s", mt76xx_Activation.DeviceID,action,outDataString);
    Cloud_Debug("activationDir:%s\n", activationDir);
    webclient_get("54.255.221.85", HTTP_SERVER_DEFAULT_PORT, activationDir);
}

uint32 cloud_connect_process(void) {
    mt76xx_cloudClientState = CLOUD_CLIENT_CONNECT;
    memset(&mt76xx_Connection, 0 , sizeof(ClientConnectInfo));
    mt76xx_Connection.FirstSendKeepAlive = 1;
    cloud_http_get("connect", mt76xx_Activation.DeviceID);
}

uint32 cloud_activation_process(uint8 *input, uint32 length) {
    uint8 *inDataTmp = input;
    uint8 AES_output[128]= {0};
    uint32 AES_outputLen = 128;
    uint16 ret = 0;
    uint8 inData[128] = {0};
    uint32 inputLen = 128;
    uint8 activeInfo[96] = {0};
    uint16 i;
    uint8 *pointer;
    uint8 info[32] = {0}; 
    
    if ((inDataTmp == NULL) || (length == 0)) {
        Cloud_Debug("%s input parameter error!\n", __func__);
        return 2;
    }
    i = FLASH_COM_CFG_CLOUD_DEVID_LEN+FLASH_COM_CFG_CLOUD_DEVKEY_LEN
        +FLASH_COM_CFG_CLOUD_ACTIVE_LEN+FLASH_COM_CFG_CLOUD_SERVER_URL_LEN;
    spi_flash_read(FLASH_COM_CFG_BASE+FLASH_COM_CFG_CLOUD_DEVID, activeInfo, i);
    memset(&mt76xx_Activation, 0, sizeof(ClientActivationInfo));
    pointer = activeInfo+FLASH_COM_CFG_CLOUD_DEVID_LEN+FLASH_COM_CFG_CLOUD_DEVKEY_LEN;
    mt76xx_Activation.ActivedFlag = *((uint16 *)pointer);
    pointer += FLASH_COM_CFG_CLOUD_ACTIVE_LEN;
    memcpy(mt76xx_Activation.ServerURL, pointer, FLASH_COM_CFG_CLOUD_SERVER_URL_LEN);
    if ((!memcmp(mt76xx_Activation.ServerURL, "http", 4))&&(mt76xx_Activation.ActivedFlag == 1)) {
        Cloud_Debug("Active ok, please not do again!\n");
        return 1;
    } else {
        mt76xx_cloudClientState = CLOUD_CLIENT_ACTIVE;
        string_to_hex(inDataTmp,length,inData,&inputLen);
        cloud_client_apk_decrypt(inData,inputLen,AES_output,&AES_outputLen);
        //dump(AES_output, AES_outputLen);
        ret = cloud_command_parse(AES_output, AES_outputLen);
        cloud_client_resp_apk(ret);
        if (mt76xx_Activation.ForceFlag == 1) {
            cloud_set_activation_para();
            cloud_connect_process();
        } else {
            sprintf(info, "%s,%s", mt76xx_Activation.UserID, productKey);
            cloud_http_get("verifyRegistration", info);
        }
        return 0;
    }
}

uint32 cloud_para_check_connect(void) {
    uint8 activeInfo[96] = {0};
    uint8 cloudConnectServerIP[16] = {0};
    uint16 cloudConnectServerPort = 0;
    uint16 connectFlag = 0, i, dotCount = 0;
    uint8 *pointer;
    i = FLASH_COM_CFG_CLOUD_DEVID_LEN+FLASH_COM_CFG_CLOUD_DEVKEY_LEN
        +FLASH_COM_CFG_CLOUD_ACTIVE_LEN+FLASH_COM_CFG_CLOUD_SERVER_URL_LEN
        +FLASH_COM_CFG_CLOUD_PRODUCT_ID_LEN+FLASH_COM_CFG_CLOUD_PRODUCT_KEY_LEN;
    spi_flash_read(FLASH_COM_CFG_BASE+FLASH_COM_CFG_CLOUD_DEVID, activeInfo, i);
    memset(&mt76xx_Activation, 0, sizeof(ClientActivationInfo));
    pointer = activeInfo;
    memcpy(mt76xx_Activation.DeviceID, pointer, FLASH_COM_CFG_CLOUD_DEVID_LEN);
    pointer += FLASH_COM_CFG_CLOUD_DEVID_LEN;
    memcpy(mt76xx_Activation.DeviceKey, pointer, FLASH_COM_CFG_CLOUD_DEVKEY_LEN);
    pointer += FLASH_COM_CFG_CLOUD_DEVKEY_LEN;
    mt76xx_Activation.ActivedFlag = *((uint16 *)pointer);
    pointer += FLASH_COM_CFG_CLOUD_ACTIVE_LEN;
    memcpy(mt76xx_Activation.ServerURL, pointer, FLASH_COM_CFG_CLOUD_SERVER_URL_LEN);
    pointer += FLASH_COM_CFG_CLOUD_SERVER_URL_LEN;

    memcpy(mt76xx_Activation.ProductID, pointer, FLASH_COM_CFG_CLOUD_PRODUCT_ID_LEN);
    pointer += FLASH_COM_CFG_CLOUD_PRODUCT_ID_LEN;
    if((mt76xx_Activation.ProductID[0] != 0xff) && (mt76xx_Activation.ProductID[0] != 0xff))
      memcpy(productID, mt76xx_Activation.ProductID, FLASH_COM_CFG_CLOUD_PRODUCT_ID_LEN); 
	memcpy(mt76xx_Activation.ProductKey, pointer, FLASH_COM_CFG_CLOUD_PRODUCT_KEY_LEN);
    pointer += FLASH_COM_CFG_CLOUD_PRODUCT_KEY_LEN;
    if((mt76xx_Activation.ProductKey[0] != 0xff) && (mt76xx_Activation.ProductKey[0] != 0xff))
      memcpy(productKey, mt76xx_Activation.ProductKey, FLASH_COM_CFG_CLOUD_PRODUCT_KEY_LEN);

    if (mt76xx_Activation.ActivedFlag == 1) {
      Cloud_Debug("cloud client had beeb actived!\n");
      cloud_connect_process();
      return 0;
    } else {
      Cloud_Debug("cloud client parameter error or no setting, please config it!\n");
      memset(&mt76xx_Activation, 0, sizeof(ClientActivationInfo));
      return 1;
    }
}

uint32 cloud_fota_update(char *data, uint16 len) {
    uint8 FOTAPara[16] = {0},leftover = 0, pages =0;
    char *begin;
    uint32 count = 0, ret = 0, num;
    uint32 stringLen = 0;
    char FOTAOutput[560]= {0};
    uint32 FOTAOutputLen = sizeof(FOTAOutput);
    MT7681_FOTAInfo.ImgTotLen += len;
    Cloud_Debug("get %d bytes file.\n", MT7681_FOTAInfo.ImgTotLen);
    if (len > sizeof(FOTAOutput))
        Cloud_Debug("download %d bytes error.\n", len);
    if (len >MT7681_FOTAInfo.RemainNum) {
        memcpy(encryptBuf+sizeof(encryptBuf)-MT7681_FOTAInfo.RemainNum, data, MT7681_FOTAInfo.RemainNum);
        cloud_fota_decrypt(encryptBuf, sizeof(encryptBuf), FOTAOutput, &FOTAOutputLen);
        memcpy(FOTA_AES_CBC_IV, encryptBuf+(sizeof(encryptBuf)-sizeof(FOTA_AES_CBC_IV)), sizeof(FOTA_AES_CBC_IV));//change IV for next
        RT_MD5_Append(&FOTA_md5_ctx, FOTAOutput, sizeof(encryptBuf));
        MT7681_FOTAInfo.PageHead = MT7681_FOTAInfo.PageEnd;
        MT7681_FOTAInfo.PageEnd += sizeof(encryptBuf)/pageSize;
        if (MT7681_FOTAInfo.PageEnd == 1) { //head page, no need to update
            ret = spi_flash_update_fw(UART_FlASH_UPG_ID_FOTA_FW, 0, FOTAOutput+128, sizeof(encryptBuf)-128);//128 img header
            if (ret)
                Cloud_Debug("spi_flash_update_fw return error, %d line.\n", __LINE__);
        } else {
            ret = spi_flash_update_fw(UART_FlASH_UPG_ID_FOTA_FW, (MT7681_FOTAInfo.PageHead-1)*pageSize, FOTAOutput, sizeof(encryptBuf));
            if (ret)
                Cloud_Debug("spi_flash_update_fw return error, %d line.\n", __LINE__);
        }
        pages = (len-MT7681_FOTAInfo.RemainNum)/pageSize;
        if (pages) {
            FOTAOutputLen = sizeof(FOTAOutput);
            cloud_fota_decrypt(data+MT7681_FOTAInfo.RemainNum, pages*pageSize, FOTAOutput, &FOTAOutputLen);
            memcpy(FOTA_AES_CBC_IV, (data+MT7681_FOTAInfo.RemainNum+pages*pageSize-sizeof(FOTA_AES_CBC_IV)), sizeof(FOTA_AES_CBC_IV));//change IV for next
            RT_MD5_Append(&FOTA_md5_ctx, FOTAOutput, pages*pageSize);
            MT7681_FOTAInfo.PageHead = MT7681_FOTAInfo.PageEnd;
            MT7681_FOTAInfo.PageEnd += pages;
            for (num = 0; num < pages; num++) { //every write 128B.
                ret = spi_flash_update_fw(UART_FlASH_UPG_ID_FOTA_FW, (MT7681_FOTAInfo.PageHead+num-1)*pageSize, FOTAOutput+num*pageSize, pageSize);
                if (ret)
                    Cloud_Debug("spi_flash_update_fw return error, %d line.\n", __LINE__);
            }
        }
        leftover = len-MT7681_FOTAInfo.RemainNum-pages*pageSize;
        if (leftover) {
            memcpy(encryptBuf, data+MT7681_FOTAInfo.RemainNum+pages*pageSize, leftover);
            MT7681_FOTAInfo.RemainNum = sizeof(encryptBuf)-leftover;
        } else
            MT7681_FOTAInfo.RemainNum = sizeof(encryptBuf); //encryptBuf empty.
    } else if (len <MT7681_FOTAInfo.RemainNum) {
        memcpy(encryptBuf+sizeof(encryptBuf)-MT7681_FOTAInfo.RemainNum, data, len);
        MT7681_FOTAInfo.RemainNum -= len;
    } else if (len == MT7681_FOTAInfo.RemainNum) {
        memcpy(encryptBuf+sizeof(encryptBuf)-MT7681_FOTAInfo.RemainNum, data, len);
        cloud_fota_decrypt(encryptBuf, sizeof(encryptBuf), FOTAOutput, &FOTAOutputLen);
        memcpy(FOTA_AES_CBC_IV, encryptBuf+sizeof(encryptBuf)-sizeof(FOTA_AES_CBC_IV), sizeof(FOTA_AES_CBC_IV));//change IV for next
        RT_MD5_Append(&FOTA_md5_ctx, FOTAOutput, sizeof(encryptBuf));
        MT7681_FOTAInfo.PageHead = MT7681_FOTAInfo.PageEnd;
        MT7681_FOTAInfo.PageEnd += sizeof(encryptBuf)/pageSize;
        if (MT7681_FOTAInfo.PageEnd == 1) {
            ret = spi_flash_update_fw(UART_FlASH_UPG_ID_FOTA_FW, 0, FOTAOutput+128, sizeof(encryptBuf)-128);//128 img header
            if (ret)
                Cloud_Debug("spi_flash_update_fw return error, %d line.\n", __LINE__);
        } else {
            ret = spi_flash_update_fw(UART_FlASH_UPG_ID_FOTA_FW, (MT7681_FOTAInfo.PageHead-1)*pageSize, FOTAOutput, sizeof(encryptBuf));
            if (ret)
                Cloud_Debug("spi_flash_update_fw return error, %d line.\n", __LINE__);
        }
        MT7681_FOTAInfo.RemainNum = sizeof(encryptBuf);
    }
    if (MT7681_FOTAInfo.ImgTotLen >= MT7681_FOTAInfo.ImageLen) { //file download complete
        if (MT7681_FOTAInfo.ImgTotLen%16 != 0) {
            Cloud_Debug("error get %d bytes.\n", MT7681_FOTAInfo.ImgTotLen);
            return 1;
        } else
            Cloud_Debug("get %d bytes file.\n", MT7681_FOTAInfo.ImgTotLen);
        if (MT7681_FOTAInfo.RemainNum < sizeof(encryptBuf)) {
            Cloud_Debug("remainNum < sizeof(encryptBuf), remainNum:%d\n", MT7681_FOTAInfo.RemainNum);
            FOTAOutputLen = sizeof(FOTAOutput);
            cloud_fota_decrypt(encryptBuf, sizeof(encryptBuf)-MT7681_FOTAInfo.RemainNum, FOTAOutput, &FOTAOutputLen);
            Cloud_Debug("BEFORE RT_MD5_Append, bufOutput_len:%d\n", FOTAOutputLen);
            RT_MD5_Append(&FOTA_md5_ctx, FOTAOutput, FOTAOutputLen);
            RT_MD5_End(&FOTA_md5_ctx, FOTA_DigestMessage);
            MT7681_FOTAInfo.PageHead = MT7681_FOTAInfo.PageEnd;
            ret = spi_flash_update_fw(UART_FlASH_UPG_ID_FOTA_FW, (MT7681_FOTAInfo.PageHead-1)*pageSize, FOTAOutput, FOTAOutputLen);
            if (ret)
                Cloud_Debug("spi_flash_update_fw return error, %d line.\n", __LINE__);
            memset(encryptBuf, 0, sizeof(encryptBuf));
            MT7681_FOTAInfo.ImgTotLen = MT7681_FOTAInfo.ImgTotLen-(sizeof(encryptBuf)-MT7681_FOTAInfo.RemainNum)+FOTAOutputLen;
        } else if (MT7681_FOTAInfo.RemainNum == sizeof(encryptBuf)) {
            memset(encryptBuf, 0, sizeof(encryptBuf));
            RT_MD5_End(&FOTA_md5_ctx, FOTA_DigestMessage);
        }
        if (MT7681_FOTAInfo.ImgTotLen != MT7681_FOTAInfo.ImageLen) {
            Cloud_Debug("download size(%d) error,file size(%d).\n", MT7681_FOTAInfo.ImgTotLen, MT7681_FOTAInfo.ImageLen);
            return 1;
        }
        ret = memcmp(FOTA_DigestMessage, MT7681_FOTAInfo.ImageMD5, sizeof(FOTA_DigestMessage));
        if (!ret) {
            begin = FOTAPara;
            Cloud_Debug("%s,MT7681_FOTAInfo.MajorVersion:%d\n",__func__, MT7681_FOTAInfo.MajorVersion);
            memcpy(begin, &MT7681_FOTAInfo.MajorVersion,FLASH_COM_CFG_CLOUD_FOTA_MAJORVER_LEN);
            begin += FLASH_COM_CFG_CLOUD_FOTA_MAJORVER_LEN;
            Cloud_Debug("%s,MT7681_FOTAInfo.MinorVersion:%d\n",__func__, MT7681_FOTAInfo.MinorVersion);
            memcpy(begin, &MT7681_FOTAInfo.MinorVersion,FLASH_COM_CFG_CLOUD_FOTA_MINORVER_LEN);
            begin += FLASH_COM_CFG_CLOUD_FOTA_MINORVER_LEN;
            Cloud_Debug("%s,MT7681_FOTAInfo.ImageLen:%d\n",__func__, MT7681_FOTAInfo.ImageLen);
            memcpy(begin, (uint8 *)(&MT7681_FOTAInfo.ImageLen),FLASH_COM_CFG_CLOUD_FOTA_IMGSIZE_LEN);
            begin += FLASH_COM_CFG_CLOUD_FOTA_IMGSIZE_LEN;
            memcpy(begin, FOTA_updateOk,FLASH_COM_CFG_CLOUD_FOTA_UPDATEFLAG_LEN);
            count = FLASH_COM_CFG_CLOUD_FOTA_MAJORVER_LEN+FLASH_COM_CFG_CLOUD_FOTA_MINORVER_LEN
                    +FLASH_COM_CFG_CLOUD_FOTA_IMGSIZE_LEN+FLASH_COM_CFG_CLOUD_FOTA_UPDATEFLAG_LEN;
            spi_flash_write(FLASH_COM_CFG_BASE+FLASH_COM_CFG_CLOUD_FOTA_MAJORVER, FOTAPara, count);
            printf_high("Img update ok!Now system reboot...\n");
            iot_sys_reboot();
        } else {
            Cloud_Debug("download file MD5 error.\n");
            //dump(FOTA_DigestMessage, sizeof(FOTA_DigestMessage));
            //dump(MT7681_FOTAInfo.ImageMD5, 16);
            return 1;
        }
    }
    return 0;
}

uint32 cloud_http_callback(char *data, uint16 len) {
    char *input = data;
    uint8 inputData[64]= {0};
    uint32 inputLen = 64;
    uint8 outputData[96]= {0};
    uint32 outputDataLen = 96;
    char *begin;
    char *endptr;
    uint32 count = 0;
    uint32 dotCount = 0;
    uint8 tmp[4] = {0};
    if (mt76xx_cloudClientState == CLOUD_CLIENT_FOTAUPDATE) {
        cloud_fota_update(input, len);
    } else {
        string_to_hex(input, len, inputData, &inputLen);
        cloud_client_server_decrypt(inputData, inputLen, outputData, &outputDataLen);
        Cloud_Debug("inputLen:%d,outputDataLen:%d\n", inputLen, outputDataLen);
        switch (mt76xx_cloudClientState) {
            case CLOUD_CLIENT_ACTIVE:
                if (!memcmp(outputData,"1,",2)) {
                    cloud_client_resp_apk(0);
                    cloud_set_activation_para();
                    cloud_connect_process();
                } else {
                    Cloud_Debug("server activation result error.\n");
                    cloud_client_resp_apk(3);
                    return 1;
                }
                break;
            case CLOUD_CLIENT_CONNECT:
                begin = outputData;
                for (input = outputData; (*input != 0)&&(*input != ','); input++) {
                    count ++;
                    if (*input == '.')
                        dotCount ++;
                }
                if ((*input == ',') && (dotCount == 3)) {
                    memcpy(mt76xx_Connection.ServerIP, begin, count);
                    Cloud_Debug("get ip:%s from server.\n", mt76xx_Connection.ServerIP);
                } else {
                    Cloud_Debug("server connect return ip format error.\n");
                    return 2;
                }
                input ++;
                begin = input;
                count = 0;
                for (; (*input != 0)&&(*input != ','); input++) {
                    count ++;
                }
                if (*input == ',') {
                    memcpy(tmp, begin, count);
                    mt76xx_Connection.ServerPort = (uint16)simple_strtoul(tmp, &endptr, 10);
                    printf("get port:%d from server.\n", mt76xx_Connection.ServerPort);
                } else {
                    Cloud_Debug("server connect return port format error.\n");
                    return 2;
                }
                mt76xx_cloudClientState = CLOUD_CLIENT_COMMAND;
                FOTAUpdateTimeCnt = 0;
                cloud_tcp_connect(mt76xx_Connection.ServerIP, mt76xx_Connection.ServerPort);
                break;
            case CLOUD_CLIENT_COMMAND:
            default:
                //printf_high("error, mt76xx_cloudClientState:%d\n", mt76xx_cloudClientState);
                break;
        }
        return 0;
    }
}

void cloud_client_ctr_header(char *data,uint32 type,uint32 subtype, uint16 sequence) {
    CloudClientCtrlHeader header;

    memset(&header, 0, sizeof(CloudClientCtrlHeader));
    header.Magic = 0x76814350;
    memcpy(header.DeviceID, mt76xx_Activation.DeviceID, 8);
    memcpy(header.DeviceIDCopy, mt76xx_Activation.DeviceID, 8);
    header.CommandID = sequence;
    header.Reserve = 0xf;
    header.Type = type&0xf;
    header.SubType = (uint8)subtype&0xff;

	memcpy(data, &header, sizeof(CloudClientCtrlHeader));
}

void cloud_client_ctr_body(char *data,uint8 dataType) {
    DataHeader* Dataheader;
    GPIO_Information *GpioData;
    uint8 gpio_num, polarity, value;
    Dataheader = (DataHeader*)data;
    GpioData = (GPIO_Information *)(data + CP_DATA_HDR_LEN);
    switch (dataType) {
        case AUTO_UPDATE:
            Dataheader->Type = GPIO_INFO;
            Dataheader->Length = 8;
            GpioData->GPIO_List = 0;
            GpioData->GPIO_Value = 0;          
            for (gpio_num = 0; gpio_num< 8; gpio_num++) {
                iot_gpio_read(gpio_num, &value, &polarity);
                GpioData->GPIO_List |= (polarity<<gpio_num);
                GpioData->GPIO_Value |= (value<<gpio_num);
            }
            printf("gpio auto update,GpioData->GPIO_List:0x%x, GpioData->GPIO_Value:0x%x\n", GpioData->GPIO_List,GpioData->GPIO_Value);
            break;
        default:
            return;
    }
}

void cloud_client_send_keepalive(void) {
    char *cptr;
    uint8 output[16] = {0};
    uint32 len, outputLen = 16;
    Cloud_Debug("send keepalive packet,time:%d, mt76xx_Connection.SendSeqID:%d\n", iot_get_ms_time(), mt76xx_Connection.SendSeqID);
    cptr = (char *)uip_appdata;
    if (mt76xx_Connection.FirstSendKeepAlive)
        cloud_client_ctr_header(cptr, MANAGEMENT, KEEP_ALIVE_SEND, 0);
    else
        cloud_client_ctr_header(cptr, MANAGEMENT, KEEP_ALIVE_SEND, ++mt76xx_Connection.SendSeqID);
    cloud_client_server_encrypt(cptr+CTRHEADER_NOENCRYPT_LEN,sizeof(CloudClientCtrlHeader)-CTRHEADER_NOENCRYPT_LEN,output,&outputLen);
    memcpy(cptr+CTRHEADER_NOENCRYPT_LEN, output, outputLen);
    //dump(cptr, CTRHEADER_NOENCRYPT_LEN+outputLen);
    keepAliveTimeCnt = 0;
    TcpConnectTimeCnt = 0;
    keepAliveResTimeOut = 0;
    if (keepAliveRepeatCnt) //if no keepalive response, short keepAlivePeriod to 10s.
        keepAlivePeriod = 20;
    else
        keepAlivePeriod = 80;
    len = CTRHEADER_NOENCRYPT_LEN+outputLen;
    uip_send(uip_appdata, len);
}

void cloud_client_gpio_autoupdate(void) {
    char *cptr;
    uint8 output[32] = {0};
    uint32 len, inputLen, outputLen = 32;
    printf("gpio auto update packet,time:%d\n", GetMsTimer());
    cptr = (char *)uip_appdata;
    cloud_client_ctr_header(cptr, FUNCTION, AUTO_UPDATE, ++mt76xx_Connection.SendSeqID);
    cloud_client_ctr_body(cptr+sizeof(CloudClientCtrlHeader), AUTO_UPDATE);
    inputLen = sizeof(CloudClientCtrlHeader)-CTRHEADER_NOENCRYPT_LEN+CP_DATA_HDR_LEN+sizeof(GPIO_Information);
    cloud_client_server_encrypt(cptr+CTRHEADER_NOENCRYPT_LEN,inputLen,output,&outputLen);
    memcpy(cptr+CTRHEADER_NOENCRYPT_LEN, output, outputLen);
    len = CTRHEADER_NOENCRYPT_LEN+outputLen;
    uip_send(uip_appdata, len);
}

void cloud_client_uart_autoupdate(void) {
    char *cptr;
    char *tlv;
    uint8 output[64] = {0};
    uint32 len, inputLen, outputLen = 64;
    DataHeader* Dataheader;
    uint16  i = 0;
    uint8 pCmdBuf[64] = {0};
    uint16 uart_len = 0,rx_len = 0;
    uart_len = uart_get_avail();
    if (uart_len) {
        rx_len = uart_len>64?64:uart_len;
        for (i=0; i<rx_len; i++)
            pCmdBuf[i] = uart_rb_pop();
        cptr = (char *)uip_appdata;
        Cloud_Debug("cloud_client_uart_autoupdate,pCmdBuf:%s\n", pCmdBuf);
        //dump(pCmdBuf, rx_len);
        cloud_client_ctr_header(cptr, FUNCTION, AUTO_UPDATE, ++mt76xx_Connection.SendSeqID);
        tlv = cptr+sizeof(CloudClientCtrlHeader);
        Dataheader = (DataHeader*)tlv;
        Dataheader->Type = UART_INFO;
        Dataheader->Length = rx_len;
        memcpy(cptr+sizeof(CloudClientCtrlHeader)+CP_DATA_HDR_LEN, pCmdBuf, rx_len);
        inputLen = sizeof(CloudClientCtrlHeader)-CTRHEADER_NOENCRYPT_LEN+CP_DATA_HDR_LEN+rx_len;
        cloud_client_server_encrypt(cptr+CTRHEADER_NOENCRYPT_LEN,inputLen,output,&outputLen);
        memcpy(cptr+CTRHEADER_NOENCRYPT_LEN, output, outputLen);
        len = CTRHEADER_NOENCRYPT_LEN+outputLen;
        uip_send(uip_appdata, len);
    }
}

void cloud_client_fota_autoupdate(void) {
    char *cptr;
    uint8 output[32] = {0};
    uint32 len, inputLen, outputLen = 32;
    DataHeader* Dataheader;
    FOTAInfo *FOTAInformation;
    uint8 fData[1] = {0};
    uint8 version[FLASH_COM_CFG_CLOUD_FOTA_MAJORVER_LEN+FLASH_COM_CFG_CLOUD_FOTA_MINORVER_LEN] = {0};
    spi_flash_read(FLASH_COM_CFG_BASE+FLASH_COM_CFG_BOOT_IDX, fData, FLASH_COM_CFG_BOOT_IDX_LEN);
    len = FLASH_COM_CFG_CLOUD_FOTA_MAJORVER_LEN+FLASH_COM_CFG_CLOUD_FOTA_MINORVER_LEN;
    spi_flash_read(FLASH_COM_CFG_BASE+FLASH_COM_CFG_CLOUD_FOTA_MAJORVER, version, len);
    if ((version[0]==0xff) && (version[1]==0xff)) { //no version information,write default value.
        version[0] = imgMajorVersion;
        version[1] = imgMinorVersion;
        spi_flash_write(FLASH_COM_CFG_BASE+FLASH_COM_CFG_CLOUD_FOTA_MAJORVER, version, len);
    }
    Cloud_Debug("FOTA auto update packet,MajorVersion:%d,MinorVersion:%d\n", version[0],version[1]);
    if ((fData[0] == 0) || (fData[0] == 1)) {
        cptr = (char *)uip_appdata;
        cloud_client_ctr_header(cptr, FUNCTION, AUTO_UPDATE, ++mt76xx_Connection.SendSeqID);
        Dataheader = (DataHeader*)(cptr+sizeof(CloudClientCtrlHeader));
        Dataheader->Type = FOTA_INFO;
        Dataheader->Length = sizeof(FOTAInfo);
        FOTAInformation = (FOTAInfo *)(cptr+sizeof(CloudClientCtrlHeader)+CP_DATA_HDR_LEN);
        FOTAInformation->BootMode = fData[0];
        FOTAInformation->MajorVersion = version[0];
        FOTAInformation->MinorVersion = version[1];
    } else {
        Cloud_Debug("7681 boot mode error %d!", fData[0]);
        return ;
    }
    inputLen = sizeof(CloudClientCtrlHeader)-CTRHEADER_NOENCRYPT_LEN+CP_DATA_HDR_LEN+3;
    cloud_client_server_encrypt(cptr+CTRHEADER_NOENCRYPT_LEN,inputLen,output,&outputLen);
    memcpy(cptr+CTRHEADER_NOENCRYPT_LEN, output, outputLen);
    len = CTRHEADER_NOENCRYPT_LEN+outputLen;
    uip_send(uip_appdata, len);
}

void cloud_client_resp_state(uint8 type, uint8 subtype,uint16 dataType,uint16 dataLen,uint8* result) {
    char *cptr;
    uint8 output[32] = {0};
    uint32 len, inputLen, outputLen = 32;
    DataHeader* Dataheader;
    uint8 *data;
    cptr = (char *)uip_appdata;
    cloud_client_ctr_header(cptr, type, subtype, mt76xx_Connection.ReceiveSeqID);
    Dataheader = (DataHeader*)(cptr+sizeof(CloudClientCtrlHeader));
    Dataheader->Type = dataType;
    Dataheader->Length = dataLen;
    data = (uint8 *)(Dataheader) + CP_DATA_HDR_LEN;
    memcpy(data, result, dataLen);
    inputLen = sizeof(CloudClientCtrlHeader)-CTRHEADER_NOENCRYPT_LEN+CP_DATA_HDR_LEN+dataLen;
    cloud_client_server_encrypt(cptr+CTRHEADER_NOENCRYPT_LEN,inputLen,output,&outputLen);
    memcpy(cptr+CTRHEADER_NOENCRYPT_LEN, output, outputLen);
    len = CTRHEADER_NOENCRYPT_LEN+outputLen;
    uip_send(uip_appdata, len);
}

uint32 cloud_client_function_packet(DataHeader* Dataheader, uint8 subType) {
    char *Data;
    GPIO_Information *GpioData;
    PWM_Descriptor *PWM_Info;
    uint32 GPIO_Value, GPIO_List, i, num, GPIO_Info[2]={0};
    uint16 type, length;
    Data = (char *)(Dataheader) + CP_DATA_HDR_LEN;
    char result[8] ={0};
    uint8 gpio_num, polarity=0, value=0;
    PWM_Descriptor PWM_Status[MAX_PWM_COUNT];
    uint8 fData[1] = {0};
    uint8 HttpServer[16] = {0};
    uint8 version[FLASH_COM_CFG_CLOUD_FOTA_MAJORVER_LEN+FLASH_COM_CFG_CLOUD_FOTA_MINORVER_LEN] = {0};
    FOTAInfo *FOTAInformation;
    Cloud_Debug("enter %s,subType:%d\n", __func__, subType);
    switch (subType) {
        case SET_GPIO_REQUEST:
            GpioData = (GPIO_Information *)Data;
            GPIO_Value = GpioData->GPIO_Value;
            GPIO_List = GpioData->GPIO_List;
            Cloud_Debug("GPIO_SET_REQUEST,GPIO_List:0x%x,GPIO_Value:0x%x\n",GPIO_List,GPIO_Value);
            if (GPIO_Value > 0xff) {
                Cloud_Debug("GPIO_SET_REQUEST,GPIO_Value error!\n");
                result[0] = 1;
                cloud_client_resp_state(FUNCTION,SET_GPIO_RESPONSE,STATUS,1,result);
                return 1;
            } else {
                if (Dataheader->Type == GPIO_INFORMATION) {
                    for (i=0; i < 8; i++) {
                        if (GPIO_List & ((uint32)(1<<i))) {
                            printf("GPIO %d will be set.\n",i);
#if ((IOT_PWM_SUPPORT == 1) && (IOT_PWM_TYPE == 2))
                            /*if this GPIO has been set as PWM, cancel PWM first*/
                            iot_sw_pwm_del(i);
#endif
                            if ((GPIO_Value)&((uint32)1<<i)) {
                                Cloud_Debug("GPIO %d will be set to high.\n",i);
                                iot_gpio_output(i,1);
                            } else {
                                Cloud_Debug("GPIO %d will be set to low.\n",i);
                                iot_gpio_output(i,0);
                            }
                        }
                    }
                }
                result[0] = 0;
                cloud_client_resp_state(FUNCTION,SET_GPIO_RESPONSE,STATUS,1,result);
            }
            break;
        case GET_GPIO_REQUEST:
            for (gpio_num = 0; gpio_num< 8; gpio_num++) {
                iot_gpio_read(gpio_num, &value, &polarity);
                GPIO_Info[0] |= (polarity<<gpio_num);
                GPIO_Info[1] |= (value<<gpio_num);
            }
            cloud_client_resp_state(FUNCTION,GET_GPIO_RESPONSE,GPIO_INFORMATION,8,(uint8 *)GPIO_Info);
            break;
        case SET_PWM_REQUEST:
            PWM_Info = (PWM_Descriptor *)Data;
#if (IOT_PWM_SUPPORT == 1 && IOT_PWM_TYPE == 2)
            if (Dataheader->Type == PWM_INFORMATION) {
                if ((Dataheader->Length%5)==0) { //every control 5Bytes
                    for (length = 0; length<Dataheader->Length; length +=5) {
                        Cloud_Debug("PWM_SET_REQUEST,PWM_Value:%d,PWM_Period:%d\n", PWM_Info->PWM_Value,PWM_Info->PWM_Period);
                        iot_sw_pwm_add(PWM_Info->PWMID, PWM_Info->PWM_Value, PWM_Info->PWM_Period);
                        PWM_Info += 5;
                    }
                    result[0] = 0;
                    cloud_client_resp_state(FUNCTION,SET_PWM_RESPONSE,STATUS,1,result);
                } else {
                    Cloud_Debug("PWM_SET_REQUEST,length(%d) error!\n", Dataheader->Length);
                    result[0] = 1;
                    cloud_client_resp_state(FUNCTION,SET_PWM_RESPONSE,STATUS,1,result);
                    return 1;
                }
            }
#else
            result[0] = 1;
            cloud_client_resp_state(FUNCTION,SET_PWM_RESPONSE,STATUS,1,result);
            return 1;
#endif
            break;
        case GET_PWM_REQUEST:
#if (IOT_PWM_SUPPORT == 1 && IOT_PWM_TYPE == 2)
            num =0;
            memset(PWM_Status, 0, sizeof(PWM_Status));
            for (i=0; i<MAX_PWM_COUNT; i++) {
                if (mt7681_PWMInfo.available[i] == TRUE) {
                    PWM_Status[num].PWMID = i;
                    PWM_Status[num].PWM_Value = mt7681_PWMInfo.on_time[i];
                    PWM_Status[num].PWM_Period = mt7681_PWMInfo.off_time[i]+mt7681_PWMInfo.on_time[i];
                    num ++;
                }
            }
            cloud_client_resp_state(FUNCTION,GET_PWM_RESPONSE,PWM_INFORMATION,num*5,PWM_Status);
#endif
            break;
        case SET_UART_REQUEST:
            Cloud_Debug("UART_SET_REQUEST,Dataheader->Length:%d\n", Dataheader->Length);
            if (Dataheader->Type == UART_INFORMATION) {
                dump((int8 *)Dataheader+CP_DATA_HDR_LEN, Dataheader->Length);
                //iot_uart_output((int8 *)Dataheader+CP_DATA_HDR_LEN, Dataheader->Length);
                result[0] = 0;
                cloud_client_resp_state(FUNCTION,SET_UART_RESPONSE,STATUS,1,result);
            } else {
                Cloud_Debug("UART_SET_REQUEST,Dataheader->Type(%d) error!\n", Dataheader->Type);
                result[0] = 1;
                cloud_client_resp_state(FUNCTION,SET_UART_RESPONSE,STATUS,1,result);
            }
            break;
        case FOTA_START_UPDATE:
            spi_flash_read(FLASH_COM_CFG_BASE+FLASH_COM_CFG_BOOT_IDX, fData, FLASH_COM_CFG_BOOT_IDX_LEN);
            length = FLASH_COM_CFG_CLOUD_FOTA_MAJORVER_LEN+FLASH_COM_CFG_CLOUD_FOTA_MINORVER_LEN;
            spi_flash_read(FLASH_COM_CFG_BASE+FLASH_COM_CFG_CLOUD_FOTA_MAJORVER, version, length);
            if ((Dataheader->Length > (sizeof(FOTAInfo)-64))&&(Dataheader->Length < sizeof(FOTAInfo))) {
                FOTAInformation = (FOTAInfo *)Data;
                memcpy(&MT7681_FOTAInfo, FOTAInformation, Dataheader->Length);
                Cloud_Debug("Download img version %d:%d\n",FOTAInformation->MajorVersion, FOTAInformation->MinorVersion);
                if (FOTAInformation->BootMode == fData[0]) {
                    if (FOTAInformation->MajorVersion > version[0]) {
                    } else if ((FOTAInformation->MajorVersion == version[0])&&(FOTAInformation->MinorVersion > version[1])) {
                    } else {
                        Cloud_Debug("Now img version(%d:%d),download img version(%d:%d) error!\n",
                                    version[0], version[1], FOTAInformation->MajorVersion, FOTAInformation->MinorVersion);
                        result[0] = 1;
                        cloud_client_resp_state(FUNCTION,FOTA_UPDATE_RESPONSE,STATUS,1,result);
                        return 1;
                    }
                } else {
                    Cloud_Debug("FOTA_START_UPDATE, download img mode(%d) error!\n", FOTAInformation->BootMode);
                    result[0] = 1;
                    cloud_client_resp_state(FUNCTION,FOTA_UPDATE_RESPONSE,STATUS,1,result);
                    return 1;
                }
                sprintf((char *)HttpServer, "%d.%d.%d.%d", MT7681_FOTAInfo.ServerIP[0],MT7681_FOTAInfo.ServerIP[1],
                        MT7681_FOTAInfo.ServerIP[2], MT7681_FOTAInfo.ServerIP[3]);
                Cloud_Debug("HttpServer:%s,file size:%d,file path:%s\n", HttpServer, MT7681_FOTAInfo.ImageLen, MT7681_FOTAInfo.FilePath);
                mt76xx_cloudClientState = CLOUD_CLIENT_FOTAUPDATE;
                result[0] = 0;
                cloud_client_resp_state(FUNCTION,FOTA_UPDATE_RESPONSE,STATUS,1,result);
                memcpy(FOTA_AES_CBC_IV, AES_CBC_IV, sizeof(AES_CBC_IV));
                memset(&FOTA_md5_ctx, 0, sizeof(MD5_CTX_STRUC));
                memset(FOTA_DigestMessage, 0, sizeof(FOTA_DigestMessage));
                RT_MD5_Init(&FOTA_md5_ctx);
                memset(encryptBuf, 0, sizeof(encryptBuf));
                MT7681_FOTAInfo.PageHead = MT7681_FOTAInfo.PageEnd = 0;
                MT7681_FOTAInfo.RemainNum = sizeof(encryptBuf);
                MT7681_FOTAInfo.ImgTotLen = 0;
                webclient_get(HttpServer, HTTP_SERVER_DEFAULT_PORT, MT7681_FOTAInfo.FilePath);
            } else {
                Cloud_Debug("FOTA_START_UPDATE, length(%d) error!", Dataheader->Length);
                return 1;
            }
            break;
    }
    return 0;
}

uint32 cloud_client_manage_packet(DataHeader* Dataheader, uint8 subType) {
    char *pointer = (char *)Dataheader;
    uint16 seqID = 0;
    CloudClientCtrlHeader *header;
    char result[8] = {0};
    switch (subType) {
        case KEEP_ALIVE_RESPONSE:
            keepAliveResTimeOut = 0xffff;
            if (keepAliveRepeatCnt <= 2)
                keepAliveRepeatCnt = 0;
            pointer = pointer-sizeof(CloudClientCtrlHeader);
            header = (CloudClientCtrlHeader *)pointer;
            seqID = header->CommandID;
            if (mt76xx_Connection.FirstSendKeepAlive) {
                Cloud_Debug("receive first keep-alive response, seqID:0x%x\n", seqID);
                mt76xx_Connection.ReceiveSeqID = seqID;
                mt76xx_Connection.SendSeqID = seqID;
                mt76xx_Connection.FirstSendKeepAlive = 0;
            } else {
                if (seqID == mt76xx_Connection.SendSeqID)
                    Cloud_Debug("receive server keep-alive response!\n");
                else
                    Cloud_Debug("receive bad keep-alive response,seqID:0x%x,mt76xx_Connection.SendSeqID:0x%x\n", seqID, mt76xx_Connection.SendSeqID);
            }
            break;
        case CONTROL_OFFLINE_REQUEST:
            result[0] = 0;
            cloud_client_resp_state(MANAGEMENT,CONTROL_CLIENT_OFFLINE_RESPONSE,STATUS,1,result);
            reset_sta_cfg();
            iot_linkdown(0);
            break;
    }
    return 0;
}

uint32 cloud_client_data_handler(char *data, uint16 len) {
    char *pointer = data;
    char input[128] = {0};
    uint32 inputLen = 128;
    uint32 tmp;
    CloudClientCtrlHeader *header;
    Cloud_Debug("enter cloud_client_data_handler,len:%d,time:%d\n", len, iot_get_ms_time());
    cloud_client_server_decrypt(pointer+CTRHEADER_NOENCRYPT_LEN,len-CTRHEADER_NOENCRYPT_LEN,input,&inputLen);
    memcpy(pointer+CTRHEADER_NOENCRYPT_LEN, input, inputLen);
    header = (CloudClientCtrlHeader *)pointer;
    Cloud_Debug("cloud receive tcp packet,header->Type:%d,header->SubType:%d\n", header->Type, header->SubType);
    if ((header->Magic != 0x76814350) || memcmp(header->DeviceID,mt76xx_Activation.DeviceID,8)
        || memcmp(header->DeviceID,header->DeviceIDCopy,8)) {
        Cloud_Debug("cloud client receive error packet!\n");
        return 1;
    }
    switch (header->Type) {
        case MANAGEMENT:
            cloud_client_manage_packet(pointer+sizeof(CloudClientCtrlHeader), header->SubType);
            break;
        case FUNCTION:
            if (!mt76xx_Connection.FirstSendKeepAlive) {
                if ((header->CommandID > mt76xx_Connection.ReceiveSeqID)&&(header->CommandID <= (mt76xx_Connection.ReceiveSeqID+20))) {
                } else if (mt76xx_Connection.ReceiveSeqID > 65515) {
                    tmp = mt76xx_Connection.ReceiveSeqID+20;
                    if (header->CommandID > tmp%65536) {
                        Cloud_Debug("sequenceID error!header->CommandID:0x%x,mt76xx_Connection.ReceiveSeqID:0x%x\n",
                                    header->CommandID,mt76xx_Connection.ReceiveSeqID);
                        return 1;
                    }
                } else {
                    Cloud_Debug("sequenceID error2!header->CommandID:0x%x,mt76xx_Connection.ReceiveSeqID:0x%x\n",
                                header->CommandID,mt76xx_Connection.ReceiveSeqID);
                    return 1;
                }
                mt76xx_Connection.ReceiveSeqID = header->CommandID;
            }
            cloud_client_function_packet(pointer+sizeof(CloudClientCtrlHeader), header->SubType);
            break;
    }
}

void cloud_tcp_conn_check(void) {
    TcpConnectTimeCnt ++;
    if (TcpConnectTimeCnt == 160) { //not send keepalive in 80s,tcp disconnect.
        Cloud_Debug("cloud client try connect again!, TcpConnectTimeCnt:%d\n", TcpConnectTimeCnt);
        keepAliveTimeCnt = 0;
        gpioUpdateTimeCnt = 15;
        TcpConnectTimeCnt = 0;
        keepAliveRepeatCnt = 0;
        cloud_para_check_connect();
        return;
    }
    if (mt76xx_cloudClientState == CLOUD_CLIENT_COMMAND) {
        if (keepAliveResTimeOut < 0xffff)
            keepAliveResTimeOut ++;
        if (keepAliveResTimeOut == 10) {
            keepAliveRepeatCnt ++;
            Cloud_Debug("keepalive response timeout!keepAliveRepeatCnt:%d\n", keepAliveRepeatCnt);
            if (keepAliveRepeatCnt == 3) {
                keepAliveTimeCnt = 0;
                gpioUpdateTimeCnt = 15;
                TcpConnectTimeCnt = 0;
                keepAliveRepeatCnt = 0;
                cloud_para_check_connect();
                return;
            }
        }
    }
}

void cloudClient_tcpAppcall(void) {
    int sendKeepAlive_flag = 0;
    int gpioAutoUpdate_flag = 0;
    int FOTAAutoUpdate_flag = 0;
    
    if (uip_connected()) {
        cloud_client_send_keepalive();
        return;
    }
    //if(s.state == WEBCLIENT_STATE_CLOSE) {
    //webclient_closed();
    //uip_abort();
    // return;
    //}
    if (uip_aborted()) {
        //webclient_aborted();
    }
    if (uip_timedout()) {
        //webclient_timedout();
    }
    if (uip_acked()) {
        //s.timer = 0;
        //acked();
    }
    if (uip_newdata()) {
        cloud_client_data_handler((char *)uip_appdata, uip_datalen());
        return;
    }
    if (uip_rexmit()) {
        //senddata();
    }
    if (uip_poll()) {
        printf("keepAliveTimeCnt:%d, gpioUpdateTimeCnt:%d, TcpConnectTimeCnt:%d, FOTAUpdateTimeCnt:%d\n",
               keepAliveTimeCnt, gpioUpdateTimeCnt, TcpConnectTimeCnt, FOTAUpdateTimeCnt);
        if (mt76xx_cloudClientState == CLOUD_CLIENT_COMMAND) {
            keepAliveTimeCnt ++;
            gpioUpdateTimeCnt ++;
            if (keepAliveTimeCnt%keepAlivePeriod == 0) {
                cloud_client_send_keepalive();
                return;
            } else {
                sendKeepAlive_flag = 0;
            }
            if (gpioUpdateTimeCnt%gpioUpdatePeriod == 0) {
                cloud_client_gpio_autoupdate();
                gpioUpdateTimeCnt = 0;
                return;
            } else {
                gpioAutoUpdate_flag = 0;
            }
            if (FOTAUpdateTimeCnt < 1) {
                FOTAUpdateTimeCnt++;
                cloud_client_fota_autoupdate();
                return;
            } else {
                FOTAAutoUpdate_flag = 0;
            }
            if (!sendKeepAlive_flag && !gpioAutoUpdate_flag && !FOTAAutoUpdate_flag)
                cloud_client_uart_autoupdate();
        }
    }
    if (uip_closed()) {
    }
}
#endif
#endif

