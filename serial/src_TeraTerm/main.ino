#include <Arduino.h>

#define MAX_ARG 2

uint8_t sendId;
uint8_t commandId;
int32_t arg[MAX_ARG];

void setup() {
  Serial.begin(115200);
  Serial.println("Ready for ASCII commands: sendId,commandId,arg0,arg1");
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) return;

    // デフォルト値
    arg[0] = 0;
    arg[1] = 0;

    int tmpSend = 0, tmpCmd = 0;
    int tmpArg0 = 0, tmpArg1 = 0;

    // 安全に int で受けてからキャスト
    int n = sscanf(line.c_str(), "%d,%d,%d,%d", &tmpSend, &tmpCmd, &tmpArg0, &tmpArg1);

    if (n >= 2) {
      sendId    = (uint8_t)tmpSend;
      commandId = (uint8_t)tmpCmd;
    }
    if (n >= 3) arg[0] = tmpArg0;
    if (n >= 4) arg[1] = tmpArg1;

    Serial.print("sendId: "); Serial.print(sendId);
    Serial.print(", commandId: "); Serial.print(commandId);
    Serial.print(", arg[0]: "); Serial.print(arg[0]);
    Serial.print(", arg[1]: "); Serial.println(arg[1]);
  }
}

