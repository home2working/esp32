#define PUL_PIN 25
#define DIR_PIN 26
#define ENA_PIN 27

const int PULSE_PER_REV = 1600;  // DIP設定と合わせる

void rotate(int rpm, int revolutions, bool dir) {
  digitalWrite(ENA_PIN, HIGH);      // 有効
  digitalWrite(DIR_PIN, dir);       // 方向
  delayMicroseconds(5);             // DIR→PUL待ち

  int freq = rpm * PULSE_PER_REV / 60;
  int delay_us = 1000000 / (freq * 2);

  int totalPulses = revolutions * PULSE_PER_REV;

  for (int i = 0; i < totalPulses; i++) {
    digitalWrite(PUL_PIN, HIGH);
    delayMicroseconds(delay_us);
    digitalWrite(PUL_PIN, LOW);
    delayMicroseconds(delay_us);
  }
}

void setup() {
  pinMode(PUL_PIN, OUTPUT);
  pinMode(DIR_PIN, OUTPUT);
  pinMode(ENA_PIN, OUTPUT);
}

void loop() {
  rotate(60, 2, true);   // 正転 60rpm 2回転
  delay(2000);

  rotate(120, 1, false); // 逆転 120rpm 1回転
  delay(3000);
}

