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
#define IN4 6

float calibration_factor = 680;

// ---------------- CONTROL ----------------
float targetWeight = 0;
bool targetSet = false;

float tolerance = 5.0;
int stableCount = 0;

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
  Serial.println("Enter Target Weight (0 to reset):");
}

void loop() {

  // -------- SERIAL INPUT --------
  if (Serial.available()) {
    String inputStr = Serial.readStringUntil('\n');
    inputStr.trim();

    if (inputStr.length() > 0) {
      float input = inputStr.toFloat();

      if (input == 0 && inputStr == "0") {
        targetSet = false;
        targetWeight = 0;
        stableCount = 0;

        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);

        Serial.println("System Reset");

        display.displayClear();
        display.print("RESET");
        delay(1500);

        display.print("SET WT");
      }
      else if (input > 0) {
        targetWeight = input;
        targetSet = true;
        stableCount = 0;

        Serial.print("Target Set: ");
        Serial.println(targetWeight);

        display.displayClear();
        display.print("SET OK");
        delay(1500);
      }
    }
  }

  // -------- READ WEIGHT --------
  float weight = scale.get_units(10);

  if (weight > -5 && weight < 5) {
    weight = 0;
  }

  // -------- SEND TO ESP (RS485) --------
  rs485.println(weight);

  // -------- DISPLAY --------
  char weightStr[10];
  dtostrf(weight, 4, 1, weightStr);

  display.displayClear();
  display.print(weightStr);

  Serial.print("Weight: ");
  Serial.println(weightStr);

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

        Serial.println("Target Reached");

        display.displayClear();
        display.print("DONE");
        delay(1500);

        targetSet = false;
      }
    }

    else if (weight > (targetWeight + tolerance)) {
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);

      Serial.println("Overshoot Stop");

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