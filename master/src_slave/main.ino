#include <WiFi.h>
#include <esp_now.h>

#define NODE_ID 1   // 子機ごとに変更
uint8_t masterMac[] = {0x78, 0x42, 0x1C, 0x2D, 0xF4, 0x9C}; // ESP32-AのMAC

typedef struct {
  uint8_t nodeId; 
  uint8_t statusId;
  char msg[64];    //char *は使えない
} SlvPacket;

typedef struct{
  uint8_t sendId;
  uint8_t commandId;
  int32_t arg[2];
} MstPacket;

MstPacket recvPacket;
SlvPacket sendPacket;
volatile bool packetReceived = false;
int status = 0;

// 受信コールバック（親機から送られた時刻を受け取る）
void onReceive(const uint8_t *mac, const uint8_t *data, int len) {
  memcpy(&recvPacket, data, sizeof(recvPacket));
  packetReceived = true;
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onReceive);

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, masterMac, 6);
  peer.channel = 1;
  peer.encrypt = false;
  esp_now_add_peer(&peer);

  Serial.printf("Slave %d ready\n", NODE_ID);
  sendPacket.nodeId = 1;
  packetReceived = false;
}

void send_msg(const char *msg) {
  strncpy(sendPacket.msg, msg, 63);
  sendPacket.msg[63] = '\0'; // 念のため
  sendPacket.statusId = status;
  esp_now_send(masterMac, (uint8_t *)&sendPacket, sizeof(sendPacket));
}

void loop() {
  if (!packetReceived) {
    return;
  }
  packetReceived = false;

  switch (status){
    case 0:
        if (recvPacket.commandId != 0) {
            send_msg("start command 0");
            status = 1;
        } else {
            send_msg("waiting....");
            break;
        }
    case 1:
        delay(recvPacket.commandId * 1000);
        send_msg("end command");
        status = 0;
        recvPacket.commandId = 0;
        break;
    default:
        send_msg("unknown status");
        status = 0;
  }
}