#include <Servo.h>

Servo myServo;
int pos = 90;            // current position, start at 90°
String inputBuffer = ""; // holds incoming serial data
bool newData = false;

void setup() {
  Serial.begin(9600);
  myServo.attach(13);    // servo signal on pin 13
  myServo.write(pos);    // move to starting position
  Serial.println("Servo control ready on pin 13.");
  Serial.println("Enter an angle (0–180) to move the servo.");
  Serial.print("Current: ");
  Serial.print(pos);
  Serial.println("°");
}

void loop() {
  readSerial();
  if (newData) {
    int angle = inputBuffer.toInt();
    if (inputBuffer.length() > 0 && angle >= 0 && angle <= 180) {
      pos = angle;
      myServo.write(pos);
      Serial.print("Moved to ");
      Serial.print(pos);
      Serial.println("°");
    } else {
      Serial.print("Invalid: '");
      Serial.print(inputBuffer);
      Serial.println("'. Enter 0–180.");
    }
    inputBuffer = "";
    newData = false;
  }
}

void readSerial() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      newData = true;
    } else if (c != '\r') {
      inputBuffer += c;
    }
  }
}
