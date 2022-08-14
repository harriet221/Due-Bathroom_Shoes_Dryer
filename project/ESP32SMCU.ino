// NodeMCU - 온습도 측정부
// ESP32SMCU – 온습도 모듈 연결, Thingspeak 데이터 업로드, HC-06 연결
#include <WiFi.h> // 와이파이
#include <DHT.h> // 온습도 모듈
#include "ThingSpeak.h" // ThingSpeak
#include <HardwareSerial.h> // HC-06

#define DHTPIN 13 // 온습도 모듈 핀 정의
#define DHTTYPE DHT11 // 온습도 모듈 타입 정의

HardwareSerial mySerial(2); // ESP32S(NodeMCU)는 SoftwareSerial을 지원하지 않는다. 하드웨어 시리얼 Serial2 사용.

const char* ssid = "Q202_A"; // 와이파이 이름
const char* password = "22901409"; // 와이파이 비밀번호
DHT DHTsensor(DHTPIN, DHTTYPE); // 온습도 모듈 정의

WiFiServer server(80); // 웹 기본 서버
WiFiClient client; // ESP32를 클라이언트로 설정

unsigned long channelNumber = 1751402; // ThingSpeak 채널 아이디
const char* writeAPIKey = "L30GGCB63NHC6T0U"; // ThingSpeak 채널 접속을 위한 API 키

void setup() {
  ThingSpeak.begin(client); // ThingSpeak 클라이언트 연결
  DHTsensor.begin();      // 온습도 센서 연결
  Serial.begin(115200);   // 시리얼 통신 연결 with PC
  mySerial.begin(9600, SERIAL_8N1, 16, 17); // 하드웨어 시리얼 통신 연결 with HC-06
  WiFi.begin(ssid, password); // 와이파이 연결
  
  while(WiFi.status() != WL_CONNECTED) { // 와이파이 연결 확인
    delay(500);
    Serial.print(".");
  }
}

void loop() {
  float temp = DHTsensor.readTemperature(); // 온도 측정
  float humidity = DHTsensor.readHumidity(); // 습도 측정

  // thingspeak에 데이터 값 전달
  ThingSpeak.setField(1, temp);
  ThingSpeak.setField(2, humidity);
  ThingSpeak.writeFields(channelNumber, writeAPIKey);
  
  if( humidity > 70 ){ // 습도가 너무 높거나
    mySerial.write(1);
  }
  else if( (humidity >= 60) && ((temp <= 30) && (temp >= 20)) ){ // 온습도가 곰팡이 번식에 적합하다면
    mySerial.print(1); // 블루투스로 앱에 신호보내기
  }
  delay(600000); // 10min 간격
}
