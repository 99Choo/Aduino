/****************************************
 * Include Libraries
 ****************************************/
#include <Ubidots.h>
#include <DHT.h>

/****************************************
 * Define Instances and Constants
 ****************************************/
const char* UBIDOTS_TOKEN = "BBUS-bpcvCrrMs0VKhXk7Qde2Wa7rjjZcyR";  // Ubidots 토큰
const char* WIFI_SSID = "DIT_FREE_WiFi";  // WiFi SSID
const char* WIFI_PASS = "";  // WiFi 비밀번호

Ubidots ubidots(UBIDOTS_TOKEN, UBI_HTTP);
DHT dht(D1, DHT11);  // DHT11 센서 초기화

void setup() {
  dht.begin();  // DHT 센서 초기화
  Serial.begin(115200);  // 시리얼 통신 초기화
  ubidots.wifiConnect(WIFI_SSID, WIFI_PASS);  // WiFi 연결
  // ubidots.setDebug(true);  // 디버그 메시지 출력을 원하면 이 줄의 주석을 해제
}

void loop() {
  float temperature = dht.readTemperature();  // 온도 읽기
  float humidity = dht.readHumidity();  // 습도 읽기

  ubidots.add("Temperature", temperature);  // Ubidots에 온도 데이터 추가
  ubidots.add("Humidity", humidity);  // Ubidots에 습도 데이터 추가

  bool bufferSent = false;
  bufferSent = ubidots.send();  // Ubidots에 데이터 전송

  if (bufferSent) {
    // 값이 정상적으로 전송된 경우
    Serial.println("Values sent by the device");
  }
  delay(20000);  // 20초 대기
}
