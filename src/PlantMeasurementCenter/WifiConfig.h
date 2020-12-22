#ifndef WifiConfig_h
#define WifiConfig_h

#include "Arduino.h"
#include <ESP8266WiFi.h>
#include "Settings.h"

class WifiConfig
{
  public:
    void Connect() {
      Serial.print("Connecting to ");
      Serial.print(CONFIG_WIFI_SSID);

      if (CONFIG_WIFI_STATIC_IP) {
        WiFi.config(ip, gateway, subnet);
      }

      WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASSWORD);
      while (WiFi.status() != WL_CONNECTED)
      { delay(1000);
        Serial.print(".");
      }
      Serial.println("");
      Serial.print("WiFi connected. My Ip is ");
      Serial.println(WiFi.localIP());
    };
};

#endif
