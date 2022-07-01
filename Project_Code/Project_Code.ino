// TEAM -> Electro-Power
/* Members :-
    Bhav Beri (Software)
    Prisha (Software)
    Harshit Aggarwal (Hardware)
    Vanshika Dhingra (Hardware)
*/

/*
  => Additional Board manager - https://dl.espressif.com/dl/package_esp32_index.json
  Libraries Needed to be installed -
    OneWire by Paul Stoffregen
    DallasTemperature by Miles Burton
    ThingSpeak by MathWorks
    PubSubClient by Nick O'Leary
    WiFi (Built-in)
*/

#include <DallasTemperature.h>
#include <OneWire.h>
#include <ThingSpeak.h>
#include <PubSubClient.h>
#include <WiFiClient.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <ArduinoJson.h>

// WATER TEMPERATURE SENSOR DETAILS

#define BUZZER_PIN 18
#define WATER_TEMP_PIN 4
#define MAX_WATER_TEMP 38
#define MIN_WATER_TEMP 36

// Setup a oneWire instance to communicate
OneWire oneWire(WATER_TEMP_PIN);
// Pass our oneWire reference to Dallas Temperature sensor object
DallasTemperature waterTempObj(&oneWire);

float tempC = 0;

// ---------------------------------------------
// WATER LEVEL ULTRASONIC SENSOR DETAILS

#define SOUND_SPEED 0.034

#define TRIG_PIN 32
#define ECHO_PIN 35

#define LVL_POWER_PIN 16
#define LED1 26
#define LED2 25
#define LED3 12
#define LED4 33
#define LED5 14

#define LEVEL1 1
#define LEVEL2 4
#define LEVEL3 6
#define LEVEL4 8
#define LEVEL5 10

int waterLevelReading = 0;

// ----------------------------------------------
// THINGSPEAK DETAILS

const char *ssid = "V2027";
const char *password = "HARSHIT12";

const char* server = "mqtt3.thingspeak.com";
char mqttUserName[] = "bhavberi";
char mqttPass[] = "5UKHX0B9UOS4MRAZ";
long channelID = 1779679; // PUT ID
char writeAPI[] = "51X915EZ4PJ001QX"; // PUT WRITE API

// Setting up WiFi Client object
WiFiClient client;
// Setting up Publisher Sub-Client Object
PubSubClient mqttClient(server, 1883, client);

// ----------------------------------------------
// Onem2m Details
String cse_ip = "192.168.43.54"; // IP from ipconfig/ifconfig
String cse_port = "8080";
String onem2m_server = "http://" + cse_ip + ":" + cse_port + "/~/in-cse/in-name/";
String ae1 = "Water_Temp";
String ae2 = "Water_Level";
String cnt = "node";


// -----------------------------------------------
// PIR SENSOR DETAILS

#define PIR_PIN 13
#define LED_ON_TIME 5

int pirReading = 0;

// Timer: Auxiliary variables
unsigned long now = millis(); // Current Time
unsigned long lastTrigger = 0; // Last time when motion detected
boolean startTimer = false; // Check if motion is still going on

// Checks if motion was detected, sets LED based on water level and starts a timer
// IRAM_ATTR is used to run the interrupt code in RAM
void IRAM_ATTR detectsMovement() {
  // Serial.println("MOTION DETECTED!!!");
  level_read();
  startTimer = true;
  lastTrigger = millis();
}

// -------------------------------------------------------------
// SETTING UP DIFFERENT SENSORS & OBJECTS
void setup() {
  Serial.begin(115200);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
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

  // -----------------------------------------------
  // PIR SENSOR

  pinMode(PIR_PIN, INPUT);
  // Attaching interrupt for better catching of the motion (Rising edge of input)
  //pinMode(PIR_PIN, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(PIR_PIN), detectsMovement, RISING);

  // -----------------------------------------------
  // Beginnng/Setting up Water Temp Object
  waterTempObj.begin();

  // -----------------------------------------------
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

// -------------------------------------------------------------
// LOOPING PART OF MICRO-CONTROLLER
void loop() {
  // MQTT Client Connection Establishment
  while (!mqttClient.connected())
  {
    //Serial.println("Connect Loop");
    Serial.println(mqttClient.connect("DQQsEw4bIB4DADQ1MCkUAxM", "DQQsEw4bIB4DADQ1MCkUAxM", "BNyDp71sBF5AjHADJ4OqIA4k"));
    Serial.println(mqttClient.connected());
  }

  //Serial.println("MQTT Connected");
  mqttClient.loop();

  delay(500);

  // ------------------------------------
  // Updating Level LEDs if motion has stopped

  if (digitalRead(PIR_PIN))
  {
    // Reading value from Water Level Sensor using waterTempObj object
    water_temp_read();
    
    detectsMovement();

    // Publishing the Data
    mqttPublish(channelID, writeAPI, tempC, waterLevelReading);
    onem2mPublish(tempC, waterLevelReading);
  }

  now = millis(); // Current time
  if (startTimer && (now - lastTrigger > (LED_ON_TIME * 1000))) {
    // Serial.println("Motion stopped...");
    digitalWrite(LED1, LOW);
    digitalWrite(LED2, LOW);
    digitalWrite(LED3, LOW);
    digitalWrite(LED4, LOW);
    digitalWrite(LED5, LOW);
    startTimer = false;

    digitalWrite(BUZZER_PIN, HIGH);
  }

  delay(1000);
}

void onem2mPublish(int level, int temp)
{
  String val = String(temp);
  int code;
  
  HTTPClient http;
  
  http.begin(onem2m_server + ae2 + "/" + cnt + "/");
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");
  code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": " + String(val) + "}}");
  Serial.print("Onem2m Temp POST Code - ");
  Serial.println(code);
  if (code == -1)
    Serial.println("Connection failed.. (Temperature)");
  http.end();

  val = String(level);
  http.begin(onem2m_server + ae1 + "/" + cnt + "/");
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");
  code = http.POST("{\"m2m:cin\": {\"cnf\":\"application/json\",\"con\": " + String(val) + "}}");
  Serial.print("Onem2m Height POST Code - ");
  Serial.println(code);
  if (code == -1)
    Serial.println("Connection failed.. (Water Level)");
  http.end();
}

void mqttPublish(long pubChannelID, char* pubWriteAPIKey, int level, int temp)
{
  if (level < 1 || temp < -10 || temp >= 85)
    return;

  // Publishing MQTT Data
  String dataString = "field1=" + String(temp) + "&field2=" + String(level);
  String topicString = "channels/" + String(pubChannelID) + "/publish";
  Serial.println(mqttClient.publish(topicString.c_str(), dataString.c_str()));
  Serial.println(pubChannelID);
}

// ----------------------------------------------------------
// Reading value from Water Level Sensor using waterTempObj object
void water_temp_read()
{
  waterTempObj.requestTemperatures();
  tempC = waterTempObj.getTempCByIndex(0);
  Serial.print("Temperature: ");
  Serial.println(tempC);
  digitalWrite(BUZZER_PIN, HIGH);
  if (tempC < MIN_WATER_TEMP || tempC > MAX_WATER_TEMP)
  {
    digitalWrite(BUZZER_PIN, LOW);
  }
}

// ----------------------------------------------------
// Reading Water Level Value & setting LEDs as needed
void level_read()
{
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  digitalWrite(LED5, LOW);

  digitalWrite(LVL_POWER_PIN, HIGH);
  delay(5);

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  int duration = pulseIn(ECHO_PIN, HIGH);
  waterLevelReading = duration * SOUND_SPEED / 2;
  waterLevelReading = 12 - waterLevelReading;

  Serial.print("Water Level: ");
  Serial.println(waterLevelReading);
  // Setting up LEDs Based on Water Level

  if (waterLevelReading >= LEVEL1)
  {
    digitalWrite(LED1, HIGH);
  }
  if (waterLevelReading >= LEVEL2)
  {
    digitalWrite(LED2, HIGH);
  }
  if (waterLevelReading >= LEVEL3)
  {
    digitalWrite(LED3, HIGH);
  }
  if (waterLevelReading >= LEVEL4)
  {
    digitalWrite(LED4, HIGH);
  }
  if (waterLevelReading >= LEVEL5)
  {
    digitalWrite(LED5, HIGH);
  }

  digitalWrite(LVL_POWER_PIN, LOW);
}
