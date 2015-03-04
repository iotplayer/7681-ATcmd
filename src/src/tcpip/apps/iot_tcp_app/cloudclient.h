#include "types.h"

enum CLIENT_STATE {
    CLOUD_CLIENT_INIT = 0,
    CLOUD_CLIENT_ACTIVE,
    CLOUD_CLIENT_CONNECT,
    CLOUD_CLIENT_COMMAND,
    CLOUD_CLIENT_FOTAUPDATE
};

typedef struct GNU_PACKED t_Header {
    uint32 Magic;                    //protocol magic number
    uint8 DeviceID[8];      //receive mac address
    uint8 DeviceIDCopy[8];     //sender mac address
    uint16 CommandID;                //reserved
    uint16 Reserve:4;                   //reserved
    uint16 Type:4;                   //command type. 0: management command 1: function command
    uint16 SubType:8;                  //subtype
} CloudClientCtrlHeader;

typedef struct GNU_PACKED t_FOTAInfo {
    uint8 BootMode;
    uint8 MajorVersion;
    uint8 MinorVersion;
    uint32 ImageLen;
    uint8 ImageMD5[16];
    uint8 ServerIP[4];
    uint8 FilePath[64];
    uint16 PageHead;
    uint16 PageEnd;
    uint16 RemainNum;
    uint32 ImgTotLen;
} FOTAInfo;

typedef struct GNU_PACKED t_PWM_Descriptor
{
    uint8 PWMID;
    uint16 PWM_Period;
    uint16 PWM_Value;
}PWM_Descriptor;

typedef enum t_CloudManagementCommand
{
    QUERY_CAP_REQUEST = 1,
    QUERY_CAP_RESPONSE,
    CONTROL_OFFLINE_REQUEST,
    CONTROL_OFFLINE_RESPONSE,
    CONTROL_PW_SET_REQUEST,
    CONTROL_PW_SET_RESPONSE,
    CONTROL_PW_SET_CONFIRM,
    KEEP_ALIVE_SEND,
    KEEP_ALIVE_RESPONSE,
    MANAGEMENT_COM_MAX
}CloudManagementCommand;

typedef enum t_CloudFunctionCommand
{
    SET_GPIO_REQUEST = 1,
    SET_GPIO_RESPONSE,
    GET_GPIO_REQUEST,
    GET_GPIO_RESPONSE,
    AUTO_UPDATE,
    SET_UART_REQUEST,
    SET_UART_RESPONSE,
    SET_PWM_REQUEST,
    SET_PWM_RESPONSE,
    GET_PWM_REQUEST,
    GET_PWM_RESPONSE,
    FOTA_START_UPDATE,
    FOTA_UPDATE_RESPONSE,
    GET_UART_REQUEST,
    GET_UART_RESPONSE,
    FUNCTION_COM_MAX
}CloudFunctionCommand;

typedef enum t_CloudDataType
{
    CLIENT_CAP = 1,
    UART_INFO,
    GPIO_INFO,
    PWM_INFO,
    CONTROL_PW,
    FOTA_INFO
}CloudDataType;


uint32 cloud_activation_process(uint8 *input, uint32 length) XIP_ATTRIBUTE(".xipsec1");
uint32 cloud_para_check_connect(void) XIP_ATTRIBUTE(".xipsec1");
void cloud_tcp_conn_check(void) XIP_ATTRIBUTE(".xipsec1");
uint32 cloud_http_callback(char *data, uint16 len);

