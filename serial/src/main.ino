#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.println("ESP32 ready");
}

void loop() {
  // PC → ESP32
  if (Serial.available() > 0) {
    char received = Serial.read();
    if (received == '1') {
      Serial.println("ESP32 received: 1");
    } 
    else if (received == '0') {
      Serial.println("ESP32 received: 0");
    } 
    else {
      Serial.print("Unknown: ");
      Serial.println(received);
    }
  }
  delay(10);
}
