# 7681-ATcmd
This project is an extension for LinkIt Connect 7681.

LinkIt Connect 7681 is an product of Mediatek Labs (http://labs.mediatek.com/site/global/developer_tools/mediatek_7681/whatis_7681/index.gsp)

It is basically a Wi-Fi module. But the SDK does not include enough AT command. 
This project try to add AT command so that it can be used by Arduino or other micro controller

## AT command table

|AT Command|Description|Syntax|Result|
|----------|-----------|------|------|
|AT+WSWM|SWitch Mode between Station/AP mode|AT+WSWM?<br>AT+WSWM=1 (switch to STA)<br>AT+WSWM=2 (switch to AP)|+WSWM:1 or 2<br>OK<br>OK|
|AT+WMAC|query MAC address of 7681|AT+WMAC?|+WMAC:XX-XX-XX-XX-XX-XX|
|AT+WCAP|(Station only) Connect to an AP|AT+WCAP=\<ssid>,\<key>,\<auth>|OK ～<br>+WCAP=\<ssid>,\<key>,\<auth>,\<mac>|
||(Station only) disconnect from AP|AT+WCAP=|+WCAP=|
|AT+WQIP|(Station only) Query IP|AT+WQIP?|+WQIP=\<ip>,\<gateway>,\<dns>|
|AT+WAPC|(AP only) AP Configuration|AT+WAPC?<br>AT+WAPC=\<ssid>,\<key>,\<auth>|+WAPC=\<ssid>,\<key>,\<auth><br>OK|
|AT+WDNL|Domain Name Lookup|AT+WDNL=\<server name>|OK ～<br>+WDNL:<server name>,<ip>|
|AT+WSO|Socket Open, connect to remote server|AT+WSO=\<serverip>,\<port>,\<type>|+WFO:\<lport>|
|AT+WSL|Socket Listen, listen port|AT+WSL=\<port>,\<type>|OK|
|AT+WSW|Socket Write, send data|AT+WSW=\<lport>,\<data>|OK, ERROR, BUSY ～<br>+WSDS:\<lport>|
|AT+GPI0~4|Control GPIO 0~4|AT+GPI0?<br>AT+GPI0=0 or 1|+GPI0:0 or 1<br>OK|
|AT+GPW0~4|Control PWM 0~4|AT+GPW0?<br>AT+GPW0=\<dutycycle>,\<resolution>|+GPW0:\<dutycycle>,\<resolution><br>OK|

## Unsolicited AT notify table

|Command|Description|Syntax|
|-------|-----------|------|
|+WCAP|Successfully connect to an AP|+WCAP=\<ssid>,\<key>,\<auth>,\<mac>|
|+WSDR|Socket Data Received|+WSDR:\<lport>,\<len>,\<data>|
|+WSS|Socket State change|+WSS:\<lport>,\<state>|
