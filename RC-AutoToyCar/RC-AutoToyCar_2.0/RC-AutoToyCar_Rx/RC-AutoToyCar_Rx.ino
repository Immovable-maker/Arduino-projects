#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>

#define MOTOR_PIN1 A4
#define MOTOR_PIN2 A5
#define SERVO_PIN 6

#define CENTER_ECHO 2
#define CENTER_TRIG 3
#define LEFT_ECHO 9
#define LEFT_TRIG 10
#define RIGHT_ECHO 4
#define RIGHT_TRIG 5

int changeAngleTime = 150;
int initAngle = 65;
int runTime = 500;
int runSpeed = 128;

Servo servo;
RF24 radio(7, 8);
const byte address[6] = "00101";
uint8_t data[3] = {65, 122, 1};

void setup()
{

  pinMode(MOTOR_PIN1, OUTPUT);
  pinMode(MOTOR_PIN2, OUTPUT);
  pinMode(CENTER_ECHO, INPUT);
  pinMode(CENTER_TRIG, OUTPUT);
  pinMode(LEFT_ECHO, INPUT);
  pinMode(LEFT_TRIG, OUTPUT);
  pinMode(RIGHT_ECHO, INPUT);
  pinMode(RIGHT_TRIG, OUTPUT);

  Serial.begin(9600);
  radio.begin();
  servo.attach(SERVO_PIN);
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();

  servo.write(initAngle);
}

void loop()
{
  Serial.println(data[2]);
  radio.read(&data, sizeof(data)); //버튼이 눌리지 않았을때 data[2] = 1
  bool tempMode = true;
  if (data[2]) {
    tempMode = tempMode;
  } else {
    tempMode = !tempMode;
    carStop();
    delay(500); //0이 잠깐눌러서 두번 들어올수 있으니까
  }

  if (tempMode)
  {
    Serial.print("RC모드");
    // RC카 방향 및 속도 조절
    servo.write(data[0]);
    Serial.print("각도");
    Serial.print(data[0]);
    if (data[1] > 92 && data[1] < 152) //가운데 값이 122임
    {
      Serial.println("정지");
      carStop();
    }
    if (data[1] >= 152) //전진
    {
      uint8_t speedForw = data[1];
      Serial.print("전진");
      carStart(speedForw);
    }
    if (data[1] <= 92 && data[1] >= 0) //후진
    {
      uint8_t speedRev = map(data[1], 0, 121, 134, 255);
      Serial.print("후진");
      carReverse(speedRev);
    }

  } else
  {
    Serial.print("자율주행");
    long leftDistance = measureDistance(LEFT_ECHO, LEFT_TRIG);
    long centerDistance = measureDistance(CENTER_ECHO, CENTER_TRIG);
    long rightDistance = measureDistance(RIGHT_ECHO, RIGHT_TRIG);
    debugDistance(leftDistance, centerDistance, rightDistance);
    long distanceDiff = rightDistance - leftDistance;
    int angle = initAngle;

    if (centerDistance > 140)
    {
      if (distanceDiff < 50 && distanceDiff > -50)
      { //
        angle = map(distanceDiff, -50, 50, initAngle - 20, initAngle + 20);
        autoDrive(angle);
      }
      else if (distanceDiff >= 50 && distanceDiff <= 90)
      { // turn right
        angle = map(distanceDiff, 50, 90, initAngle + 20, initAngle + 40);
        autoDrive(angle);
      }
      else if (distanceDiff <= -50 && distanceDiff >= -90)
      {
        angle = map(distanceDiff, -50, -90, initAngle - 20, initAngle - 40);
        autoDrive(angle);
      }
      else
      {
//        angle = distanceDiff < 0 ? initAngle - 40 : initAngle + 40;
        angle = initAngle + 40;
        autoDrive(angle);
      }
    }
    else
    {
      carStop();
      servo.write(initAngle);
      delay(150);
      carReverse(runSpeed);
      delay(600);
      carStop();
      angle = distanceDiff < 0 ? initAngle - 40 : initAngle + 40;
      autoDrive(angle);
    }
  }
}

void debugDistance(long left, long center, long right)
{
  Serial.print("왼쪽 거리:");
  Serial.print(left);
  Serial.print("mm");
  Serial.print("가운데 거리:");
  Serial.print(center);
  Serial.print("mm");
  Serial.print("오른쪽 거리:");
  Serial.print(right);
  Serial.println("mm");
}


int measureDistance(int echo, int trig)
{
  long duration, distance; // 변수 설정

  digitalWrite(trig, HIGH); // 초음파 발생
  delayMicroseconds(20);

  digitalWrite(trig, LOW); // 초음파 끄기
  delayMicroseconds(20);

  duration = pulseIn(echo, HIGH);   // echo에 특정 신호 될때까지 시간 측정 함수 입력
  distance = duration * 170 / 1000; //mm로 변환
  distance = constrain(distance, 0, 300);

  return distance;
}
void autoDrive(int angle) {
  servo.write(angle);
  delay(changeAngleTime);
  carStart(runSpeed);
  delay(runTime);
}

void carStop()
{
  analogWrite(MOTOR_PIN1, 0);
  analogWrite(MOTOR_PIN2, 0);
}

void carStart(uint8_t speed)
{
  analogWrite(MOTOR_PIN1, speed);
  analogWrite(MOTOR_PIN2, 0);
}

void carReverse(uint8_t speed)
{
  analogWrite(MOTOR_PIN1, 0);
  analogWrite(MOTOR_PIN2, speed);
}
