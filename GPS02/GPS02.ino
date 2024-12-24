#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>

const int RxPin = D3;  // RX 핀
const int TXPin = -1;  // TX 핀 (사용할 핀 번호로 변경)

TinyGPSPlus gps;
SoftwareSerial gpsSerial(RxPin, TXPin);

const char *ssid = "DIT_FREE_WiFi";
const char *password = "";

WiFiServer server(80);
WiFiClient client;

void setup() {
  Serial.begin(115200);

  gpsSerial.begin(9600);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi Connected");
  Serial.print("IP : "); Serial.println(WiFi.localIP());

  server.begin();
  Serial.println("HTTP Server started.");
  Serial.println("GPS 모듈 테스트...");
}

void loop() {
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    Serial.println("GPS가 감지되지 않았습니다.");
    Serial.println();
    delay(1000);
    return;
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

  client = server.available();
  if (client) {
    while (client.connected()) {
      if (client.available()) {
        String request = client.readStringUntil('\r');
        client.flush();

        String response = "<!DOCTYPE HTML><html><head>";
        response += "<meta charset='UTF-8'>";
        response += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
        response += "<style>body{font-family:Arial;margin:20px;} .gps{padding:10px;margin:10px 0; border-radius:5px;}</style>";
        response += "</head><body>";
        response += "<h2>GPS Station</h2>;";

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
  }
  delay(500);
}
