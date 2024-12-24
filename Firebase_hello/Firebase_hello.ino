#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

#define FIREBASE_HOST "ch-iot-2ba08-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyCFO5RT47tAsyJj65o7e0ptYLBlwJAVWyQ"

#define WIFI_SSID "DIT_FREE_WiFi"
#define WIFI_PASSWORD ""

// 파이어베이스 데이터 객체 생성
FirebaseData fbdo; //1. firebase 데이터 객체 생성
FirebaseAuth auth; //2. firebase 인증 객체 생성
FirebaseConfig config; //3. firebase 설정 객체 생성

int led = D2; // LED 핀 설정
String read_data = ""; // 읽어온 데이터를 저장할 변수

void setup() {
    pinMode(led, OUTPUT); // LED 핀을 출력 모드로 설정
    Serial.begin(115200); // 시리얼 통신 시작
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // WiFi 연결 시작
    while (WiFi.status() != WL_CONNECTED) { // WiFi 연결 대기
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    // 파이어베이스 연결
    config.host = FIREBASE_HOST; //4. firebase 서버 설정
    config.signer.tokens.legacy_token = FIREBASE_AUTH; //5. firebase 인증 설정
    Firebase.begin(&config, &auth); // firebase 시작
}

void loop() {
    if (Firebase.getString(fbdo, "LED_STATUS")) { // LED_STATUS 값을 가져오기
        read_data = fbdo.stringData(); // 데이터를 문자열로 저장
        Serial.print("Read_data = ");
        Serial.println(read_data);

        if (read_data == "ON") {
            digitalWrite(led, HIGH); // LED를 켬
            Serial.println("LED turned ON");
        } else if (read_data == "OFF") {
            digitalWrite(led, LOW); // LED를 끔
            Serial.println("LED turned OFF");
        }
        Serial.println();
    } else {
        Serial.println(fbdo.errorReason()); // 오류가 있을 경우 출력
    }
}
