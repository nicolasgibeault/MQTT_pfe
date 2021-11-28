#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "BELL984";
const char* password = "9F3CDAF9";
const char* brokerUser = "nicolasgibeault@gmail.com";
const char* brokerPassword = "f10b5df0";
const char* broker = "70.52.17.228";
const char* outTopic1 = "/out";
const char* outTopic2 = "/out2";
const char* outTopic3 = "/out3";
const char* outTopic4 = "/out4";
const char* inTopic = "/in";
char message[50];
int preset = 0;
std::string s;
#define LED 2
DynamicJsonDocument doc(1024);
WiFiClient espClient;
PubSubClient client(espClient);
long triggerMsg, timeMsg;
int state = 0;
int voltage = 0;
int current = 0;
int errorcom = 0;


void sendmsgmqtt(){

    
    sniprintf(message, 75, "state: %ld", state);
    Serial.print("\n message envoye: ");
    Serial.println(message);
    client.publish(outTopic1, message);
    timeMsg = millis();
    if (state==0)
    {
      state = 1;
    }
    else
    {
      state =0;
    }
    
    

    sniprintf(message, 75, "voltage: %ld", voltage);
    Serial.print("\n message envoye: ");
    Serial.println(message);
    client.publish(outTopic2, message);
    timeMsg = millis();

    sniprintf(message, 75, "state: %ld", current);
    Serial.print("\n message envoye: ");
    Serial.println(message);
    client.publish(outTopic3, message);
    timeMsg = millis();

    sniprintf(message, 75, "state: %ld", errorcom);
    Serial.print("\n message envoye: ");
    Serial.println(message);
    client.publish(outTopic4, message);
    timeMsg = millis();
    
}
void setupWifi(){
  delay(100);
  Serial.print("\nConnection a ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(100);
   Serial.print("-");
  }

  Serial.print("\nConnection etabli pour ");
  Serial.println(ssid);
 
}

void reconnect(){
  while (!client.connected()){
    Serial.print("\n connection au broker ");
    Serial.println(broker);
    if(client.connect("707", brokerUser, brokerPassword)){
      Serial.print("\nconnection etablie sur ");
      Serial.println(broker);
      client.subscribe(inTopic);
    }
    else{
      Serial.println("\n retentative ");
      delay(5000);

    }
  }
  {
    /* code */
  }
  
}
void decodePayload (){
    deserializeJson(doc, s);
    serializeJsonPretty(doc, Serial);
  JsonObject obj = doc.as<JsonObject>();
  long age = obj[String("age")];
  Serial.println();
  Serial.println(age);

}
void callback(char* topic, byte* payload, unsigned int length){
    Serial.print("\n message recu; ");
    Serial.println(topic);
    for(int i=0; i<length; i++){
      Serial.print((char) payload[i]);
      s +=((char) payload[i]);
    }
    Serial.println();
    decodePayload ();
    
    
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  setupWifi();
  client.setServer(broker, 1883);
  client.setCallback(callback);
  


}

void loop() {
  // put your main code here, to run repeatedly:
  

  if (!client.connected()){
    reconnect();
  }
  client.loop();

  // sends each 8 seconds da ta to database
  delay(8000);
  sendmsgmqtt();
  
}

