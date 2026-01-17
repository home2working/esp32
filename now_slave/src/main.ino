#include <WiFi.h>
#include <esp_now.h>

#define NODE_ID 1   // 子機ごとに変更
uint8_t masterMac[] = {0x78, 0x42, 0x1C, 0x2D, 0xF4, 0x9C}; // ESP32-AのMAC

typedef struct {
  uint32_t Time;   //時刻（秒）
  uint8_t nodeId;      // 子機ID（1〜4用）
} TimePacket;

typedef struct {
  uint8_t nodeId;        // 子機ID（1台目なら1）
  char payload[64];      // 送信メッセージ
} MsgPacket;

MsgPacket recvPacket;
TimePacket sendTime;

// 受信コールバック（親機から送られた時刻を受け取る）
void onReceive(const uint8_t *mac, const uint8_t *data, int len) {
  memcpy(&recvPacket, data, sizeof(recvPacket));
  Serial.print("[FROM PARENT] ");
  Serial.println(recvPacket.payload);
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

  randomSeed(esp_random()); //複数子機の際の送信タイミングずらし。今回は1機なので省略可能

  Serial.printf("Slave %d ready\n", NODE_ID);
}

void loop() {
  // 1秒ごとに自分の時刻を親機に送信
  static unsigned long lastSend = 0;
  static uint32_t Time = 1700000000; // 初期値
  if (millis() - lastSend >= 1000) {
    lastSend = millis();
    Time++; // 1秒ごとに増加（簡易カウンタ）

    sendTime.Time = Time;
    sendTime.nodeId = NODE_ID;

    // 送信タイミングを少しランダムにして衝突回避
    delay(random(5, 20));

    esp_now_send(masterMac, (uint8_t *)&sendTime, sizeof(sendTime));

    Serial.print("Send my time to parent: ");
    Serial.println(Time);
  }
}