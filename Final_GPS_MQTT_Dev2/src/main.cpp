#include <Arduino.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>
#include <WiFi.h>
#include <PubSubClient.h>


// Update these with values suitable for your network.
const char* ssid = "htl-IoT";
const char* password = "iot..2015";
const char* mqtt_server = "iotmqtt.htl-klu.at";

#define MQTT_USER "htl-IoT"
#define MQTT_PASSWORD "iot..2015"
#define MQTT_SERIAL_PUBLISH_CH "/icircuit/ESP32/serialdata/tx"
#define MQTT_SERIAL_RECEIVER_CH "/icircuit/ESP32/serialdata/rx"

WiFiClient wifiClient;

PubSubClient client(wifiClient);

#define RXD2 16
#define TXD2 17
TinyGPSPlus gps;

void setup_wifi() {
    delay(10);
    // We start by connecting to a WiFi network
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    randomSeed(micros());
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32Client2";
    clientId += String(random(0xfff7), HEX);
    // Attempt to connect
    if (client.connect("ESP8266Client2", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected");
      //Once connected, publish an announcement...
      client.publish("/icircuit/presence/ESP32/", "hello world");
      // ... and resubscribe
      client.subscribe(MQTT_SERIAL_RECEIVER_CH);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void callback(char* topic, byte *payload, unsigned int length) {
    Serial.println("-------new message from broker-----");
    Serial.print("channel:");
    Serial.println(topic);
    Serial.print("data:");  
    Serial.write(payload, length);
    Serial.println();
}

void setup()
{
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
   
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  reconnect();
}
void displayINFO();

void publishSerialData(char *serialData){
  if (!client.connected()) {
    reconnect();
  }
  client.publish("htl/4bhel/StrojGPS", serialData);
}

void loop()
{
  while (Serial2.available())
  {
    char val = Serial2.read();
   // Serial.write(val);
    gps.encode(val);
  }

  displayINFO();
}


long nextTimeToExecuteDisplay = 0;
int delayValDisplay = 5000;
void displayINFO()
{



  if (millis() > nextTimeToExecuteDisplay)
  {
    Serial.println();
    Serial.print(F("Location:"));

    Serial.print(gps.location.lat(), 8);
    
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 8);
    
     String localizacion = String("\"name\":\"Stroj\"")+String(", \"lat\":") + String(gps.location.lat(), 6) +String(", \"lon\":") + String(gps.location.lng(), 6);
    char dato[localizacion.length() + 1];
    localizacion.toCharArray(dato, localizacion.length());
    client.publish("htl/4bhel/StrojGPS", dato);
    nextTimeToExecuteDisplay = millis() + delayValDisplay;
  }
}
