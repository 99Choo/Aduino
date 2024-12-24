#include <DHT.h> // DHT 센서 라이브러리 포함
#include <UniversalTelegramBot.h> // UniversalTelegramBot 라이브러리 포함
#include <WiFiClientSecure.h> // WiFiClientSecure 라이브러리 포함
#include <ESP8266WiFi.h> // ESP8266WiFi 라이브러리 포함

const char *ssid = "DIT_FREE_WiFi"; // Wi-Fi SSID 설정
const char *password = ""; // Wi-Fi 비밀번호 설정
// nonblocking 방식의 지연을 위한 변수
unsigned long prevMillis = 0; // 이전 시간 저장 변수
const int interval =10000; // 지연 간격 설정

#define BOT_TOKEN "7106601271:AAEtKGTeJM4LpN6PZExi3IAAP2-5MGVEWqg" // Telegram Bot 토큰
#define CHAT_ID "7336046897" // Telegram Chat ID

int dhtPin = D1; // DHT 센서가 연결된 핀 번호
// 텔레그램 API의 인증서
X509List cert(TELEGRAM_CERTIFICATE_ROOT); // Telegram 인증서 설정
// 안전한 Wi-Fi 클라이언트 생성
DHT dht(dhtPin, DHT11); // DHT11 센서 객체 생성
WiFiClientSecure secured_client; // 보안 Wi-Fi 클라이언트 객체 생성
// Telegram 봇 객체 생성
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

void setup() {
  Serial.begin(115200); // 시리얼 통신 시작

  configTime(0,0,"pool.ntp.org"); // 시간 서버 설정

  secured_client.setTrustAnchors(&cert); // 인증서 설정

  WiFi.mode(WIFI_STA); // Wi-Fi 모드를 STA로 설정
  WiFi.begin(ssid, password); // Wi-Fi 연결 시작

  while (WiFi.status() != WL_CONNECTED) { // Wi-Fi 연결 완료 대기
    Serial.print("."); // 연결 중 상태 출력
    delay(500); // 0.5초 대기
  }
  Serial.println(""); // 줄 바꿈
  Serial.println("WiFi connected"); // Wi-Fi 연결 완료 출력
  Serial.print("IP :"); Serial.println(WiFi.localIP()); // IP 주소 출력
}

void loop() {
  float temp = dht.readTemperature(); // DHT 센서로부터 온도 읽기
  float humid = dht.readHumidity(); // DHT 센서로부터 습도 읽기
  Serial.print("temp : "); Serial.print(temp); // 온도 출력
  Serial.print("\xc2\xb0"); Serial.print("C\t"); // 온도 단위 출력
  Serial.print("humid : "); Serial.print(humid); Serial.println("%"); // 습도 출력
  
  String msg = "온도는 " + String(temp) + "\xc2\xb0" + "c 그리고 습도는 " + String(humid) + "%"; // 메시지 생성
  bot.sendMessage(CHAT_ID, msg, "Markdown"); // Telegram으로 메시지 전송

  delay(10000); // 10초 대기
}
