#include <UniversalTelegramBot.h> // UniversalTelegramBot 라이브러리 포함
#include <WiFiClientSecure.h> // WiFiClientSecure 라이브러리 포함
#include <ESP8266WiFi.h> // ESP8266WiFi 라이브러리 포함

const char *ssid = "DIT_FREE_WiFi"; // Wi-Fi SSID 설정
const char *password = ""; // Wi-Fi 비밀번호 설정

#define BOT_TOKEN "7106601271:AAEtKGTeJM4LpN6PZExi3IAAP2-5MGVEWqg" // Telegram Bot 토큰
#define CHAT_ID "7336046897" // Telegram Chat ID

// X.509은 암호학에서 공개키 인증서와 인증알고리즘의 표준 가운에에서 공개 키 기반(PKI)의 ITU-T 표준
X509List cert(TELEGRAM_CERTIFICATE_ROOT); // Telegram 인증서 설정

WiFiClientSecure secured_client; // 보안 Wi-Fi 클라이언트 객체 생성
UniversalTelegramBot bot(BOT_TOKEN, secured_client); // Telegram 봇 객체 생성

void setup() {
  Serial.begin(115200); // 시리얼 통신 시작

  // 컴퓨터의 시간을 네트워크 시간과 맞추기 위한 설정
  configTime(0, 0, "pool.ntp.org");

  // Telegram의 API에 대한 루트 인증서 추가(api.telegram.org)
  secured_client.setTrustAnchors(&cert);

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
  // chat_id로 메시지를 보내고 메시지 전송 여부를 반환
  bot.sendMessage(CHAT_ID, "Hello, World!", ""); // 메시지 전송

  delay(10000); // 10초 대기
}
