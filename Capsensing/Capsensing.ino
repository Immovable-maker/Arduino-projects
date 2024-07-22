#include <Wire.h>
#define SLAVEWRT 0x48 // adress of device during write cycle
#define SLAVERD  0x48 // adress of device during read cycle

void setup() {
  //Set pin 22 (RDY pin as digital input)
  pinMode(22,INPUT);
  //USB setup
  Serial.begin(9600);
  //Setup i2c
  Wire.begin();
  //Cap setup register
  Wire.beginTransmission(SLAVEWRT);
  Wire.write(0x07); // sets memory pointer to CAP setup register 
  Wire.write(0x80); // sends data to cap setup enabling correct function of chip 
  Wire.endTransmission(SLAVEWRT);
  //Done
  delay(4);
  //Excitation setup
  Wire.beginTransmission(SLAVEWRT);
  Wire.write(0x09);
  Wire.write(0x0A);
  Wire.endTransmission(SLAVEWRT);
  //Done
  delay(4);
  //Configuration register
  Wire.beginTransmission(SLAVEWRT);
  Wire.write(0x0A);
  Wire.write(0xA1);
  Wire.endTransmission(SLAVEWRT);
  //Done
  delay(4);
  //Setup CAPDAC
  //Wire.beginTransmission(SLAVEWRT);
  //Wire.write(0x0B);
  //Wire.write(0x00);
  //Wire.endTransmission(SLAVEWRT);
  //Done
}

void loop() {
     int ready = digitalRead(22);// sets up integer 'ready' as data from pin 2 
     while (digitalRead(22) ==HIGH)// until pin 2 transitions high to low, do nothing
     {
      //Serial.println("High still");
     };
     Wire.beginTransmission(SLAVEWRT); // begin read cycle
     Wire.write(0x01); // send memory pointer to first cap data register
     Wire.endTransmission(); // end cycle
     Wire.requestFrom(SLAVERD,3); // reads 3 bytes starting from 0x01
     while(Wire.available())
    {
     char c = Wire.read();
     Serial.println(c, DEC); // reads the 3 bytes onto serial monitor in decimal form
    }
}
