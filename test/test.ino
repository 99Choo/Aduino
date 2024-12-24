#include <DHT.h>                  // DHT 센서를 사용하기 위한 라이브러리
#include <ESP8266WiFi.h>          // Wi-Fi 기능을 사용하기 위한 라이브러리
#include <WiFiClientSecure.h>     // HTTPS 통신을 위한 라이브러리
#include <UniversalTelegramBot.h> // 텔레그램 봇 라이브러리

// 핀 정의
#define DHTPIN D1                 // DHT11 센서 데이터 핀
#define DHTTYPE DHT11             // DHT 센서의 유형 (DHT11)
DHT dht(DHTPIN, DHTTYPE);         // DHT 객체 생성

#define SOIL_MOISTURE_PIN A0      // 토양습도 센서 핀
#define WATER_PUMP_PIN D3         // 워터펌프 1 핀
#define WATER_PUMP_PIN2 D4        // 워터펌프 2 핀
#define LED_R_PIN D6              // 빨간색 LED 핀
#define LED_G_PIN D7              // 초록색 LED 핀
#define LED_B_PIN D8              // 파란색 LED 핀
#define SWITCH_BUTTON_PIN D2      // 스위치 버튼 핀

// 텔레그램 봇 정보
const char* telegramBotToken = "7106601271:AAEtKGTeJM4LpN6PZExi3IAAP2-5MGVEWqg"; // 텔레그램 봇의 토큰
const char* chatID = "7336046897"; // 텔레그램에서 메시지를 보낼 Chat ID

WiFiClientSecure telegramClient; // HTTPS 클라이언트 생성
UniversalTelegramBot bot(telegramBotToken, telegramClient); // 텔레그램 봇 객체 생성

// Wi-Fi 정보
const char* ssid = "DIT_FREE_WiFi"; // Wi-Fi SSID
const char* password = "";          // Wi-Fi 비밀번호

// 변수 선언
int soilMoistureValue = 0;          // 토양습도 값을 저장할 변수
float temperature = 0.0;            // 온도를 저장할 변수
float humidity = 0.0;               // 습도를 저장할 변수

// 초기 설정
void setup() {
  Serial.begin(115200);             // 시리얼 모니터 통신 속도 설정
  Serial.println("Smart Plant Monitoring System"); // 시스템 시작 메시지 출력

  dht.begin();                      // DHT 센서 초기화
  pinMode(SOIL_MOISTURE_PIN, INPUT); // 토양습도 센서를 입력 모드로 설정
  pinMode(WATER_PUMP_PIN, OUTPUT);  // 워터펌프 1 핀을 출력 모드로 설정
  pinMode(WATER_PUMP_PIN2, OUTPUT); // 워터펌프 2 핀을 출력 모드로 설정
  pinMode(LED_R_PIN, OUTPUT);       // 빨간색 LED를 출력 모드로 설정
  pinMode(LED_G_PIN, OUTPUT);       // 초록색 LED를 출력 모드로 설정
  pinMode(LED_B_PIN, OUTPUT);       // 파란색 LED를 출력 모드로 설정
  pinMode(SWITCH_BUTTON_PIN, INPUT_PULLUP); // 스위치 버튼을 풀업 모드로 설정

  Serial.print("Connecting to Wi-Fi"); // Wi-Fi 연결 상태를 출력
  WiFi.begin(ssid, password);          // Wi-Fi 네트워크에 연결
  while (WiFi.status() != WL_CONNECTED) { // Wi-Fi 연결 대기
    delay(500); // 0.5초 대기
    Serial.print("."); // 연결 대기 중 상태를 출력
  }
  Serial.println("\nConnected to Wi-Fi"); // Wi-Fi 연결 완료 메시지 출력

  telegramClient.setInsecure(); // HTTPS 인증 비활성화 (SSL 인증 사용하지 않음)
}

// 메인 루프
void loop() {
  temperature = dht.readTemperature(); // DHT 센서에서 온도 읽기
  humidity = dht.readHumidity();       // DHT 센서에서 습도 읽기

  if (isnan(temperature) || isnan(humidity)) { // 값이 유효하지 않은 경우 확인
    Serial.println("Failed to read from DHT sensor!"); // 오류 메시지 출력
    return; // 함수 종료
  }

  soilMoistureValue = analogRead(SOIL_MOISTURE_PIN); // 토양습도 센서 값 읽기

  // 시리얼 모니터에 읽은 값 출력
  Serial.println("================================="); // 구분선 출력
  Serial.print("Temperature: "); // 온도 출력
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: "); // 습도 출력
  Serial.print(humidity);
  Serial.println(" %");

  Serial.print("Soil Moisture: "); // 토양습도 값 출력
  Serial.println(soilMoistureValue);

  // LED 상태 설정
  if (soilMoistureValue > 1000) { // 건조 상태 확인
    digitalWrite(LED_R_PIN, HIGH); // 빨간색 LED 켜기
    digitalWrite(LED_G_PIN, LOW);  // 초록색 LED 끄기
    digitalWrite(LED_B_PIN, LOW);  // 파란색 LED 끄기
    Serial.println("LED Status: RED (DRY)"); // 상태 메시지 출력
  } else if (soilMoistureValue > 600) { // 적당한 습도 상태 확인
    digitalWrite(LED_R_PIN, LOW);  // 빨간색 LED 끄기
    digitalWrite(LED_G_PIN, HIGH); // 초록색 LED 켜기
    digitalWrite(LED_B_PIN, LOW);  // 파란색 LED 끄기
    Serial.println("LED Status: GREEN (NORMAL)"); // 상태 메시지 출력
  } else { // 매우 습한 상태 확인
    digitalWrite(LED_R_PIN, LOW);  // 빨간색 LED 끄기
    digitalWrite(LED_G_PIN, LOW);  // 초록색 LED 끄기
    digitalWrite(LED_B_PIN, HIGH); // 파란색 LED 켜기
    Serial.println("LED Status: BLUE (WET)"); // 상태 메시지 출력
  }

  sendTelegramMessage(); // 텔레그램 메시지 전송
  delay(60000);          // 1분 대기
}

// 텔레그램 메시지 전송 함수
void sendTelegramMessage() {
  String message = "Smart Plant Monitoring:\n"; // 메시지 생성
  message += "Temperature: " + String(temperature) + "°C\n"; // 온도 추가
  message += "Humidity: " + String(humidity) + "%\n"; // 습도 추가
  message += "Soil Moisture: " + String(soilMoistureValue) + "\n"; // 토양습도 값 추가

  if (bot.sendMessage(chatID, message, "")) { // 텔레그램 메시지 전송
    Serial.println("Telegram message sent successfully."); // 성공 메시지 출력
  } else {
    Serial.println("Failed to send Telegram message."); // 실패 메시지 출력
  }
}
