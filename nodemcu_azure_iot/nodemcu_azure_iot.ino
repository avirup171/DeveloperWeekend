#include "SoftwareSerial.h"
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <AzureIoTHubMQTTClient.h>
#include <WiFiManager.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>


SoftwareSerial serialData(2, 0);
WiFiClient espClient;


#define OLED_RESET D0
Adafruit_SSD1306 display(LED_BUILTIN);

extern const unsigned char PROGMEM wifi1[];
extern const unsigned char PROGMEM locked[];

char buff[255];
int count=0;

// Azure IoT Hub Settings --> CHANGE THESE
#define IOTHUB_HOSTNAME         "mcu.azure-devices.net"
#define DEVICE_ID               "nodemcu"
#define DEVICE_KEY              "bLRTwU5meteX8Pgk4XpNuM/3yE8kk1WfaUlxSxLt1Sw=" //Primary key of the device

WiFiClientSecure tlsClient;
AzureIoTHubMQTTClient client(tlsClient, IOTHUB_HOSTNAME, DEVICE_ID, DEVICE_KEY);
WiFiEventHandler  e1, e2;

unsigned long lastMillis = 0;

void connectToIoTHub(); // <- predefine connectToIoTHub() for setup()
void onMessageCallback(const MQTT::Publish& msg);

void onClientEvent(const AzureIoTHubMQTTClient::AzureIoTHubMQTTClientEvent event) {
    if (event == AzureIoTHubMQTTClient::AzureIoTHubMQTTClientEventConnected) {

        Serial.println("Connected to Azure IoT Hub");

        //Add the callback to process cloud-to-device message/command
        client.onMessage(onMessageCallback);
    }
}

void onMessageCallback(const MQTT::Publish& msg) {

    //Handle Cloud to Device message by yourself.

//    if (msg.payload_len() == 0) {
//        return;
//    }

//    Serial.println(msg.payload_string());
}

void connectToIoTHub() 
{
    displaydata("Connecting to server ");
    Serial.print("\nBeginning Azure IoT Hub Client... ");
    if (client.begin()) {
        Serial.println("OK");
    } else {
        Serial.println("Could not connect to MQTT");
    }
    display.clearDisplay();
    display.display();
}

void wifiloop()
{
  //WIFI setup
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(15,14);
  display.println("Connect to ");
  display.drawBitmap(85, 10,  wifi1, 16, 16, WHITE);
  display.display();
  delay(300);
}

void setup() 
{
  ESP.wdtDisable();
  serialData.begin(9600);
  Serial.begin(115200);
  delay(10);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  delay(500);
  //Clear
  display.clearDisplay(); 
  // text display tests
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(25,14);
  display.println("Prescriber360");
  display.display();
  delay(1500);
  WiFiManager wifiManager;
  wifiloop();
  //wifiManager.resetSettings();
  wifiManager.autoConnect("NodeMCU");
  display.clearDisplay();
  display.display();
  delay(300);
  connectToIoTHub();
  ESP.wdtEnable(5000);
}

void loop() 
{
  display.display();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(15,14);
  display.println("Active ");
  display.drawBitmap(100, 0,  locked, 16, 16, WHITE);
  display.display();
  delay(1);
  client.run();
  if(serialData.available())
  {
    while(serialData.available())
    {
      ESP.wdtFeed();
      buff[count++]=serialData.read();
      if(buff[count]=='\n')
      break;
    }
    Serial.println(buff);
    
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(15,14);
    display.println("Finger matched ");
    delay(1000);
    display.display();
    display.clearDisplay();
    display.display();
    delay(2);
    publishData();
    yield();
  }
  else
  {
    count=0;
    buff[0]=(char)0;
    memset(buff, 0, sizeof(buff));    
    Serial.println("No data");
    Serial.flush();
  }
  delay(100);
}
void displaydata(String s)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,15);
  display.println(s);
  display.display();
  delay(300);
}
void publishData()
{
  Serial.println("In publish");
//  if(buff=="Verified ID:1")
//  {
    Serial.println("Condition 1");
    Serial.println(buff);
    if (client.connected()) 
    {
            Serial.println("Condition 2");
            //Get current timestamp, using Time lib
            time_t currentTime = now();
            //Or instead, use this more convenient way
            AzureIoTHubMQTTClient::KeyValueMap keyVal = {{"order_status", "TRUE"},{"device_id", DEVICE_ID}, {"EventTime", currentTime}};
            client.sendEventWithKeyVal(keyVal);
        }
        displaydata("Order placed ");
        delay(500);
        display.clearDisplay();
        display.display();
//    }    
    delay(10); // <- fixes some issues with WiFi stability  
}

