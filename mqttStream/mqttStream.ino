#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

const char* mqtt_server = "iot.eclipse.org";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;
char message_buff[100];

void callback(char* topic, byte* payload, unsigned int length) 
{
  int i = 0;
  Serial.println("Message arrived:  topic: " + String(topic));
  Serial.println("Length: " + String(length,DEC));
  // create character buffer with ending null terminator (string)
  for(i=0; i<length; i++) {
    message_buff[i] = payload[i];
  }
  message_buff[i] = '\0';
  String msgString = String(message_buff);
  Serial.println("Payload: " + msgString);
  int state = digitalRead(14);  // get the current state of GPIO1 pin
  /*
   * IO PIN MAPPING OF NODEMCU
   * 
   * IO INDEX - GPIO PINS
   * 
   * o-16
   * 1-5
   * 2-4
   * 3-0
   * 4-2
   * 5-14
   * 6-12
   * 7-13
   * 8-15
   * 9-3
   * 10-1
   * 11-9
   * 12-10
   * 
   */
}

void reconnect() 
{
  // Loop until we're reconnected
  while (!client.connected()) 
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("blackspektro/outtopic", "DID 001 ONLINE CONFIRMED");
      // ... and resubscribe
      client.subscribe("avirup171");
      client.subscribe("sit/it");
    }
    else 
    { 
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup()
{
  //ESP.reset();
  delay(1000);
  pinMode(16, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(14, OUTPUT);
  digitalWrite(16,HIGH);
  digitalWrite(5,HIGH);
  digitalWrite(4,HIGH);
  digitalWrite(0,HIGH);
  digitalWrite(2,HIGH);
  digitalWrite(14,HIGH);
  digitalWrite(D1,LOW);
  digitalWrite(D2,LOW);
  digitalWrite(D3,LOW);
  Serial.begin(115200);
  delay(10);
  WiFiManager wifiManager;
//wifiManager.resetSettings(); 
  wifiManager.autoConnect("NodeMCU");
  Serial.println("Connected to network!!!");
  Serial.println();
  Serial.println();  
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
void loop() 
{
  if (!client.connected()) 
  {
    reconnect();
  }
  client.loop();
  char JSONMessage[] ="{\"SensorType\":\"Temperature\", \"Value\": 10}";
  long now = millis();
  if (now - lastMsg > 2000) 
  {
    lastMsg = now;
    ++value;
    //snprintf (msg, 75, "MSG_BLACKSPEKTRO_DID_001_%ld", value);

    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish("blackspektro/outtopic", JSONMessage);
    if(value>20000)
      value=0;
  }
}
