/*
=> Additional Board manager - https://dl.espressif.com/dl/package_esp32_index.json
  Libraries Needed to be installed - 
 *  OneWire by Paul Stoffregen
 *  DallasTemperature by Miles Burton
 *  ThingSpeak by MathWorks
 *  PubSubClient by Nick O'Leary
 *  WiFi (Built-in)
*/

#include <DallasTemperature.h>
#include <OneWire.h>
#include <ThingSpeak.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <WiFi.h>

// WATER TEMPERATURE SENSOR DETAILS

#define BUZZER_PIN 18
#define WATER_TEMP_PIN 15
#define MAX_WATER_TEMP 30
#define MIN_WATER_TEMP 20

// Setup a oneWire instance to communicate
OneWire oneWire(WATER_TEMP_PIN);
// Pass our oneWire reference to Dallas Temperature sensor object 
DallasTemperature waterTempObj(&oneWire);

float tempC = 0;

// ----------------------------------
// WATER LEVEL SENSOR DETAILS

#define LVL_POWER_PIN 16
#define WATER_SENSOR_PIN 33
#define LED1 1
#define LED2 2
#define LED3 3
#define LED4 4
#define LED5 5

#define LEVEL1 100
#define LEVEL2 500
#define LEVEL3 1000
#define LEVEL4 1300
#define LEVEL5 1600

int waterLevelReading = 0;

// ----------------------------------
// THINGSPEAK DETAILS

const char *ssid = "";
const char *password = "";

const char* server = "mqtt3.thingspeak.com";
char mqttUserName[] = "bhavberi";
char mqttPass[] = "5UKHX0B9UOS4MRAZ";
long channelID = 0; // PUT ID
char writeAPI[] = "0"; // PUT WRITE API

// Setting up WiFi Client object
WiFiClient client;
// Setting up Publisher Sub-Client Object
PubSubClient mqttClient(server, 1883, client);


void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
  
  pinMode(LVL_POWER_PIN, OUTPUT);
  digitalWrite(LVL_POWER_PIN, LOW);
  
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW); 
  pinMode(LED2, OUTPUT);
  digitalWrite(LED2, LOW); 
  pinMode(LED3, OUTPUT);
  digitalWrite(LED3, LOW); 
  pinMode(LED4, OUTPUT);
  digitalWrite(LED4, LOW); 
  pinMode(LED5, OUTPUT);
  digitalWrite(LED5, LOW); 

  // ---------------------------------
  // Beginnng/Setting up Water Temp Object
  waterTempObj.begin();

  // ---------------------------------
  //Setting up Wifi Connection
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Setting up MQTT Broker details for ThingSpeak
  mqttClient.setServer(server, 1883);
}

void loop() {
  // Reading value from Water Level Sensor using waterTempObj object
  
  waterTempObj.requestTemperatures(); 
  tempC = waterTempObj.getTempCByIndex(0);
  Serial.println(tempC);
  digitalWrite(BUZZER_PIN, HIGH);
  if(tempC < MIN_WATER_TEMP || tempC > MAX_WATER_TEMP)
  {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // ------------------------------------
  // Reading Water Level Value
  digitalWrite(LVL_POWER_PIN, HIGH);
  delay(5);
  waterLevelReading = analogRead(WATER_SENSOR_PIN);
  digitalWrite(LVL_POWER_PIN, LOW);
  Serial.print(waterLevelReading);

  // Setting up LEDs Based on Water Level
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW); 
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  digitalWrite(LED5, LOW);

  if(waterLevelReading >= LEVEL1)
  {
    digitalWrite(LED1, HIGH);
  }
  if(waterLevelReading >= LEVEL2)
  {
    digitalWrite(LED2, HIGH);
  }
  if(waterLevelReading >= LEVEL3)
  {
    digitalWrite(LED3, HIGH);
  }
  if(waterLevelReading >= LEVEL4)
  {
    digitalWrite(LED4, HIGH);
  }
  if(waterLevelReading >= LEVEL5)
  {
    digitalWrite(LED5, HIGH);
  }

  // -------------------------------
  // ThingSpeak Publishing the Data

  // To be Added
  
  delay(500);
}
