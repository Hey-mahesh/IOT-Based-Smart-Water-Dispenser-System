#include <SoftwareSerial.h>

SoftwareSerial rs485(D5, D6); // RX, TX

void setup() {
  Serial.begin(9600);
  rs485.begin(9600);

  Serial.println("ESP Ready - Receiving Weight");
}

void loop() {

  if (rs485.available()) {
    String weight = rs485.readStringUntil('\n');

    Serial.print("Weight Received: ");
    Serial.println(weight);
  }
}