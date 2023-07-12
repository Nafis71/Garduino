#include <SoftwareSerial.h>
SoftwareSerial nodemcu(2,3); //rx tx

void setup() {
  Serial.begin(9600);
  nodemcu.begin(9600);
  pinMode(A0,INPUT);

}

void loop() {
  int data = analogRead(A0);
  nodemcu.print(data);
  Serial.println(data);
  nodemcu.println("\n");
  delay(200);

}
