#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include "CommandBuffer.h"
#include "Timer.h"

const char *const SSID = "";
const char *const PW = "";
const char *const HOSTNAME = "";

const int SENSOR_PIN = 33;
const int BUZZER_PIN = 26;

WiFiServer server(80);
WiFiClient client;
WiFiClient reportClient;
IPAddress reportIp(192, 168, 0, 3);
int reportPort = 30000;

bool trigger = false;

void reconnect()
{
  WiFi.disconnect();
  WiFi.begin(SSID, PW);
  delay(10000);
  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(SSID, PW);
    delay(10000);
  }
}

void sendTrigger()
{
  // SEND TRIGGER TO RPI SERVER
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.reconnect();
  }
  if (reportClient.connect(reportIp, reportPort, 500)) {
    reportClient.print("trigger:on");
    delay(20);
  }
  reportClient.stop();
}

unsigned long blinkTime = 400;
unsigned long onTime = 10000;
unsigned long detectionTime = 1000;
bool enabled = true;
bool testBuzzer = false;
bool report = true;

Timer detectionTimer(detectionTime);
Timer outputTimer(onTime);
Timer blinkTimer(blinkTime);

void tone(int pin, int freq)
{
  ledcAttachPin(BUZZER_PIN, 3);
  ledcWriteNote(3, NOTE_C, 6);
}

void noTone(int pin)
{
  ledcDetachPin(pin);
}

//**************************
//        COMMANDS
//**************************
void eReport()
{
  report = true;
}
void dReport()
{
  report = false;
}

void enable()
{
  enabled = true;
}

void disable()
{
  enabled = false;
}

void setBuzzerTime(int time)
{
  onTime = time;
  outputTimer.setTime(onTime);
}

void setDetectionTime(int time)
{
  detectionTime = time;
  detectionTimer.setTime(detectionTime);
}

void setBlinkTimer(int time)
{
  blinkTime = time;
  blinkTimer.setTime(blinkTime);
}

IPAddress localIp(192, 168, 0, 6);
IPAddress gateway(192, 168, 0, 1);
IPAddress mask(255, 255, 255, 0);
CommandBuffer buffer(40);

void setup()
{
  // put your setup code here, to run once:

  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  WiFi.config(localIp, gateway, mask);
  WiFi.begin(SSID, PW);
  delay(10000);

  while (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(SSID, PW);
    delay(10000);
  }
  Serial.print("Connected to wifi. My address: ");
  Serial.println(WiFi.localIP());

  ledcSetup(3, 22000, 16);
  server.begin();
}
bool buzzing = false;
bool toggler = false;
void loop()
{

  // put your main code here, to run repeatedly:
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connection lost, reconnecting...");
    reconnect();
  }
  if (!trigger) {

    if (client = server.available())

    {

      while (client.available())

      {
        buffer.push_back(client.read());
      }
      delay(10);

      // Serial.println("Received comand");
      // Serial.println(buffer.getCommand());
      if (buffer.compareCommand("enable")) {
        // Serial.write("ALARM ENABLED");
        enable();
      }
      else if (buffer.compareCommand("disable")) {
        // Serial.write("ALARM DISABLED");
        disable();
      }
      else if (buffer.compareCommand("setTime")) {
        int param = buffer.getParsedParameter();
        if (param > 0) {
          // Serial.write("CHANGED BUZZER TIME");
          setBuzzerTime(param);
        }
      }
      else if (buffer.compareCommand("setDetectionTime")) {
        int param = buffer.getParsedParameter();
        if (param > 0) {
          // Serial.write("CHANGED DETECTION TIME");
          setDetectionTime(param);
        }
      }
      else if (buffer.compareCommand("test")) {
        // Serial.write("TESTING COMMAND");
        testBuzzer = true;
      }
      else if (buffer.compareCommand("setBlink")) {
        // Serial.write("Setting blinkTimer");
        int param = buffer.getParsedParameter();
        setBlinkTimer(param);
      }
      else if (buffer.compareCommand("dReport")) {
        dReport();
      }
      else if (buffer.compareCommand("eReport")) {
        eReport();
      }
      buffer.clear();
      client.stop();
    }
  }
  if (enabled) {

    // TRIGGER LOGIC
    if (!trigger) {
      if (digitalRead(SENSOR_PIN) == HIGH && detectionTimer.read()) {
        trigger = true;
        if (report) {
          sendTrigger();
        }
        detectionTimer.reset();
      }
      else if (testBuzzer) {
        trigger = true;
        if (report) {
          sendTrigger();
        }
        testBuzzer = false;
      }
    }

    // BUZZER LOGIC
    if (trigger && !outputTimer.read()) {

      if (!buzzing) {
        buzzing = true;
        tone(BUZZER_PIN, 1000);
      }
      else if (blinkTimer.read()) {
        toggler = !toggler;
        blinkTimer.reset();
        if (toggler) {
          noTone(BUZZER_PIN);
        }
        else {
          tone(BUZZER_PIN, 1000);
        }
      }
    }
    else if (trigger) {
      toggler = false;
      // Serial.println("NO BUZZIN");
      outputTimer.reset();
      noTone(BUZZER_PIN);
      trigger = false;
    }
  }
  else {
    noTone(BUZZER_PIN);
  }
}
