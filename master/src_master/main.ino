#include <Arduino.h>
#include <WiFi.h>
#include <esp_now.h>

#define MAX_ARG 2

uint8_t sendId;
uint8_t commandId;
int32_t arg[MAX_ARG];

uint8_t slaveMac[] = {0x04,0x83,0x08,0x0E,0x4F,0xB4};

typedef struct {
  uint8_t nodeId; 
  uint8_t statusId;
  char msg[64];      // メッセージ
} SlvPacket;

typedef struct{
  uint8_t sendId;
  uint8_t commandId;
  int32_t arg[MAX_ARG];
} MstPacket;

MstPacket sendPacket;

void onReceive(const uint8_t *mac, const uint8_t *data, int len) {
  SlvPacket recvPacket;
  memcpy(&recvPacket, data, sizeof(recvPacket));

  Serial.print("[FROM NODE ");
  Serial.print(recvPacket.nodeId);
  Serial.print("] Status: ");
  Serial.println(recvPacket.statusId);
  Serial.print(", ");
  Serial.println(recvPacket.msg);
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onReceive);

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, slaveMac, 6);
  peer.channel = 1;
  peer.encrypt = false;
  esp_now_add_peer(&peer);

  Serial.println("Ready for ASCII commands: sendId,commandId,arg0,arg1");
}


void parseAndSend(const String &line) {
  int tmpSend = 0, tmpCmd = 0, tmpArg0 = 0, tmpArg1 = 0;

  int n = sscanf(line.c_str(), "%d,%d,%d,%d",
                 &tmpSend, &tmpCmd, &tmpArg0, &tmpArg1);

  if (n < 2) return;  // 最低限 sendId, commandId

  sendPacket.sendId    = tmpSend;
  sendPacket.commandId = tmpCmd;
  sendPacket.arg[0]    = (n >= 3) ? tmpArg0 : 0;
  sendPacket.arg[1]    = (n >= 4) ? tmpArg1 : 0;

  esp_now_send(slaveMac, (uint8_t *)&sendPacket, sizeof(sendPacket));

  Serial.printf(
    "SEND -> sendId:%d cmd:%d arg0:%d arg1:%d\n",
    sendPacket.sendId,
    sendPacket.commandId,
    sendPacket.arg[0],
    sendPacket.arg[1]
  );
}


void loop() {
  static String line = "";

  while (Serial.available()) {
    char c = Serial.read();

    // Enter (LF)
    if (c == '\n') {
      line.trim();
      if (line.length() > 0) {
        parseAndSend(line);
      }
      line = "";   // バッファクリア
    }
    // CR は無視（Windows対策）
    else if (c != '\r') {
      line += c;
    }
  }
}
