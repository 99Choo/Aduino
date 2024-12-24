#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <WiFiClient.h>

// Wi-Fi 설정
const char* ssid = "DIT_FREE_WiFi";        // Wi-Fi 이름
const char* password = ""; // Wi-Fi 비밀번호

// Thingspeak 설정
const char* server = "api.thingspeak.com";
const char* apiKey = "YOUR_API_KEY";   // Thingspeak Write API Key

// MQTT 설정
const char* mqttServer = "broker.hivemq.com"; // MQTT 브로커 주소 (HiveMQ 무료 브로커)
const int mqttPort = 1883;                   // MQTT 포트
const char* mqttTopic = "iot/sensor/alerts"; // MQTT 주제

WiFiClient espClient;
PubSubClient mqttClient(espClient);

// 핀 정의
#define DHTPIN D1          // DHT11 데이터 핀
#define DHTTYPE DHT11      // DHT 타입
DHT dht(DHTPIN, DHTTYPE);

#define SOIL_MOISTURE_PIN A0 // 토양습도 센서 핀

void setup() {
  Serial.begin(115200);
  dht.begin();
  pinMode(SOIL_MOISTURE_PIN, INPUT);

  // Wi-Fi 연결
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // MQTT 브로커 연결
  mqttClient.setServer(mqttServer, mqttPort);
  connectToMQTT();
}

void loop() {
  // 센서 데이터 읽기
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);

  // Thingspeak 데이터 전송
  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(temperature);
    postStr += "&field2=";
    postStr += String(humidity);
    postStr += "&field3=";
    postStr += String(soilMoisture);
    postStr += "\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.println("Data sent to Thingspeak");
    client.stop();
  }

  // 특정 조건에서 MQTT 알림 발송
  if (soilMoisture < 500) { // 토양 습도가 낮을 경우 알림
    String alertMessage = "ALERT: Soil moisture is low: " + String(soilMoisture);
    sendMQTTMessage(alertMessage);
  }

  delay(20000); // Thingspeak는 15초 이상 딜레이 필요
}

void connectToMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Connecting to MQTT...");
    if (mqttClient.connect("NodeMCUClient")) { // 클라이언트 ID
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
}

void sendMQTTMessage(String message) {
  if (!mqttClient.connected()) {
    connectToMQTT();
  }
  mqttClient.publish(mqttTopic, message.c_str());
  Serial.println("MQTT message sent: " + message);
}
