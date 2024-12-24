#include <ESP8266WiFi.h>  // 와이파이 라이브러리
#include <ESP8266HTTPClient.h>  // HTTPClient 라이브러리로 웹서버와 통신
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,16,2);

const char *ssid = "DIT_FREE_WiFi";  // WiFi SSID
const char *password = "";  // WiFi 비밀번호

WiFiClient client;  // WiFi 클라이언트 객체
HTTPClient http;  // HTTP 클라이언트 객체

// 요청할 URL
String url = "http://apis.data.go.kr/B552584/ArpltnStatsSvc/getCtprvnMesureLIst?serviceKey=tTWAbrMZTwmp%2FlwNXzsfmEXLtMkCVqzcjJT2t1NNFbj97yk7G0PzRDSLooOJZ6Hw7JDw6KKMekgF1ihRzJbKAg%3D%3D&returnType=xml&numOfRows=100&pageNo=1&itemCode=SO2&dataGubun=HOUR&searchCondition=MONTH";

void setup() {
  Serial.begin(115200);  // 시리얼 통신 초기화
  WiFi.begin(ssid, password);  // WiFi 연결 시작

  // WiFi 연결 완료 대기
  while(WiFi.status() != WL_CONNECTED) {
    Serial.print(".");  // 연결 상태 표시
    delay(500);  // 0.5초 대기
  }
  Serial.println("WiFi connected");  // 연결 완료 메시지
  Serial.print("IP : "); 
  Serial.println(WiFi.localIP());  // 연결된 IP 주소 출력

  lcd.init();
  lcd.backlight();
  
}

void loop() {
  Serial.println("loop");
  if (WiFi.status() == WL_CONNECTED) {  // WiFi 연결 상태 확인
    // HTTP 클라이언트 세션을 초기화하고 URL 설정
    http.begin(client, url);
    Serial.println("WiFi connected");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());

    // HTTP GET 요청 수행
    int httpCode = http.GET();
    Serial.println(httpCode);

    if (httpCode > 0) {
      // 응답 본문 가져오기
      String payload = http.getString();
      Serial.println(payload);

      // 첫 번째 <busan> 태그의 값을 추출
      int startIdx = payload.indexOf("<busan>") + String("<busan>").length();
      int endIdx = payload.indexOf("</busan>", startIdx);
      
      if (startIdx != -1 && endIdx != -1) {
        String so2Value = payload.substring(startIdx, endIdx);  // SO2 값 추출
        Serial.print("부산의 SO2 농도: ");
        Serial.println(so2Value);
        lcd.setCursor(0,0);
        lcd.print("busan SO2 : "); lcd.print(so2Value);
        delay(3000);
        lcd.clear();
      } else {
        Serial.println("SO2 데이터를 찾을 수 없습니다.");
      }
    } else {
      Serial.print("HTTP 요청 실패, 코드: ");
      Serial.println(httpCode);
    }

    // HTTP 연결 종료
    http.end();
  }
  Serial.println("딜레이 시작");
  delay(10000);  // 10초 대기

}
