//MACアドレス確認用。pio build時に確認することも可能なのでいらない可能性あり。
#include <WiFi.h>

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);   // ESP-NOWでは必須
  delay(100);

  Serial.print("ESP32 MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
}