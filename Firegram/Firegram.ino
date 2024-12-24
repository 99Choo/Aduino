#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <WiFiClientSecure.h>

// Wi-Fi 설정
const char* ssid = "your-SSID";
const char* password = "your-PASSWORD";

// Firebase 설정
#define FIREBASE_HOST "your-project.firebaseio.com"
#define FIREBASE_AUTH "your-firebase-database-secret"
FirebaseData firebaseData;

// Telegram 설정
const char* telegramBotToken = "YOUR_TELEGRAM_BOT_TOKEN";
const char* chatID = "YOUR_TELEGRAM_CHAT_ID";

WiFiClientSecure telegramClient;

void setup() {
  Serial.begin(115200);

  // Wi-Fi 연결
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  // Firebase 초기화
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);

  // Telegram SSL 설정
  telegramClient.setInsecure();
}

void loop() {
  // Firebase에서 특정 상태 감지
  if (Firebase.getString(firebaseData, "/waterPump")) {
    String pumpState = firebaseData.stringData();
    if (pumpState == "ON") {
      // Telegram 알림 전송
      sendTelegramMessage("Water Pump is ON! Starting 3-second operation.");
    }
  }

  delay(2000); // Firebase 업데이트 주기
}

void sendTelegramMessage(String message) {
  if (telegramClient.connect("api.telegram.org", 443)) {
    String url = String("/bot") + telegramBotToken + "/sendMessage?chat_id=" + chatID + "&text=" + message;
    telegramClient.print(String("GET ") + url + " HTTP/1.1\r\n" +
                         "Host: api.telegram.org\r\n" +
                         "Connection: close\r\n\r\n");
    while (telegramClient.connected()) {
      String line = telegramClient.readStringUntil('\n');
      if (line == "\r") break; // Header 끝
    }
    Serial.println("Telegram message sent: " + message);
  } else {
    Serial.println("Failed to connect to Telegram");
  }
}
