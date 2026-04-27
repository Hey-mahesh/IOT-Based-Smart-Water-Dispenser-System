#define BLYNK_TEMPLATE_ID "TMPL3foYLqB3x"
#define BLYNK_TEMPLATE_NAME "Acropolice"
#define BLYNK_AUTH_TOKEN "sEHdvXhNv5D94GmAiSYiW2kR8v6vbIj0"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>

// RS485
SoftwareSerial rs485(D5, D6);

// Blynk Credentials

// WiFi
char ssid[] = "mahesh";
char pass[] = "maheshphand";

// ThingSpeak
const char* server = "api.thingspeak.com";
String writeAPI = "J3L8W7PW69JR5CV5";

WiFiClient client;
BlynkTimer timer;

// -------- VARIABLES --------
float latestWeight = 0;
String buffer = "";

bool showCmd = false;
String cmdValue = "";
unsigned long showCmdTime = 0;

// -------- READ FROM ARDUINO --------
void readWeight() {

  while (rs485.available()) {
    char c = rs485.read();

    if (c == '\n') {
      buffer.trim();

      if (buffer.length() > 0) {
        latestWeight = buffer.toFloat();

        if (!showCmd) {
          Serial.print("W: ");
          Serial.println(latestWeight);
        }

        // Send to Blynk
        Blynk.virtualWrite(V1, latestWeight);
      }

      buffer = "";
    } else {
      buffer += c;
    }
  }
}

// -------- SHOW CMD --------
void showCommand() {
  if (showCmd) {
    if (millis() - showCmdTime < 2000) {
      Serial.print("CMD: ");
      Serial.println(cmdValue);
    } else {
      showCmd = false;
    }
  }
}

// -------- SEND TO THINGSPEAK --------
void sendToThingSpeak() {

  if (client.connect(server, 80)) {

    String url = "/update?api_key=" + writeAPI +
                 "&field1=" + String(latestWeight);

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + server + "\r\n" +
                 "Connection: close\r\n\r\n");

    client.stop();

    Serial.println("Uploaded");
  }
}

// -------- RECEIVE FROM BLYNK --------
BLYNK_WRITE(V0) {

  int value = param.asInt();

  rs485.println(value);

  cmdValue = String(value);
  showCmd = true;
  showCmdTime = millis();
}

// -------- SETUP --------
void setup() {
  Serial.begin(9600);
  rs485.begin(9600);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  // Timers
  timer.setInterval(100L, readWeight);        // fast read
  timer.setInterval(200L, showCommand);       // show cmd
  timer.setInterval(15000L, sendToThingSpeak); // upload
}

// -------- LOOP --------
void loop() {
  Blynk.run();
  timer.run();
}
