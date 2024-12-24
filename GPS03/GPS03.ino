#include <ESP8266WiFi.h>  // ESP8266 WiFi 라이브러리 추가
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

const int RXPin = D3, TXPin = -1;
const int GPSBaud = 9600;

TinyGPSPlus gps;
SoftwareSerial gpsSerial(RXPin, TXPin);

WiFiServer server(80);
WiFiClient client; // 변수 선언

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(GPSBaud);

  // WiFi 연결 설정
  WiFi.begin("DIT_FREE_WiFi", "");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  server.begin();
}

void loop() {
  // Watchdog Timer 리셋
  ESP.wdtFeed();

  // charsProcessed() - 개체가 수신한 총 문자 수
  // 5초가 지났거나 수신된 문자가 적으면 기다린다.
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println(F("GPS가 감지되지 않았습니다."));
    Serial.println();
    while (true);
  }

  // WiFiClient 변수 초기화
  client = server.available();

  if (client) {
    while (client.connected()) {
      if (client.available()) {
        String request = client.readStringUntil('\r');
        client.flush();

        String response = "<!DOCTYPE HTML><html><head>";
        response += "<meta charset='UTF-8'>";
        response += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
        response += "<style>body{font-family:Arial;margin:20px;} .gps{padding:10px;margin:10px 0;border-radius:5px;}</style>";
        response += "</head><body>";
        response += "<h2>GPS Station</h2>";

        if (gps.location.isValid()) {
          response += "<div class='gps' style='background-color:pink'>위도: ";
          response += String(gps.location.lat(), 6);
          response += "</div>";
          response += "<div class='gps' style='background-color:lightblue'>경도: ";
          response += String(gps.location.lng(), 6);
          response += "</div>";
        } else {
          response += "<div class='data' style='background-color:lightgray'>GPS 신호 수신 대기중...</div>";
        }
        response += "</body></html>";
        client.println(response);
      }
    }
    client.stop();
  }

  delay(500);

  // Watchdog Timer 리셋
  ESP.wdtFeed();
}
