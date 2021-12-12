#include <main.h>
//WIFI credentials to enter by user
static char ssid[512] = "BELL984";
static char password[512] = "9F3CDAF9";

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
const char* brokerUser = "";
const char* brokerPassword = "";
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

//declaration variables to talk with
int state = 0;
int voltage = 0;
int current = 0;
int charge = 0;
int errorcom ;

//i2c declaration
byte ADDRESS_SLAVE = 0X22; 
FUSB302 usbc;
void Prompti2c(){

// get chip id and if not the ID of the board, set a comm error
    int chip_id;
  usbc.get_chip_id(&chip_id);
  Serial.print("FUSB302 ID = 0x");
  Serial.println(chip_id, HEX);
  if (chip_id != 0xA0){
    errorcom =1;
  }
  else{
    errorcom =0;
  }

//trying different recipe to see if there is coherent values in CC_meas
  usbc.pd_reset();
  //   delay(1000);
  //usbc.init();
  //   delay(1000);


  int cc1_meas, cc2_meas;
 // define the port configuration and set the variables accordingly
  usbc.get_cc(&cc1_meas, &cc2_meas);
  Serial.print("CC1 level = ");
  switch (cc1_meas) {
    case TYPEC_CC_VOLT_OPEN :
      Serial.println("Open");
      break;
    case TYPEC_CC_VOLT_RA :
      Serial.println("Ra pull-down");
      break;
    case TYPEC_CC_VOLT_RD :
      Serial.println("Rd pull-down");
      break;
    case TYPEC_CC_VOLT_SNK_DEF :
      Serial.println("Connected with default power");
      break;
    case TYPEC_CC_VOLT_SNK_1_5 :
      Serial.println("Connected with 1.5A at 5V");
      break;
    case TYPEC_CC_VOLT_SNK_3_0 :
      Serial.println("Connected with 3.0A at 5V");
      break;
    default :
      Serial.println("Unknown");
      break;
  }
 // define the port configuration and set the variables accordingly
  Serial.print("CC2 level = ");
  switch (cc2_meas) {
    case TYPEC_CC_VOLT_OPEN :
      Serial.println("Open");
      current =0;
      voltage = 0;
      break;
    case TYPEC_CC_VOLT_RA :
      Serial.println("Ra pull-down");
      break;
    case TYPEC_CC_VOLT_RD :
      Serial.println("Rd pull-down");
      break;
    case TYPEC_CC_VOLT_SNK_DEF :
      Serial.println("Connected with default power");
      current =0;
      voltage = 5;
      break;
    case TYPEC_CC_VOLT_SNK_1_5 :
      Serial.println("Connected with 1.5A at 5V");
      current =1;
      voltage = 5;
      break;
    case TYPEC_CC_VOLT_SNK_3_0 :
      Serial.println("Connected with 3.0A at 5V");
      current =3;
      voltage = 5;
      break;
    default :
      Serial.println("Unknown");
      current =0;
      voltage = 0;
      break;
  }

  if (cc1_meas > cc2_meas) {
    usbc.set_polarity(0);
  } else {
    usbc.set_polarity(1);
  }
 

   Serial.println("FIN");
}

//handles MQTT format msf and sends it
void sendmsgmqtt(){

    
    sniprintf(message, 75, "state: %ld", state);
    Serial.print("\n message envoye: ");
    Serial.println(message);
    client.publish(outTopic1, message);
    timeMsg = millis();

        sniprintf(message, 75, "voltage: %ld", voltage);
    Serial.print("message envoye: ");
    Serial.println(message);
    client.publish(outTopic2, message);
    timeMsg = millis();

            sniprintf(message, 75, "current: %ld", current);
    Serial.print("message envoye: ");
    Serial.println(message);
    client.publish(outTopic3, message);
    timeMsg = millis();

            sniprintf(message, 75, "charge: %ld", charge);
    Serial.print("message envoye: ");
    Serial.println(message);
    client.publish(outTopic4, message);
    timeMsg = millis();

                sniprintf(message, 75, "error: %ld", errorcom);
    Serial.print("message envoye: ");
    Serial.println(message);
    client.publish(outTopic5, message);
    timeMsg = millis();
}

// handle the access point configuration
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

//set up the wifi connection (if there is no Wifi SSID or password allreally in the variables, it switches to an access point)
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

//if communication is lost to the MQTT broker, reconnect procedure
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
}
// callback to recieve mqtt publish
void callback(char* topic, byte* payload, unsigned int length){
    Serial.print("\n message recu; ");
    Serial.println(topic);
    for(int i=0; i<length; i++){
      Serial.print((char) payload[i]);
      s +=((char) payload[i]);
    }
    Serial.println();   
    
}


//setup code here, to run once:
void setup() {
  //serial port setting
  Serial.begin(115200);
  //set GPIO if needed
  pinMode(LED, OUTPUT);
  //set wifi
  setupWifi();
  //set Broker and callback
  client.setServer(broker, 707);
  client.setCallback(callback);
  //setup for USBC devboard
  usbc.init();



}


void loop() {

  //go grab the info from charger
  Prompti2c();
   delay(1000);
   // if disconnected from MQTT broker, retry with procedure
  if (!client.connected()){
    reconnect();
  }
  client.loop();

  // after 2 more seconds data to database
  delay(2000);
  sendmsgmqtt();
// if there is a wifi connection, Flash the led
if(WiFi.status() != WL_CONNECTED){
    digitalWrite(LED, LOW);
  }
  else
  {
  digitalWrite(LED, HIGH);
  delay(500);
  digitalWrite(LED, LOW);
  }
  
}

