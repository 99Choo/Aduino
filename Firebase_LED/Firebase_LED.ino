#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>

#define FIREBASE_HOST "https://ch-iot-2ba08-default-rtdb.firebaseio.com/"
#define FIREBASE_AUTH "AIzaSyCFO5RT47tAsyJj65o7e0ptYLBlwJAVWyQ"

#define WIFI_SSID "DIT_FREE_WiFi"
#define WIFI_PASSWORD ""

int ledPin = D2;
String read_data = "";

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("connecting to");
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("connected: ");
  Serial.println(WiFi.localIP());

  config.host = FIREBASE_HOST;
  config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);
  Firebase.setString(fbdo, "LED_STATUS", "OFF");
}

void loop() {
  bool res = Firebase.getString(fbdo, "LED_STATUS");
  read_data = fbdo.stringData();

  Serial.print("result : "); Serial.println(res);
  Serial.print("read_data : "); Serial.println(read_data);

  if(res){
    if(read_data == "ON"){
      Serial.println("Led Turned ON");
      digitalWrite(ledPin, HIGH);
    }else if(read_data == "OFF"){
      Serial.println("Led Turned OFF");
      digitalWrite(ledPin, LOW);
    }else {
      Serial.println(fbdo.errorReason());
    }
  }
  delay(2000);
}
