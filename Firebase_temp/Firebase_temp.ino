#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <DHT.h>

#define FIREBASE_HOST "https://ch-iot-2ba08-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyCFO5RT47tAsyJj65o7e0ptYLBlwJAVWyQ"
#define WIFI_SSID "DIT_FREE_WiFi"
#define WIFI_PASSWORD ""

// DHT 센서 설정
#define DHTPIN D1
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// 파이어베이스 데이터 객체 생성
FirebaseData fbdo; // firebase 데이터 객체 생성
FirebaseAuth auth; // firebase 인증 객체 생성
FirebaseConfig config; // firebase 설정 객체 생성

int n = 0; // 전역 변수 선언

void setup() {
    Serial.begin(115200);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());

    // 파이어베이스 연결
    Firebase.reconnectWiFi(true); // firebase에 재접속 용
    config.host = FIREBASE_HOST; // firebase 서버 설정
    config.signer.tokens.legacy_token = FIREBASE_AUTH; // firebase 인증 설정
    Firebase.begin(&config, &auth);

    Serial.println("DHT11 test!");
    dht.begin();
} // setup 함수 닫는 중괄호 추가

void loop() {
    // 온습도 데이터를 DHT11에서 측정하기
    float humid = dht.readHumidity();
    float temp = dht.readTemperature();

    // 둘 다 측정된 데이터가 없을 경우 에러 출력
    if (isnan(humid) || isnan(temp)) {
        Serial.println("Error");
        return;
    }

    n++; // 입력되는 데이터의 갯수를 파악하기 위한 카운터
    Firebase.pushInt(fbdo, "/nodeMCU/num", n);
    Firebase.pushFloat(fbdo, "/nodeMCU/humidity", humid);
    Firebase.pushFloat(fbdo, "/nodeMCU/temperature", temp);

    delay(60000); // 1분에 한 번씩 측정 데이터 전달

    // if (Serial.available()) {
    //     // 시리얼모니터에 line ending 없음을 선택한 뒤 실행할 것
    //     String greeting = Serial.readStringUntil('\r');
    //     // 시리얼모니터에 입력한 내용을 파이어베이스 데이터베이스에 입력하기
    //     bool res = Firebase.setString(fbdo, "/GREETING", greeting);

    //     // 데이터베이스에 입력이 성공했으면 시리얼 모니터에 "OK"를 아니면 해당 에러를 출력하기
    //     if (res) {
    //         Serial.println("Set string...ok");
    //     } else {
    //         Serial.println(fbdo.errorReason().c_str());
    //     }
    // }
}
