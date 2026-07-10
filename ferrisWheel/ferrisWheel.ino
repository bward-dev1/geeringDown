#include <AccelStepper.h>

// 28BYJ-48 stepper motor: 32 * 64 gear ratio = 2048 steps per revolution
// Define pins for first stepper (original)
#define IN1_1 2
#define IN2_1 3
#define IN3_1 4
#define IN4_1 5

// Define pins for second stepper (pins 8-11)
#define IN1_2 8
#define IN2_2 9
#define IN3_2 10
#define IN4_2 11

// Create AccelStepper instances
AccelStepper stepper1(AccelStepper::FULL4WIRE, IN1_1, IN3_1, IN2_1, IN4_1);
AccelStepper stepper2(AccelStepper::FULL4WIRE, IN1_2, IN3_2, IN2_2, IN4_2);

void setup() {
  // Configure stepper 1
  stepper1.setMaxSpeed(300);       // Steps per second (~15 RPM for 28BYJ-48)
  stepper1.setAcceleration(100);   // Steps per second²
  stepper1.setCurrentPosition(0);
  
  // Configure stepper 2
  stepper2.setMaxSpeed(400);       // Steps per second (~15 RPM for 28BYJ-48)
  stepper2.setAcceleration(100);
  stepper2.setCurrentPosition(0);
}

void loop() {
  // Stepper 1: alternate between forward and backward
  if (stepper1.distanceToGo() == 0) {
    stepper1.moveTo(stepper1.currentPosition() == 0 ? -2048 : 0);
  }
  stepper1.run();
  
  // Stepper 2: alternate between forward and backward
  if (stepper2.distanceToGo() == 0) {
    stepper2.moveTo(stepper2.currentPosition() == 0 ? -2048 : 0);
  }
  stepper2.run();
  
}
// This is the end of the code. If it actually compiles, it might be the end of the world too.
