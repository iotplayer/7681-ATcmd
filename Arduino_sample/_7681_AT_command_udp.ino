#include <Arduino.h>
#include <IPAddress.h>

static int base64_encode_len(int len)
{
    return ((len+2)/3)*4;
}

static uint8_t base64_enc_map(uint8_t n)
{
    if(n < 26)
        return 'A'+n;
    if(n < 52)
        return 'a'+(n-26);
    if(n < 62)
        return '0'+(n-52);
    return n == 62 ? '+' : '/';
}

static void base64_encode(const uint8_t* src, int len, uint8_t* dest)
{
    uint32_t w;
    uint8_t t;
    while(len >= 3)
    {
        w = ((uint32_t)src[0])<<16 | ((uint32_t)src[1])<<8 | ((uint32_t)src[2]);

        dest[0] = base64_enc_map((w>>18)&0x3F);
        dest[1] = base64_enc_map((w>>12)&0x3F);
        dest[2] = base64_enc_map((w>>6)&0x3F);
        dest[3] = base64_enc_map((w)&0x3F);

        len-=3;
        src+=3;
        dest+=4;
    }
    if(!len)
        return;

    if(len == 2)
    {
        w = ((uint32_t)src[0])<<8 | ((uint32_t)src[1]);

        dest[0] = base64_enc_map((w>>10)&0x3F);
        dest[1] = base64_enc_map((w>>4)&0x3F);
        dest[2] = base64_enc_map((w&0x0F)<<2);
        dest[3] = '=';
    }
    else
    {
        w = src[0];

        dest[0] = base64_enc_map((w>>2)&0x3F);
        dest[1] = base64_enc_map((w&0x03)<<4);
        dest[2] = '=';
        dest[3] = '=';
    }
}

static int base64_decode_len(int len)
{
    return ((len+3)/4)*3;
}

static uint32_t base64_dec_map(uint8_t n)
{
    if(n >= 'A' && n <= 'Z')
        return n - 'A';
    if(n >= 'a' && n <= 'z')
        return n - 'a' + 26;
    if(n >= '0' && n <= '9')
        return n - '0' + 52;
    return n == '+' ? 62 : 63;
}

static int base64_decode(const uint8_t* src, int len, uint8_t* dest)
{
    uint32_t w;
    uint8_t t;
    int result = 0;

    // remove trailing =
    while(src[len-1] == '=')
        len--;

    while(len >= 4)
    {
        w = (base64_dec_map(src[0]) << 18) |
            (base64_dec_map(src[1]) << 12) |
            (base64_dec_map(src[2]) << 6) |
            base64_dec_map(src[3]);

        dest[0] = (w>>16)&0xFF;
        dest[1] = (w>>8)&0xFF;
        dest[2] = (w)&0xFF;

        len-=4;
        src+=4;
        dest+=3;
        result+=3;
    }
    if(!len)
        return result;

    if(len == 3)
    {
        w = (base64_dec_map(src[0]) << 18) |
            (base64_dec_map(src[1]) << 12) |
            (base64_dec_map(src[2]) << 6) |
            0;

        dest[0] = (w>>16)&0xFF;
        dest[1] = (w>>8)&0xFF;
        result+=2;
    }
    else if(len == 2)
    {
        w = (base64_dec_map(src[0]) << 18) |
            (base64_dec_map(src[1]) << 12) |
            0;

        dest[0] = (w>>16)&0xFF;
        result+=1;
    }
    else
    {
        // should not happen.
    }
    return result;
}

class LC7681Wifi
{
public:
  typedef void (*LC7681WifiCallback)(int event, const uint8_t* data, int dataLen);

  enum {
    EVENT_NONE,
    EVENT_DATA_RECEIVED,
    EVENT_SOCKET_DISCONNECT,
    EVENT_MAX
  };
  
public:
  LC7681Wifi(Stream *s, Stream* l = NULL):
    m_lport(0)
  {
    m_stream = s;
    m_log = l;
    m_bufferPos = 0;
  }

  void begin()
  {
    
  }
  
  bool connectAP(const char* ssid, const char* key)
  {
    String str = "AT+WCAP=";
    str += ssid;
    str += ",";
    str += key;
    str += ",0";
    m_stream->println(str);
  
    str = String("+WCAP:") + ssid;
  
    str = _wait_for(str.c_str(), 20);
    if(str.length() != 0)
    {
      return true;
    }
    return false;
  }
  
  IPAddress s2ip(const char* str)
  {
    uint32_t ip[4];
    sscanf(str, "%d.%d.%d.%d", ip, ip+1, ip+2, ip+3);
    return IPAddress(ip[0], ip[1], ip[2], ip[3]);
  }
  
  IPAddress IP()
  {
    String result, ips;
    m_stream->println("AT+WQIP?");
    result = _wait_for("+WQIP:", 5);
    if(result.length() == 0)
      return IPAddress();
    ips = result.substring(6, result.indexOf(',', 6));
    return s2ip(ips.c_str());
  }
  
  IPAddress nslookup(const char* server)
  {
    String result, ips;
    
    result = "AT+WDNL=";
    result += server;
    m_stream->println(result);
    result = _wait_for("+WDNL:", 5);
    if(result.length() == 0)
      return IPAddress();
    
    ips = result.substring(result.lastIndexOf(',')+1);
    return s2ip(ips.c_str());
  }

  bool connect(IPAddress ip, int port, bool udp = false)
  {
    char buf[16];
    sprintf(buf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
    return connect(buf, port, udp);
  }
  
  bool connect(const char* ip, int port, bool udp = false)
  {
    String str = "AT+WSO=";
    str += ip;
    str += ",";
    str += port;
    str += ",";
    str += udp ? "1" : "0";
    m_stream->println(str);
    
    str = _wait_for("+WSO:");
    if(str.length() == 0)
      return false;
    
    m_lport = str.substring(5).toInt();
    if(udp)
      return true;

    str = "+WSS:";
    str += m_lport;
    str = _wait_for(str.c_str());
    if(str.length() == 0)
    {
      m_lport = 0;
      return false;
    }
    
    if(str.substring(str.lastIndexOf(',')+1).toInt() == 0)
    {
      m_lport = 0;
      return false;
    }

    return true;
  }

  bool print(const char* data, int dataLen)
  {
    char buf[100];
    const char *src = data;
    char *dest = buf;
    int remain = dataLen;

    while(remain > 0)
    {
      int part = remain > 48 ? 48 : remain;
      
      sprintf(buf, "AT+WSW=%d,", m_lport);
      dest = buf + strlen(buf);
      base64_encode((uint8_t*)src, part, (uint8_t*)dest);
      dest+= base64_encode_len(part);
      dest[0] = '\r';
      dest[1] = '\n';
      dest[2] = 0;
      
      m_stream->print(buf);
      src+=part;
      remain-=part;

      String str = _wait_for("+WSDS:");
      if(str.length() == 0)
        return false;
    }    
    return true;
  }
  
  bool print(const char* data = NULL)
  {
    if(!data)
      return false;
    
    return print(data, strlen(data));
  }

  bool println(const char* data = NULL)
  {
    String str;
    if(data)
      str = data;
    str += "\r\n";
    
    return print(str.c_str());
  }
  
  void process(LC7681WifiCallback cb)
  {
    int c;
    while(m_stream->available())
    {
      c = m_stream->read();
      m_buffer[m_bufferPos] = (char)c;
      m_bufferPos++;
      if(c == '\n')
      {
        m_buffer[m_bufferPos] = 0;
        m_bufferPos = 0;
        
        if(m_log)
        {
          m_log->print("[log]");
          m_log->println(m_buffer);
        }

        if(!strncmp(m_buffer, "+WSDR:", 6))
        {
          String s = m_buffer+6;
          s.trim();
          int t1 = s.indexOf(',');
          int t2 = s.indexOf(',', t1+1);
          int port = s.substring(0, t1).toInt();
          
          if(port == m_lport)
          {
            int len = s.substring(t1+1, t2).toInt();
            String data = s.substring(t2+1);
            if(m_log && data.length() != len)
            {
              m_log->print("[Warning] length not matching:");
              m_log->print(len);
              m_log->print(data.length());
              m_log->println(data);
            }

            // use m_buffer as temp buffer
            t1 = base64_decode((const uint8_t*)data.c_str(), len, (uint8_t*)m_buffer);
            m_buffer[t1] = 0;
            cb(EVENT_DATA_RECEIVED, (uint8_t*)m_buffer, t1);
          }
        }
        else if(!strncmp(m_buffer, "+WSS:", 5))
        {
          String s = m_buffer+5;
          s.trim();
          int t1 = s.indexOf(',');
          int port = s.substring(0, t1).toInt();
          int state = s.substring(t1+1).toInt();
          if(port == m_lport)
          {
            if(state == 0)
              cb(EVENT_SOCKET_DISCONNECT, NULL, 0);
            m_lport = 0;
          }
        }
        
      }
    }
  }
  
private:
  String _wait_for(const char* pattern, int timeout = 300)
  {
    unsigned long _timeout = millis() + timeout*1000;
    char buf[128];
    int i, c;
    
    if(m_log)
      m_log->println("[wait_for]");
    
    i = 0;
    while(millis() <= _timeout)
    {
      while(m_stream->available())
      {
        c = m_stream->read();
        if(m_log)
          m_log->write(c);

        buf[i] = (char)c;
        i++;
        if(c == '\n')
        {
          buf[i] = 0;
          
          if(0 == strncmp(buf, pattern, strlen(pattern)))
          {
            String result(buf);
            result.trim();
            return result;
          }
          
          i = 0;
        }
      }
    }
    return String(""); // timeout
  }

  
  
private:
  Stream *m_stream;
  Stream *m_log;
  
  int m_lport;
  int m_bufferPos;
  char m_buffer[128];
};


/*
========================================================================================
*/

#ifdef __LINKIT_ONE__
LC7681Wifi wifi(&Serial1);
#else
LC7681Wifi wifi(&Serial);
#endif

const char ssid[] = "MyAP";
const char key[] = "MyKey";
const char server[] = "time.stdtime.gov.tw";
const int port = 123;

unsigned long prev_time = 0;

void setup() {
  Serial.begin(115200);

#ifdef __LINKIT_ONE__
  Serial1.begin(115200);
  while(!Serial)
    delay(100);
#endif

  // reset module
  Serial.print("[log] Resetting Module...");
  pinMode(9, OUTPUT);
  digitalWrite(9, LOW);
  delay(100);
  digitalWrite(9, HIGH);
  delay(7000);
  Serial.println("done");
  
  wifi.begin();
  
  // attempt to connect to AP
  while(true)
  {
    Serial.println("[log] Trying to connect AP...");
    if(!wifi.connectAP(ssid, key))
    {
      Serial.println("[log] Fail to connect, wait 5 secs to retry...");
      delay(5000);
      continue;
    }

    Serial.println("[log] Connected to AP!");
    break;
  }

  // print status
  delay(1000);
  Serial.print("[log] IP Address: ");
  Serial.println(wifi.IP());

  // lookup server ip
  IPAddress serverIp = wifi.nslookup(server);
  Serial.print("[log] IP Address of ");
  Serial.print(server);
  Serial.print(" is ");
  Serial.println(serverIp);
  
  // connect to server
  while(true)
  {
    Serial.println("[log] Trying to start connection to server...");
    if (!wifi.connect(serverIp, port, true))
    {
      Serial.println("[log] Fail to connected to server, wait 5 secs to retry...");
      delay(5000);
      continue;
    }
    
    Serial.println("[log] Connected to server!");
    break;
  }
  
  
}

void callback(int event, const uint8_t* data, int dataLen)
{
  switch(event)
  {
  case LC7681Wifi::EVENT_DATA_RECEIVED:
    if(dataLen == 48)
    {
      // Unix time starts on Jan 1 1970. In seconds, that's 2208988800:
      const unsigned long seventyYears = 2208988800UL;
      
      uint32_t epoch = ((uint32_t)data[40]<<24) | ((uint32_t)data[41]<<16) | ((uint32_t)data[42]<<8) | ((uint32_t)data[43]);
      //Serial.println(epoch);
      
      epoch = epoch- seventyYears;
      // print the hour, minute and second:
      Serial.print("NTP packet recevied, current UTC time is ");
      Serial.print((epoch  % 86400L) / 3600); // print the hour (86400 equals secs per day)
      Serial.print(':');
      if ( ((epoch % 3600) / 60) < 10 ) {
        // In the first 10 minutes of each hour, we'll want a leading '0'
        Serial.print('0');
      }
      Serial.print((epoch  % 3600) / 60); // print the minute (3600 equals secs per minute)
      Serial.print(':');
      if ( (epoch % 60) < 10 ) {
        // In the first 10 seconds of each minute, we'll want a leading '0'
        Serial.print('0');
      }
      Serial.println(epoch % 60); // print the second
    }
    else
    {
      Serial.print("Warning: received ");
      Serial.print(dataLen);
      Serial.print(" bytes data, should be 48 bytes!");
    }
    break;
  case LC7681Wifi::EVENT_SOCKET_DISCONNECT:
    Serial.println("[log] Connection to server is closed!");
    break;
  }
}

void loop() 
{
  wifi.process(callback);

  if(millis() > prev_time + 1000)
  {
    prev_time = millis();

    // Make a NTP request every seconds:
    //Serial.println("[log] Send NTP request");
    char buf[48];
    memset(buf, 0, 48);
    buf[0] = 0x1b;
    wifi.print(buf, 48);
  }
}



