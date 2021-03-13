#include <Servo.h>
Servo myServo;
#define trig 8 //8번 핀을 trig로 전역변수 설정
#define echo 9 //9번 핀을 echo로 전역변수 설정 
#define spin 7 //7번 핀을 servo용 핀 전역변수 설정

void setup()
{

  myServo.attach(spin);
  Serial.begin(9600); //serial 통신 9600레이트로 시작
  pinMode(trig, OUTPUT); // Trigger 초음파 발생 아웃풋
  pinMode(echo, INPUT); // Echo 초음파 수신부 인풋

}

void loop()
{
  myServo.write(60); // 서보 초기 각도 설정 60도

  long duration, distance; // 변수 설정

  digitalWrite(trig, HIGH); // 초음파 발생
  delayMicroseconds(10);

  digitalWrite(trig, LOW); // 초음파 끄기
  delayMicroseconds(10);

  duration = pulseIn(echo, HIGH); // echo에 특정 신호 될때까지 시간 측정 함수 입력
  distance = duration * 170 / 1000; //mm로 변환

  Serial.print("거리:"); //시리얼 모니터에 거리 확인을 위한 mm 출력
  Serial.print(distance);
  Serial.println("mm");

  if (distance < 80) // 손이 밑에 들어가야하므로 80mm이하가 되면 서보모터가 돌아가도록 설정
  {
    myServo.write(0); //0도로 변화
    delay(1500); // 1.5초간 유지 후
    myServo.write(61); // 원상태 복귀
  }
  else
  {
    myServo.write(61); // 80mm 이상이 측정되면 그냥 평소각도대로
  }
  delay(1000); // 전체 루프 과정 1초마다 새로 하도록 딜레이 설정
}
