#include <TinyGPS++.h>
#include <SoftwareSerial.h>

const int RxPin = D3;  // RX 핀
const int TXPin = -1;  // TX 핀 (사용할 핀 번호로 변경)

TinyGPSPlus gps;
SoftwareSerial gpsSerial(RxPin, TXPin);

void setup() {
  Serial.begin(115200);

  gpsSerial.begin(9600);

  Serial.println("GPS 모듈 테스트...");
}

void loop() {
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("GPS가 감지되지 않았습니다.");
    Serial.println();
    while (true);
  }
  
  while (gpsSerial.available() > 0) {
    if (gps.encode(gpsSerial.read())) {
      Serial.print(F("위치: = "));
      float loc = gps.location.lat();
      Serial.print(loc, 6);
      Serial.print(F(", "));
      Serial.print(F("경도 = "));
      float lng = gps.location.lng();
      Serial.println(lng, 6);
    } else {
      Serial.println(F("위치 수신 실패"));
    }
  }
  delay(50);  // 여기로 이동
}
