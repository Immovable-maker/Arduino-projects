#include<SoftwareSerial.h>
int measurePin = 0; //아날로그 0 핀 먼지센서 연결
int relay = 2; //릴레이 모듈 핀 번호 설정
int btTx = 3; //블루투스 TX핀 설정
int btRx = 4; //블루투스 RX핀 설정
int ledPower = 5; //먼지센서 LED 설정

int samplingTime = 280;
int deltaTime = 40; 
int sleepTime = 9680; //LED delay 관련 기본 시간 설정

int limitation = 50; // 미세먼지 농도 조건 단위 microgram/m3

float voMeasured = 0; //dustDensity를 측정하기 위해 구하는 첫 parameter
float calcVoltage = 0; //dustDensity를 측정하기 위한 구하는 두번째 parameter
float dustDensity = 0; //dustDensity int 설정




SoftwareSerial mySerial(btTx,btRx);//시리얼 통신 객체선언
String myString = ""; //블루투스 핸드폰으로 받는 문자열


void setup() {
  
  pinMode(relay,OUTPUT); // relay 모듈 아웃풋 설정
  pinMode(ledPower,OUTPUT); // 미세먼지 led 아웃풋 설정
  Serial.begin(9600); // 확인을 위한 시리얼 모니터 통신 설정
  mySerial.begin(9600); // 블루투스 통신을 위한 mySerial 통신 설정

}

void loop() {
  
  while(mySerial.available()){ //bluetooth 통신이 되는 동안
    
    char myChar = (char)mySerial.read(); // Terminal 창에 입력한 문자를 myChar라는 객체에 저장
    myString+=myChar; //myString 에 입력한 문자열 추가
    delay(5); // 원활한 통신을 위한 약간의 딜레이
    
  }
  if(!myString.equals(""))  //myString 값이 있을 때
  {
    Serial.println("input value: " + myString); //시리얼모니터에 myString값 출력
 
      if(myString=="up"){  //핸드폰 Terminal에 up을 입력하면 
        limitation = limitation + 10; //limitation 을 10 올려준다
      } 
      else if(myString="down"){
        limitation = limitation - 10; //limitation 을 10 내려준다     
      }
      else{
        limitation = limitation; //인풋값이 ?일때 limitation값을 일정하게 유지시켜주는 장치
      }
      
    myString="";  //myString 변수값 초기화
  }
  
  check(); // void loop 함수 밑에 작성해놓은 미세먼지 check 함수를 작동

  if(dustDensity>limitation){ // 설정해 놓은 값보다 현재 공기 중 농도가 클 때 
    digitalWrite(relay,LOW);// relay를 작동 (현재 설계해놓은 바로는 LOW일때 작동)
    delay(3000); // 연속적인 작동 방지를 위한 3초 딜레이
    check(); // check함수를 다시 작동시켜 농도 반복 측정
    while(dustDensity>limitation){ // 마찬가지로 농도가 설정값 초과일때
      digitalWrite(relay,LOW); // relay로 그대로 작동
      delay(3000); 
      check(); //농도 다시 업로드 
    }
  }
  else{
    digitalWrite(relay,HIGH); //설정값이 공기 중 먼지 농도보다 작을 때
    delay(5000);
  }
  delay(1000);
}

void check(){

  digitalWrite(ledPower,LOW);
  delayMicroseconds(samplingTime);

  delayMicroseconds(deltaTime);
  digitalWrite(ledPower,HIGH);
  delayMicroseconds(sleepTime); //위 과정은 미세먼지 LED를 통한 센서 확인 과정\

  voMeasured = analogRead(measurePin); // 아날로그 핀에서 voMeasured 값 받아오기

  calcVoltage = voMeasured * (5.0/1024.0); // 측정을 위한 두 번째 parameter 계산

  dustDensity = ( 0.17 * calcVoltage -0.1 )*1000; // dustDensity 측정

  Serial.print("Raw Signal Value(0~1023): ");
  Serial.println(voMeasured);

  Serial.print("- Voltage: ");
  Serial.println(calcVoltage);

  Serial.print("- Limitation: ");
  Serial.println(limitation);

  Serial.print("- Dust Density: ");
  Serial.print(dustDensity);
  Serial.println("microgram/m3"); // Serial 창에 위 값들이 잘 나오는 지 확인을 위해 넣은 코드 unit :microgram/m3
  

  mySerial.print("- Limitation: ");
  mySerial.print(limitation);
  mySerial.println("microgram/m3");

  mySerial.print("- Dust Density: ");
  mySerial.print(dustDensity);
  mySerial.println("microgram/m3"); //unit :microgram/m3
  mySerial.println("up 또는 down을 입력하여 On Off를 조절할 먼지 수치를 설정하세요");//블루투스를 통해 핸드폰 화면창에 필요한 값들만 추려서 입력
  delay(1000);

}
