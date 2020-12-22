#ifndef SETTINGS_H
#define SETTINGS_H

#define DHT_TYPE                    <Dht type>       //DHT11 | DHT22
#define DHT_PIN                     <Digital port>   //example: D5
#define WATER_PUMP_RELAY_PIN        <Digital port>   //example: D6

#define MOISTURE_SENSOR_DRY         <Dry Measurement of moisture sensor>
#define MOISTURE_SENSOR_WET         <Wet Measurement of moisture sensor>
#define MOISTURE_SENSOR_PIN         <Analog port>   //example: A0    

#define CONFIG_WIFI_SSID            <SSID Name>
#define CONFIG_WIFI_PASSWORD        <SSID Password>
#define CONFIG_WIFI_STATIC_IP       true
IPAddress ip(192, 168, 1, 23);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

#define WEB_PUBLISHED               true
#define WEB_PORT                    80
#define WEBSOCKET_ENABLED           false
#define WEBSOCKET_PORT              81
#define WEB_DATA_REFRESH            2000            //DATA REFRESH TIME IN MILLISECONDS

#endif SETTINGS_H