#include "HX711.h"

#define DT 3
#define SCK 2

HX711 scale;


float calibration_factor = 670;

void setup() {
  Serial.begin(9600);

  scale.begin(DT, SCK);
  scale.set_scale();
  scale.tare();  // reset to 0

  Serial.println("=== Calibration Mode ===");
  Serial.println("Remove all weight...");
  delay(3000);

  Serial.println("Place known weight on load cell");
  Serial.println("Use '+' or '-' to adjust calibration");
}

void loop() {

  scale.set_scale(calibration_factor);

  float weight = scale.get_units(5);

  Serial.print("Weight: ");
  Serial.print(weight);
  Serial.print(" g | Calibration Factor: ");
  Serial.println(calibration_factor);

  if (Serial.available()) {
    char input = Serial.read();

    if (input == '+') calibration_factor += 10;
    if (input == '-') calibration_factor -= 10;
  }

  delay(500);
}



