#ifndef SETTINGS_H
#define SETTINGS_H

#include <DHT.h>
class Settings {
  public:
    const bool DebuggerMode = true;
    const uint8_t DhtType = <Dht type>                                      //DHT11 | DHT22
    const uint8_t DhtPin = <Digital port>                                   //example: D5 | 14
    const uint8_t RelayPin = <Digital port>                                 //example: D6 | 12
    const uint16_t MoistureSensorDry = <Dry Measurement of moisture sensor>
    const uint16_t MoistureSensorWet = <Wet Measurement of moisture sensor>
    const uint8_t MoistureSensorPin = <Analog port>                         //example: A0
    const String WifiSsdi = <SSID Name>
    const String WifiPassword = <SSID Password>
    const boolean WifiStaticIp = true;
    const IPAddress WifiIp = IPAddress(192, 168, 1, 23);
    const IPAddress WifiGateway = IPAddress(192, 168, 1, 1);
    const IPAddress WifiSubnet = IPAddress(255, 255, 255, 0);
    const boolean WebPublished = true;
    const uint16_t WebPort = 80;
    const uint16_t WebDataRefresh = 2000;            //DATA REFRESH TIME IN MILLISECONDS
    const boolean WebSocketEnabled = false;
    const uint16_t WebSocketPort = 81;
};
#endif SETTINGS_H
