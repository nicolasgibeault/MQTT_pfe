#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

const char* ssid = "BELL984";
const char* password = "9F3CDAF9";
const char* brokerUser = "nicolasgibeault@gmail.com";
const char* brokerPassword = "f10b5df0";
const char* broker = "mqtt.dioty.co";
const char* outTopic = "/nicolasgibeault@gmail.com/out";
const char* inTopic = "/nicolasgibeault@gmail.com/in";
char message[50];
int preset = 0;
std::string s;
#define LED 2
DynamicJsonDocument doc(1024);
WiFiClient espClient;
PubSubClient client(espClient);
long triggerMsg, timeMsg;
int count = 0;

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
    if(client.connect("personnel", brokerUser, brokerPassword)){
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


/*void stringToChar(){
    int n = s.length();
     // declaring character array
    char char_array[n + 1];
     // copying the contents of the
    // string to char array
    strcpy(char_array, s.c_str());
     for (int i = 0; i < n; i++){
        cout << char_array[i];
     }
}*/
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


   /* sniprintf(message, 75, "count: %ld", count);
    Serial.print("\n message envoye: ");
    Serial.println(message);
    client.publish(outTopic, message);
    timeMsg = millis();*/
  
}