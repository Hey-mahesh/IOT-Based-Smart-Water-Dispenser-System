#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "HX711.h"
#include <SoftwareSerial.h>

// ---------------- RS485 ----------------
SoftwareSerial rs485(8, 7); // RX, TX

// ---------------- DISPLAY ----------------
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4

#define DATA_PIN 11
#define CS_PIN   10
#define CLK_PIN  13

MD_Parola display = MD_Parola(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// ---------------- HX711 ----------------
#define DT 3
#define SCK 2

HX711 scale;

// ---------------- MOTOR ----------------
#define IN3 5
#define IN4 4

float calibration_factor = 680;

// ---------------- CONTROL ----------------
float targetWeight = 0;
bool targetSet = false;

float tolerance = 5.0;
int stableCount = 0;

// -------- DISPLAY CONTROL --------
unsigned long showTime = 0;
bool showTarget = false;

// -------- FUNCTION TO HANDLE INPUT --------
void processInput(String inputStr) {
  inputStr.trim();

  if (inputStr.length() > 0) {
    float input = inputStr.toFloat();

    if (input == 0 && inputStr == "0") {
      targetSet = false;
      targetWeight = 0;
      stableCount = 0;

      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);

      Serial.println("RESET");

      display.displayClear();
      display.print("RESET");
      delay(1500);

      display.print("SET WT");
    }
    else if (input > 0) {
      targetWeight = input;
      targetSet = true;
      stableCount = 0;

      Serial.print("TARGET: ");
      Serial.println(targetWeight);

      // Show target for 2 sec
      showTarget = true;
      showTime = millis();
    }
  }
}

void setup() {
  Serial.begin(9600);
  rs485.begin(9600);

  display.begin();
  display.setIntensity(5);
  display.displayClear();

  scale.begin(DT, SCK);
  scale.set_scale(calibration_factor);
  scale.tare();

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  display.print("SET WT");
}

void loop() {

  // -------- SERIAL INPUT --------
  if (Serial.available()) {
    String inputStr = Serial.readStringUntil('\n');
    processInput(inputStr);
  }

  // -------- RS485 INPUT --------
  if (rs485.available()) {
    String inputStr = rs485.readStringUntil('\n');
    processInput(inputStr);
  }

  // -------- READ WEIGHT --------
  float weight = scale.get_units(10);

  if (weight > -5 && weight < 5) {
    weight = 0;
  }

  // -------- SEND TO ESP --------
  rs485.println(weight);

  // -------- DISPLAY --------
  display.displayClear();

  if (showTarget && (millis() - showTime < 2000)) {
    char targetStr[10];
    dtostrf(targetWeight, 4, 1, targetStr);
    display.print(targetStr);
  } else {
    showTarget = false;

    char weightStr[10];
    dtostrf(weight, 4, 1, weightStr);
    display.print(weightStr);
  }

  // -------- SERIAL OUTPUT --------
  Serial.print("Weight: ");
  Serial.println(weight);

  // -------- MOTOR LOGIC --------
  if (targetSet) {

    if (weight < (targetWeight - tolerance)) {
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      stableCount = 0;
    }

    else if (weight >= (targetWeight - tolerance) && weight <= (targetWeight + tolerance)) {
      stableCount++;

      if (stableCount >= 10) {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);

        Serial.println("DONE");

        display.displayClear();
        display.print("DONE");
        delay(1500);

        targetSet = false;
      }
    }

    else if (weight > (targetWeight + tolerance)) {
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);

      Serial.println("STOP");

      display.displayClear();
      display.print("STOP");
      delay(1500);

      targetSet = false;
    }

  } else {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }

  delay(300);
}
