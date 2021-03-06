#include <ESP8266WiFi.h>
#include <AmazonIOTClient.h>
#include "ESP8266AWSImplementations.h"

Esp8266HttpClient httpClient;
Esp8266DateTimeProvider dateTimeProvider;

AmazonIOTClient iotClient;
ActionError actionError;

char *ssid="BELL984";
char *password="9F3CDAF9";

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to WAP
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  iotClient.setAWSRegion("eu-west-1");
  iotClient.setAWSEndpoint("a38ogfyxr6lvw-ats.iot.ca-central-1.amazonaws.com");
  iotClient.setAWSDomain("foobar.iot.eu-west-1.amazonaws.com");
  iotClient.setAWSPath("/ESP32_thing/example-1/shadow");
  iotClient.setAWSKeyID("ID");
  iotClient.setAWSSecretKey("SECRET");
  iotClient.setHttpClient(&httpClient);
  iotClient.setDateTimeProvider(&dateTimeProvider);
}

void loop(){
  char shadow[100];
  strcpy(shadow, "{\"state\":{\"reported\": {\"foobar\": \"bar\"}}}");
  char* result = iotClient.update_shadow(shadow, actionError);

  Serial.print(result);

  delay(60000);
}