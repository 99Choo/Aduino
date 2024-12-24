#include <DHT.h>

// DHT11 설정

DHT dht(D5, DHT11); // DHT 객체 생성

#define SOIL_MOISTURE_PIN A0

void setup() {
  Serial.begin(115200);  // 시리얼 통신 초기화
  Serial.println("DHT11 센서 테스트 시작");
  dht.begin();         // DHT 센서 초기화
}

void loop() {
  // 온도와 습도 읽기
  float humidity = dht.readHumidity();    // 습도 값 읽기
  float temperature = dht.readTemperature(); // 온도 값 읽기 (섭씨 기준)

  // 토양 수분 센서 값 읽기
  int soilMoistureValue = analogRead(SOIL_MOISTURE_PIN);

  // 시리얼 모니터에 출력
  Serial.println("============");
  Serial.print("습도: ");
  Serial.print(humidity);
  Serial.print(" %\t");

  Serial.print("온도: ");
  Serial.print(temperature);
  Serial.println(" °C");
  Serial.println("============");

  // 토양 수분 데이터 출력
  Serial.print("토양 수분 값: ");
  Serial.println(soilMoistureValue);
  Serial.println("0~300: 젖음 (습한 상태) \n"
               "300~700: 적당 (중간 상태) \n"
               "700~1023: 건조 (마른 상태)");

  delay(2000); // 2초 대기 (센서 데이터 갱신 시간 고려)
}
