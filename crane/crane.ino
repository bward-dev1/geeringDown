#include <Stepper.h>

#define STEPS 32
String movestring = "";
int move = 0;
Stepper stepper1(STEPS, 8, 10, 9, 11); 
Stepper stepper2(STEPS, 2, 4, 3, 5);
int val = 0;
void setup() {
    Serial.begin(9600);
    stepper1.setSpeed(400);
    stepper2.setSpeed(400);
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
                stepper2.step(move);
            } else {
                Serial.println("Invalid motor. Use 'a' or 'b' followed by steps.");
            }
        }
    }
    movestring = "";
}
