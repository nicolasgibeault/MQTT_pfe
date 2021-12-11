#include <main.h>

//WIFI credentials to enter by user
static char ssid[512] = ""; //= "BELL984";
static char password[512] = ""; // = "9F3CDAF9";

/* Put your SSID & Password for first use of esp32 */
const char* ssidesp32 = "ESP32";  // Enter SSID here
const char* passwordesp32 = "12345678";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
// variables to soft SP server
AsyncWebServer server(80);

// REPLACE WITH YOUR NETWORK CREDENTIALS

const char* PARAM_INPUT_1 = "Wifi_ssid";
const char* PARAM_INPUT_2 = "password";


// HTML web page to handle 2 input fields (SSID, Password)
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html><head>
  <title>ESP Input Form</title>
  <style>
input {color:red;}
h1 {color:blue;}
h2 {color:blue;}
body {
  font-family: Arial, Helvetica, sans-serif;
  margin: 0;
}
h1 {
  font-size: 40px;
}
</style>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  </head><body>
  <h1>HUMANITY TECH Wifi Setup</h1><br>
  <h2>Insert WIFI credentials</h2>
  <form action="/get">
    Wifi_ssid : <input type="text" name="Wifi_ssid"><br><br>
    Password :<input type="text" name="password"><br><br>
    <input type="submit" value="Submit">
  </form>
</body></html)rawliteral";

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "Not found");
}
//MQTT communication declaration
const char* brokerUser = "BELL984";
const char* brokerPassword = "9F3CDAF9";
const char* broker = "70.52.17.228";
const char* outTopic1 = "/state";
const char* outTopic2 = "/voltage";
const char* outTopic3 = "/current";
const char* outTopic4 = "/charge";
const char* outTopic5 = "/error";
const char* inTopic = "/in";
char message[50];
int preset = 0;

// API declaration
std::string s;
#define LED 2
DynamicJsonDocument doc(1024);
WiFiClient espClient;
uint16_t portnum = 707;
IPAddress adrr(70,52,17,228);
PubSubClient client(adrr, portnum, espClient);
long triggerMsg, timeMsg;

//declaration variables a communiquer
int state = 0;
int voltage = 0;
int current = 0;
int charge = 0;
int errorcom = 0;

//i2c declaration

int count = 0;
byte ADDRESS_SLAVE = 0X22; 
byte READ_LENGTH = 8;

int Prompti2c(){

int errorcode =0 ;
  Wire.beginTransmission(ADDRESS_SLAVE);  
  Wire.write(FUSB302_D_Register_DeviceID);  // set register for read
  Wire.write(FUSB302_D_Register_Switches0);  // set register for read
  Wire.write(FUSB302_D_Register_Switches1);  // set register for read
  Wire.write(FUSB302_D_Register_Measure);  // set register for read
  Wire.write(FUSB302_D_Register_Status0a);  // set register for read
  Wire.write(FUSB302_D_Register_Status1a);  // set register for read
  Wire.write(FUSB302_D_Register_Status0);  // set register for read
  Wire.write(FUSB302_D_Register_Status1);  // set register for read
  Wire.write(FUSB302_D_Register_Control0);  // set register for read
  Wire.endTransmission();

   Wire.requestFrom(ADDRESS_SLAVE,READ_LENGTH); 
   byte buff[READ_LENGTH];    
   Wire.readBytes(buff, READ_LENGTH);

/// if the device id is zero, then the fetch did not work and an error has occured 
 
   Serial.print("\n recieved from I2C: ");
  for (int i = 0; i < READ_LENGTH; i++) {
     Serial.println(buff[i], BIN);
     
     switch (i)
     {
      case 0:
        if (buff[i] == 0){
          errorcom=1;
        }
        else{
          errorcom=0;
        }
       break;
      case 3:
       voltage = 5;
       break;
      case 6:
       current = buff[i] % 3;
       break;
      case 7:
       charge =( charge + 1) % 100;
       break;
     default:
       break;
    buff[i] =0;
     }

   }
   Serial.println("FIN");
}

void sendmsgmqtt(int errori2c){

    
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
            sniprintf(message, 75, "current: %ld", current);
    Serial.print("\n message envoye: ");
    Serial.println(message);
    client.publish(outTopic3, message);
    timeMsg = millis();
            sniprintf(message, 75, "charge: %ld", charge);
    Serial.print("\n message envoye: ");
    Serial.println(message);
    client.publish(outTopic4, message);
    timeMsg = millis();
                sniprintf(message, 75, "error: %ld", errori2c);
    Serial.print("\n message envoye: ");
    Serial.println(message);
    client.publish(outTopic5, message);
    timeMsg = millis();

    
}

void wifipromptcredentials(){
  while((ssid != NULL) && (ssid[0] == '\0')){
    delay(100);
server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/get?input1=<inputMessage>
  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputwifi;
    String inputpass;
    String inputParam;
    String inputParam2;
    // GET input1 value on <ESP_IP>/get?input1=<inputMessage>
    if (request->hasParam(PARAM_INPUT_2)) {
      inputwifi = request->getParam(PARAM_INPUT_1)->value();
      inputpass = request->getParam(PARAM_INPUT_2)->value();
      inputParam2 = PARAM_INPUT_2;
    }
    else {
      inputwifi = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputwifi);
    Serial.println(inputpass);
    strcpy(ssid, inputwifi.c_str());
    strcpy(password, inputpass.c_str());

    Serial.println(ssid);
    Serial.println(password);
    request->send(200, "text/html", "HTTP GET request sent to your ESP on input field wifi with value: ("
                                     + inputwifi +
                                    ") and password with value: " + inputpass +
                                     "<br><a href=\"/\">Return to Home Page</a>");

  });
  server.onNotFound(notFound);
  server.begin();
  }
  
}

void setupWifi(){
     WiFi.mode(WIFI_AP);
    WiFi.softAP(ssidesp32, passwordesp32);
    WiFi.softAPConfig(local_ip, gateway, subnet);


 
     wifipromptcredentials();
  
    WiFi.mode(WIFI_STA);
Serial.println("HTTP server started and had input");
  /* connection au reseau WIFI avec les credential*/
  delay(100);

  Serial.print("\nConnection a  ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(100);
   Serial.print("-");
  }

  Serial.print("\n");
  Serial.println(ssid);

    Serial.println("Connected to wifi");
 
}

void reconnect(){
  while (!client.connected()){
    Serial.print("\n connection au broker ");
    Serial.println(broker);
    if(client.connect("dev.to")){
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

#define I2C_SDA 21
#define I2C_SCL 22

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(LED, OUTPUT);
  setupWifi();
  client.setServer(broker, 707);
  client.setCallback(callback);
  Wire.begin(I2C_SDA, I2C_SCL,100000);
     delay(1000);


}

  int errori2c;
void loop() {

  //go grab the info from charger
 errori2c= Prompti2c();
   delay(1000);
  if (!client.connected()){
    reconnect();
  }
  client.loop();

  // sends each 2 seconds da ta to database
  delay(2000);
  sendmsgmqtt(errori2c);

}

