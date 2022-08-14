// 아두이노 우노 슬레이브
#include <Stepper.h>
#include <SoftwareSerial.h>

// 소프트웨어시리얼 통신 (UART)
SoftwareSerial s(2,3);      //2:RX, 3:TX

// 스텝 모터
const int stepsPerRevolution = 2048;           
Stepper myStepper(stepsPerRevolution, 4,6,5,7);  // IN 1, 3, 2, 4 -> 4,5,6,7

// 통신 신호 받을 것
int received_data = 0;

void setup() {
  Serial.begin(9600);       // PC와 시리얼 통신
  s.begin(9600);            // 아두이노 마스터와 시리얼 통신
  myStepper.setSpeed(14);   // 스텝 모터 속도 설정
}
 
void loop() {
 
  if(s.available()){                       // 버퍼에 받아진 신호 있을 때
    int received_data = s.read();         
    if(received_data == 1){                // 신호값이 1인 경우 (욕실등이 켜진 상황)
      myStepper.step(+stepsPerRevolution); // 낮추기(반시계방향회전, 빨강 감김)
      delay(1);
      myStepper.step(+stepsPerRevolution);
    }else if(received_data == 2){          // 신호값이 2인 경우 (신발이 올려진 상황)
      myStepper.step(-stepsPerRevolution); // 높이기(시계방향회전, 파랑 감김)
      delay(1);
      myStepper.step(-stepsPerRevolution);
    }else{                                 // 신호값이 0인 경우
                                           // 움직임 없음   
    }
  }
}
