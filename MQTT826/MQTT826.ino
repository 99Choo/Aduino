/* test.mosquitto.org 브로커에게 "hello" 메시지 보내기
 * 구현 기능
 * - setup()
 * - reconnect() :wifi가 끝어질 때 재 연결 시도
 * - loop()
 */
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

//와이파이 연결 정보
const char *ssid = "DIT_FREE_WiFi";
const char *password = "";

//mqtt broker 설정
const char *mqtt_server = "test.mosquitto.org";
const int mqtt_port = 1883;

//topic
const char* topic = "csdit/moka";

//WiFi를 바탕으로 MQTT 클라이언트 생성, 초기화
WiFiClient esp8266Client;
PubSubClient client(esp8266Client);//mqtt client

//non-blocking delay를 위한 변수
unsigned long lastMsgTime = 0;
unsigned long delayTime = 3000;
int value = 0;//일련번호

//----------------------------------------------
void setup(){
  Serial.begin(115200);

  Serial.print("Connecting to ... ");
  Serial.println(ssid);

  //와이파이 연결
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }
  Serial.print("Connected!! IP : ");
  Serial.println(WiFi.localIP());

  //MQTT 서버와 포트 설정
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(onMsgReceived);
}

void onMsgReceived(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.print("수신된 메시지: ");
  Serial.println(msg);
}
//----------------------------------------------
//MQTT 연결의 상태를 확인하고 다시 연결을 시도
void reconnect() {
  // MQTT 서버에 연결
  while (!client.connected()) {
    Serial.print("Trying MQTT connection...");
    // MQTT 연결 시도
    if (client.connect("DIT-Publisher")) {
      Serial.println("MQTT connected");
      //토픽 구독
      client.subscribe(topic);
    } else {
      Serial.print("connecting failed");
      Serial.println(" 3초 후 재시도");
      delay(3000);
    }
  }
}
//----------------------------------------------
void loop() {
  //MQTT broker와연결되어 있지 않다면 reconnect()함수 호출
  if(!client.connected()){
    reconnect();
  }
  //MQTT broker와연결되었을 때 들어오는 메시지를 확인하고, 연결 상태를 유지하며, 
  //연결이 끊어진 경우 자동으로 재연결을 수행하는데 사용
  client.loop();

  //non-blocking방식으로 delayTime 마다 센서에서 데이터 읽어내기
  //delay()를 사용하면 모든 동작이 지연되기 때문에 사용할 수 없음
  //서버와 연결을 계속 유지해야 하기 위해 non-blocking 방식 사용
  long now = millis();
  if(now - lastMsgTime > delayTime){
    lastMsgTime = now;
    char msg[10];  

    //MQTT broker에 데이터 전송
    ++value;  //일련번호
    //snprintf() : 지정된 형식에 따라 문자열을 생성하는 함수
    //snprintf(결과문자열저장버퍼, 버퍼의 크기(널문자포함), 
    //형식문자열(%ld는 long 정수 위한 형식 지정자), 지정된 장소에 넣을 값)
    snprintf(msg, 10, "hello#%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client.publish(topic, msg); //메시지 발행 -> 브로커에게 전달
  }
}