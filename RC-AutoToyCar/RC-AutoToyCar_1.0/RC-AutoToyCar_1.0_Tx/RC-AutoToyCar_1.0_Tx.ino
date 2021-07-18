#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define joystickAngle A0
#define joystickAngle_click 6
#define joystickCarSpeed A1
#define joystickCarSpeed_sub 5


RF24 radio(7, 8); // SPI 버스에 nRF24L01 라디오를 설정하기 위해 CE, CSN를 선언.
const byte address[6] = "00101"; //주소값을 5가지 문자열로 변경할 수 있으며,
//송신기와 수신기가 동일한 주소로 해야됨.

uint8_t data[2];



void setup() {
  radio.begin();
  Serial.begin(9600);
  radio.openWritingPipe(address); //이전에 설정한 5글자 문자열인 데이터를 보낼 수신의 주소를 설정
  radio.setPALevel(RF24_PA_MIN); //전원공급에 관한 파워레벨을 설정합니다. 모듈 사이가 가까우면 최소로 설정합니다.
  radio.stopListening(); //모듈을 송신기로 설정

  pinMode (joystickAngle, INPUT) ;
  pinMode (joystickCarSpeed, INPUT) ;
}

void loop() {


  int angle = map(analogRead (joystickAngle), 0, 1023, 105, 25) ; //가운데 90도를 기준으로 하여 줄 값 선정
  int carSpeed = map(analogRead (joystickCarSpeed), 0, 1023, 255, 0) ; //1023까지의 값을 255크기로 매핑
  data[0] = angle; //data버퍼에 방향 크기 할당
  data[1] = carSpeed; //data버퍼에 오른쪽 속도 크기 할당
  Serial.println(radio.available());
  radio.write(&data, sizeof(data)); //해당 메시지를 수신자에게 보냄


    Serial.print(" direction_angle : ");
    Serial.println(data[0], DEC);
    Serial.print(" speed : ");
    Serial.println(data[1], DEC);


}
