#include "arduino.h"

typedef struct __attribute__((packed)) {
  uint8_t sendId;
  uint8_t commandId;
  int32_t arg[2];
} Cmd_t;

Cmd_t from_PC;

void setup() {
  Serial.begin(115200);
}

void loop() {
  if (Serial.available() >= sizeof(Cmd_t)) {
    Serial.readBytes((uint8_t*)&from_PC, sizeof(Cmd_t));

    Serial.print("sendId: ");
    Serial.print(from_PC.sendId);
    Serial.print(", commandId: ");
    Serial.print(from_PC.commandId);
    Serial.print(", arg[0]: ");
    Serial.print(from_PC.arg[0]);
    Serial.print(", arg[1]: ");
    Serial.println(from_PC.arg[1]);
  }
}
