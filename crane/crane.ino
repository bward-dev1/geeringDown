#include <Stepper.h>
#include <Servo.h>

#define STEPS 32
String movestring = "";
int move = 0;
Stepper stepper1(STEPS, 8, 10, 9, 11); 
Stepper baseStepper(STEPS, 2, 4, 3, 5);

Servo scoopServo;

int val = 0;
void setup() {
    Serial.begin(9600);
    stepper1.setSpeed(200);
    baseStepper.setSpeed(300);
    scoopServo.attach(6);
}
void loop() {

    if (Serial.available()) {
        movestring = Serial.readString();
        movestring.trim();
        
        if (movestring.length() > 1) {
            char motor = movestring.charAt(0);
            String stepsStr = movestring.substring(1);
            move = stepsStr.toInt();
            
            if (motor == 'a') {
                stepper1.step(move);
            } else if (motor == 'b') {
                baseStepper.step(move);
            } else if (motor == 's') {
                scoopServo.write(move);
                //180 = fully open, 0 = fully closed
            }
            else {
                Serial.println("Invalid motor. Use 'a', 'b', or 's' followed by steps.");
            }
        }
    }
    movestring = "";
}
