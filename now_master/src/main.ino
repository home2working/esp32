#include <WiFi.h>
#include <esp_now.h>

uint8_t slaveMac[] = {0x04,0x83,0x08,0x0E,0x4F,0xB4};

typedef struct {
  uint8_t nodeId;        // 子機ID（1台目なら1）
  char payload[64];      // 送信メッセージ
} MsgPacket;

typedef struct {
  uint32_t Time;   // 時刻（秒）
  uint8_t nodeId;      // 子機ID（1〜4用）
} TimePacket;

MsgPacket sendPacket;

uint32_t baseUnixTime = 0;
unsigned long baseMillis = 0;

// 受信コールバック（子機から送られてくる時刻を表示）
void onReceive(const uint8_t *mac, const uint8_t *data, int len) {
  TimePacket recvPacket;
  memcpy(&recvPacket, data, sizeof(recvPacket));

  Serial.print("[FROM NODE ");
  Serial.print(recvPacket.nodeId);
  Serial.print("] Time: ");
  Serial.println(recvPacket.unixTime);
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

  Serial.println("Master ready");
  Serial.println("Send: TIME <unix_time>");
}

void loop() {
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) return;

    line.toCharArray(sendPacket.payload, sizeof(sendPacket.payload));
    sendPacket.nodeId = 1; // 1台目の子機

    esp_now_send(slaveMac, (uint8_t *)&sendPacket, sizeof(sendPacket));

    Serial.print("Sent to slave: ");
    Serial.println(line);
  }
}
