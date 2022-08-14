// 아두이노 우노 마스터 - 욕실화 건조부
#include <SoftwareSerial.h>
// 소프트웨어시리얼 통신 (UART)
SoftwareSerial s(2,3);      // 2:RX, 3:TX

// boolean 
// 1: true / 0: false
                                  // 현재 받침대의 상태 확인
boolean status_down = false;      // 현재 평평한 상태 여부
boolean status_up = false;        // 현재 높아진 상태 여부

boolean light_on_flag = false;      // 천장등 켜져 있음 여부 (1: true, 0: false)
boolean down_flag = false;          // 낮추기 동작 실행

boolean slippers_on_flag = false;   // 신발 올려져 있음 여부
boolean up_flag = false;            // 높이기 동작 실행


// 조도 센서 A 입력값 (천장등)
int lightBr = 0;
int lightBr_new = 0;

// 조도 센서 B 입력값 (받침대)
int lightOr = 0;
int lightOr_new = 0;



void setup() {
  Serial.begin(9600);       // PC와 시리얼 통신
  s.begin(9600);            // 아두이노 슬레이브와 시리얼 통신
}

void loop() {

  // 처음(0초) 조도 측정
  lightOr = analogRead(A1);
  lightBr = analogRead(A0);
  
  delay(3000);    // 3초마다 상태 변화 여부를 확인

  // 욕실등이 켜지면 사람이 들어올 것을 예상하고 높이를 낮춤 (신호 1 전송)
                                          // if
  if(lightBr < 40){                       // 원래 천장등이 꺼져 있다가
    light_on_flag = false;
    lightBr_new = analogRead(A0);         // (상태 변화 확인)   
    if(lightBr_new >= 40){                // 천장등이 켜지는 경우
        down_flag = true;                 // 낮추기 동작 실행(true)할 것을 알림
        light_on_flag = true;
    }                                     // else
  }else{                                  // 원래 천장등이 켜져 있는 경우
    light_on_flag = true;
    down_flag = false;
  }
  
  //시리얼 모니터로 천장등 조도 센서 값과 천장등 켜짐 여부, 낮추기 동작 실행 여부 확인
  Serial.print("천장등 조도 센서 1: "); Serial.print(lightBr);
  Serial.print("  2: "); Serial.println(lightBr_new);
  Serial.println("");
  Serial.print("light_on_flag: "); Serial.println(light_on_flag);
  Serial.print("down_flag: "); Serial.println(down_flag);
  Serial.println("");

  // 낮추기 동작 실행
  // 슬레이브 아두이노로 신호 1을 전송해 스텝 모터를 움직임
  if(light_on_flag && down_flag&&!status_down){
    s.write(1);
    status_down = true;               // 낮춰진 상태임을 알림
    status_up = false;                // 높여진 상태가 아님을 알림
    down_flag = false;                // 낮추기 동작 실행(false)하지 않을 것을 알림
    delay(1000);
  }else{
      s.write((byte)0);               // 낮추기 동작 실행하지 않는 경우 신호 0 전송
  }
  
  // 받침대에 신발이 올려진 경우 높이를 높임 (신호 2 전송)
                                           // if
  if(lightOr > 70){                        // 원래 신발이 올려져 있지 않다가
    slippers_on_flag = false;
    lightOr_new = analogRead(A1);
    
    if(lightOr - lightOr_new >= 35){       // 신발이 올려진 경우
        up_flag = true;
        slippers_on_flag = true; 
    }                                      // else
  }else{                                   // 원래 신발이 올려져 있는 경우
    slippers_on_flag = true;
    up_flag = false;
  }
  
  // 시리얼 모니터로 받침대 아래 조도 센서 값과 신발 올려짐 여부, 높이기 동작 실행 여부 확인
  Serial.print("받침대 아래 조도 센서 1: "); Serial.print(lightOr);
  Serial.print("  2: "); Serial.println(lightOr_new);

  Serial.print("slippers_on_flag: ");Serial.println(slippers_on_flag);
  Serial.print("up_flag: "); Serial.println(up_flag);
  Serial.println("");

  // 높이기 동작 실행
  // 슬레이브 아두이노로 신호 2를 전송해 스텝 모터를 움직임  
  if(slippers_on_flag && up_flag && !status_up && light_on_flag){  
    // 천장등이 꺼진 것을 신발이 올라온 것으로 감지하고 높이를 높이는 것을 방지하기 위해 불이 켜져 있어야 함을 조건으로 추가
    delay(2000);                      // 신발이 조도 센서가 있는 쪽에 한 짝만 올라왔을 것을 고려해 다른 쪽 신발을 올릴 때까지 기다림    
    s.write(2);
    status_up = true;               // 높여진 상태임을 알림
    status_down = false;            // 낮춰진 상태가 아님을 알림
    up_flag = false;                // 높이기 동작 실행(false)하지 않을 것을 알림
    delay(1000);
  }else{
      s.write((byte)0);             // 높이기 동작 실행하지 않는 경우 신호 0 전송
  }

  //시리얼 모니터로 현재 높이 상태 확인
  Serial.print("status_up: ");
  Serial.println(status_up);
  Serial.print("status_down: ");
  Serial.println(status_down);
}
