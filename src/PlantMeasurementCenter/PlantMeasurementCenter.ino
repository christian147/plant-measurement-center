#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>
#include <FS.h>
#include <DHT.h>
#include <ArduinoJson.h>
#include "WifiConfig.h"
#include "Settings.h"
#include "DFRobot_VEML7700.h"

uint8_t MoistureSensorPin = MOISTURE_SENSOR_PIN;
uint8_t DHTPin = DHT_PIN;
uint8_t RelayPin = WATER_PUMP_RELAY_PIN;
int relayState = LOW;
float Temperature;
float Humidity;
int SoilHumidity;
float Illumination;
unsigned long previousMillis;

DHT dht(DHTPin, DHT_TYPE);
AsyncWebServer server(WEB_PORT);
WebSocketsServer webSocketServer = WebSocketsServer(WEBSOCKET_PORT);
WifiConfig wifiConfig;

DFRobot_VEML7700 als;

void setup()
{
  Serial.begin(115200);
  pinMode(RelayPin, OUTPUT);
  digitalWrite(RelayPin, HIGH);
  pinMode(DHTPin, INPUT);
  digitalWrite(DHTPin, HIGH);
  dht.begin();

  als.begin();
  
  wifiConfig.Connect();

  if (WEB_PUBLISHED) {
    if (!SPIFFS.begin()) {
      Serial.println("Error mounting the file system");
    }
    server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");
    server.serveStatic("/styles", SPIFFS, "/styles");

    if (WEBSOCKET_ENABLED) {
      server.on("/scripts/index.js", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(SPIFFS, "/scripts/websocket.js", "text/javascript");
      });
      webSocketServer.onEvent(webSocketEvent);
      webSocketServer.begin();
      Serial.println("WebSocketServer started");
    } else {
      server.on("/scripts/index.js", HTTP_GET, [](AsyncWebServerRequest * request) {
        request->send(SPIFFS, "/scripts/api.js", "text/javascript");
      });
      server.on("/api/measurements", HTTP_GET, [](AsyncWebServerRequest * request) {
        String measurements;
        StaticJsonDocument<120> jsonDoc;
        jsonDoc["temperature"] = Temperature;
        jsonDoc["humidity"] = Humidity;
        jsonDoc["soilHumidity"] = SoilHumidity;
        jsonDoc["illumination"] = round(Illumination);
        jsonDoc["refreshIn"] = WEB_DATA_REFRESH;
        jsonDoc["isWatering"] = digitalRead(RelayPin) == LOW;
        serializeJson(jsonDoc, measurements);
        webSocketServer.broadcastTXT(measurements);
        request->send(200, "application/json; charset=utf-8", measurements);
      });
      server.on("/api/water", HTTP_POST, [](AsyncWebServerRequest * request) {
        water();
        String response;
        StaticJsonDocument<5> jsonDoc;
        jsonDoc = digitalRead(RelayPin) == LOW;
        serializeJson(jsonDoc, response);
        request->send(200, "application/json; charset=utf-8", response);
      });

      Serial.println("HTTPServer started");
    }

    server.begin();
  }
  previousMillis = millis() + WEB_DATA_REFRESH;
}

void loop()
{
  if (millis() - previousMillis >= WEB_DATA_REFRESH) {
    Temperature = dht.readTemperature();
    Humidity = dht.readHumidity();

    int sensor = analogRead(MoistureSensorPin);
    SoilHumidity = map(sensor, MOISTURE_SENSOR_WET, MOISTURE_SENSOR_DRY, 100, 0);

    als.getALSLux(Illumination);

    if (WEB_PUBLISHED && WEBSOCKET_ENABLED) {
      String measurements;
      StaticJsonDocument<120> jsonDoc;
      jsonDoc["command"] = "measurements";
      jsonDoc["temperature"] = Temperature;
      jsonDoc["humidity"] = Humidity;
      jsonDoc["soilHumidity"] = SoilHumidity;
      jsonDoc["illumination"] = round(Illumination);
      jsonDoc["refreshIn"] = WEB_DATA_REFRESH;
      jsonDoc["isWatering"] = digitalRead(RelayPin) == LOW;
      serializeJson(jsonDoc, measurements);
      webSocketServer.broadcastTXT(measurements);
    }

    previousMillis = millis();
  }

  if (WEB_PUBLISHED && WEBSOCKET_ENABLED) webSocketServer.loop();
}

void water() {
  digitalWrite(RelayPin, !digitalRead(RelayPin));
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
      jsonDoc["isWatering"] = digitalRead(RelayPin) == LOW;
      serializeJson(jsonDoc, response);
      webSocketServer.broadcastTXT(response);
    }
  }
}
