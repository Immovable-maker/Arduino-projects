void setup() {
  Serial.begin(115200);
  pinMode(A0, INPUT_PULLUP);
}

void loop() {
  float a = analogRead(A0);
  Serial.println(a);
  delay(50);
}
