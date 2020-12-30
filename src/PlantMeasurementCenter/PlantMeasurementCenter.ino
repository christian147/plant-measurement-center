#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <FS.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include "WifiConfig.h"
#include "Settings.h"
#include "DFRobot_VEML7700.h"

int relayState = LOW;
float Temperature;
float Humidity;
int SoilHumidity;
float Illumination;
unsigned long previousMillis;

Settings settings;
DFRobot_VEML7700 ambientLightSensor;
DHT dht(settings.DhtPin, settings.DhtType);
AsyncWebServer webServer(settings.WebPort);
WebSocketsServer webSocketServer = WebSocketsServer(settings.WebSocketPort);
WifiConfig wifiConfig;

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(settings.DebuggerMode);

  pinMode(settings.RelayPin, OUTPUT);
  digitalWrite(settings.RelayPin, HIGH);
  pinMode(settings.DhtPin, INPUT);
  digitalWrite(settings.DhtPin, HIGH);
  dht.begin();

  ambientLightSensor.begin();

  if (settings.WifiStaticIp) {
    wifiConfig.configStaticIp(settings.WifiIp, settings.WifiGateway, settings.WifiSubnet);
  }
  wifiConfig.connect(settings.WifiSsdi.c_str(), settings.WifiPassword.c_str());
  //  iotHubDevice.connect();

  if (settings.WebPublished) {
    if (!SPIFFS.begin()) {
      Serial.println("Error mounting the file system");
    }
    webServer.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
    webServer.serveStatic("/styles", SPIFFS, "/styles");

    if (settings.WebSocketEnabled) {
      webServer.on("/scripts/index.js", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(SPIFFS, "/scripts/websocket.js", "text/javascript");
      });
      webSocketServer.onEvent(webSocketEvent);
      webSocketServer.begin();
      Serial.println("WebSocketServer started");
    } else {
      webServer.on("/scripts/index.js", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(SPIFFS, "/scripts/api.js", "text/javascript");
      });
      webServer.on("/api/measurements", HTTP_GET, [](AsyncWebServerRequest * request) {
        String measurements;
        StaticJsonDocument<120> jsonDoc;
        jsonDoc["temperature"] = Temperature;
        jsonDoc["humidity"] = Humidity;
        jsonDoc["soilHumidity"] = SoilHumidity;
        jsonDoc["illumination"] = round(Illumination);
        jsonDoc["refreshIn"] = settings.WebDataRefresh;
        jsonDoc["isWatering"] = digitalRead(settings.RelayPin) == LOW;
        serializeJson(jsonDoc, measurements);
        webSocketServer.broadcastTXT(measurements);
        request->send(200, "application/json; charset=utf-8", measurements);
      });
      webServer.on("/api/water", HTTP_POST, [](AsyncWebServerRequest * request) {
        water();
        String response;
        StaticJsonDocument<5> jsonDoc;
        jsonDoc = digitalRead(settings.RelayPin) == LOW;
        serializeJson(jsonDoc, response);
        request->send(200, "application/json; charset=utf-8", response);
      });

      Serial.println("HTTPServer started");
    }

    webServer.begin();
  }
  previousMillis = millis() + settings.WebDataRefresh;
}

void loop()
{
  if (millis() - previousMillis >= settings.WebDataRefresh) {
    Temperature = dht.readTemperature();
    Humidity = dht.readHumidity();

    int sensor = analogRead(settings.MoistureSensorPin);
    SoilHumidity = map(sensor, settings.MoistureSensorWet, settings.MoistureSensorDry, 100, 0);

    ambientLightSensor.getALSLux(Illumination);

    String measurements;
    StaticJsonDocument<120> jsonDoc;
    jsonDoc["temperature"] = Temperature;
    jsonDoc["humidity"] = Humidity;
    jsonDoc["soilHumidity"] = SoilHumidity;
    jsonDoc["illumination"] = round(Illumination);
    serializeJson(jsonDoc, measurements);
    //    iotHubDevice.sendMessage(const_cast<char*>(measurements.c_str()));

    if (settings.WebPublished && settings.WebSocketEnabled) {
      jsonDoc["command"] = "measurements";
      jsonDoc["refreshIn"] = settings.WebDataRefresh;
      jsonDoc["isWatering"] = digitalRead(settings.RelayPin) == LOW;
      serializeJson(jsonDoc, measurements);
      webSocketServer.broadcastTXT(measurements);
    }

    previousMillis = millis();
  }

  if (settings.WebPublished && settings.WebSocketEnabled) webSocketServer.loop();
}

void water() {
  digitalWrite(settings.RelayPin, !digitalRead(settings.RelayPin));
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length)
{
  if (type ==  WStype_TEXT) {
    StaticJsonDocument<200> doc;
    DeserializationError error = deserializeJson(doc, payload);
    String command = doc["command"];
    if (command == "water") {
      water();
      String response;
      StaticJsonDocument<120> jsonDoc;
      jsonDoc["command"] = "water";
      jsonDoc["isWatering"] = digitalRead(settings.RelayPin) == LOW;
      serializeJson(jsonDoc, response);
      webSocketServer.broadcastTXT(response);
    }
  }
}
