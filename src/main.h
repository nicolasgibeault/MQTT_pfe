#include <Arduino.h>
#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <USB_TCPM.h>
#include <FUSB302.h>

void Prompti2c();
void sendmsgmqtt();
void wifipromptcredentials();
void setupWifi();
void reconnect();
void callback(char* topic, byte* payload, unsigned int length);