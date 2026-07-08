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
    movestring = Serial.readString();
    move = movestring.toInt();
    stepper2.step(move);
    movestring = "";

}
