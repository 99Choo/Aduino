// #include <DHT.h>
#include <UniversalTelegramBot.h>
#include <WiFiClientSecure.h>
#include <ESP8266WiFi.h>

const char *ssid = "DIT_FREE_WiFi"; // Wi-Fi SSID 설정
const char *password = ""; // Wi-Fi 비밀번호 설정
unsigned long prevMillis = 0; // 이전 시간 저장 변수
const int interval = 10000; // 지연 간격 설정

#define BOT_TOKEN "7106601271:AAEtKGTeJM4LpN6PZExi3IAAP2-5MGVEWqg" // Telegram Bot 토큰
#define CHAT_ID "7336046897" // Telegram Chat ID
int ledPin = D1;
// int dhtPin = D1; // DHT 센서가 연결된 핀 번호
X509List cert(TELEGRAM_CERTIFICATE_ROOT); // Telegram 인증서 설정
// DHT dht(dhtPin, DHT11); // DHT11 센서 객체 생성
WiFiClientSecure secured_client; // 보안 Wi-Fi 클라이언트 객체 생성
UniversalTelegramBot bot(BOT_TOKEN, secured_client); // Telegram 봇 객체 생성

void setup() {
  Serial.begin(115200); // 시리얼 통신 시작
  pinMode(D1, OUTPUT);
  configTime(0, 0, "pool.ntp.org"); // 시간 서버 설정
  secured_client.setTrustAnchors(&cert); // 인증서 설정
  WiFi.mode(WIFI_STA); // Wi-Fi 모드를 STA로 설정
  WiFi.begin(ssid, password); // Wi-Fi 연결 시작
Q
  while (WiFi.status() != WL_CONNECTED) { // Wi-Fi 연결 완료 대기
    Serial.print("."); // 연결 중 상태 출력
    delay(500); // 0.5초 대기
  }
  Serial.println(""); // 줄 바꿈
  Serial.println("WiFi connected"); // Wi-Fi 연결 완료 출력
  Serial.print("IP :"); Serial.println(WiFi.localIP()); // IP 주소 출력
}

void loop() {
  int numNdsMsg = bot.getUpdates(bot.last_message_received +1 );

  for(int i=0; i<numNdsMsg; i++){
    String chat_id = bot.messages[i].chat_id;
    String text = bot.messages[i].text;
    Serial.println(text);
    Serial.println(chat_id);

    if(text == "on") {
      digitalWrite(ledPin, HIGH);
      bot.sendMessage(chat_id, "LED turned ON");
     }
    else if(text == "off") {
      digitalWrite(ledPin, LOW);
      bot.sendMessage(chat_id, "LED turned OFF");
    }
    else{
      bot.sendMessage(chat_id, "wrong command");
    }
  }
}
