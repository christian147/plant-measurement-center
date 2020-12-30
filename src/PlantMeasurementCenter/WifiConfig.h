#ifndef WifiConfig_h
#define WifiConfig_h

#include <ESP8266WiFi.h>

class WifiConfig
{
  public:
    void configStaticIp(IPAddress ip, IPAddress gateway, IPAddress subnet) {
      WiFi.config(ip, gateway, subnet);
    };
    void connect(const char* ssid, const char* pass) {
      init(ssid, pass);
    };
  private:
    static void init(const char* ssid, const char* pass) {
      Serial.print("Connecting to ");
      Serial.print(ssid);

      WiFi.mode(WIFI_STA);
      WiFi.begin(ssid, pass);
      while (WiFi.status() != WL_CONNECTED)
      { delay(1000);
        Serial.print(".");
      }
      WiFi.setAutoReconnect(true);
      Serial.println("");
      Serial.print("WiFi connected. My Ip is ");
      Serial.println(WiFi.localIP());
    };
};

#endif
