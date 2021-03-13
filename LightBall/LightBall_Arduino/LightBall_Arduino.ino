#include <Adafruit_NeoPixel.h>
#include "M5Atom.h" //ESP32 PICO
#include <BluetoothSerial.h>

#define PIN 25

BluetoothSerial SerialBT;

char command;
String string, setColor;
boolean ledOn = false;
int ledColor;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(72, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  M5.begin(true, false, false);
  strip.begin();
  colorSet(0, 0, 0);
  SerialBT.begin("ESPLIGHTBALL");
  Serial.println("The device started, now you can pair it with bluetooth!");
}

void loop() {
  if (SerialBT.available()) {
    string = "";
  }

  while (SerialBT.available() > 0) {
    command = ((byte)SerialBT.read());
    if (command == ':') {
      break;
    }
    else {
      string += command;
    }
    Serial.println(string);
    delay(10);
  }

  if (string == "O") {
    ledOn = true;
    colorSet(255 , 255, 255);
  }
  else if (string == "F") {
    colorSet(0, 0, 0);
    ledOn = false;
  }
  else {
    return;
  }

  if (ledOn == true)
  {
    switch (string)
    {
      case 'W':
        colorSet(255, 255, 255);
        delay(500);
        colorSet(0, 0, 0);
        delay(500);
        return;
      case 'R':
        colorSet(255, 0, 0);
        return;
      case 'G':
        colorSet(0, 255, 0);
        delay(500);
        colorSet(0, 0, 0);
        delay(500);
        return;
      case 'B':
        colorSet(0, 0, 255);
        return;
    }
  }
  delay(10);

 // if you wanna control brightness, fix this code
//  if ((string.toInt() >= 0) && (string.toInt() <= 255))
//  {
//    if (ledon == true)
//    {
//      analogWrite(led, string.toInt());
//      Serial.println(string);
//      delay(10);
//    }
//  }
}

void colorSet(int a, int b, int c)
{
  strip.setBrightness(255);
  for (int i = 0; i < 72; i++)
    strip.setPixelColor(i, strip.Color(a, b, c));
  strip.show();
}
