#include <TMCStepper.h>
#include <math.h>

#define EN_PIN1     26
#define STEP_PIN1   14
#define DIR_PIN1    27

#define EN_PIN2     25
#define STEP_PIN2   33
#define DIR_PIN2    32

#define UART_TX     17
#define UART_RX     16

#define DRIVER_ADDRESS1 0b00
#define DRIVER_ADDRESS2 0b01
#define R_SENSE 0.11f

HardwareSerial SERIAL_PORT(1);

TMC2209Stepper driver1(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS1);
TMC2209Stepper driver2(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS2);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  pinMode(EN_PIN1, OUTPUT);
  pinMode(STEP_PIN1, OUTPUT);
  pinMode(DIR_PIN1, OUTPUT);
  digitalWrite(EN_PIN1, LOW);

  pinMode(EN_PIN2, OUTPUT);
  pinMode(STEP_PIN2, OUTPUT);
  pinMode(DIR_PIN2, OUTPUT);
  digitalWrite(EN_PIN2, LOW);

  SERIAL_PORT.begin(9600, SERIAL_8N1, UART_RX, UART_TX);

  driver1.begin();
  driver1.toff(4);
  driver1.rms_current(1800);
  driver1.microsteps(64);
  driver1.en_spreadCycle(true);
  driver1.intpol(true);

  driver2.begin();
  driver2.toff(4);
  driver2.rms_current(1800);
  driver2.microsteps(64);
  driver2.en_spreadCycle(true);
  driver2.intpol(true);

  Serial.println("Motors ready.");
}

void moveStepper(int stepPin, int dirPin, int steps, unsigned long duration) {
  if (steps == 0) return; // No movement needed

  digitalWrite(dirPin, steps > 0 ? HIGH : LOW);
  int absSteps = abs(steps);

  unsigned long stepDelay = duration * 1000UL / absSteps; // in microseconds

  for (int i = 0; i < absSteps; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(stepDelay - 10);
  }
}

void drawRectangle(int width, int height, int moveTime) {
  // Move right (motor1)
  moveStepper(STEP_PIN1, DIR_PIN1, width, moveTime);
  delay(500);
  // Move up (motor2)
  moveStepper(STEP_PIN2, DIR_PIN2, height, moveTime);
  delay(500);
  // Move left (motor1)
  moveStepper(STEP_PIN1, DIR_PIN1, -width, moveTime);
  delay(500);
  // Move down (motor2)
  moveStepper(STEP_PIN2, DIR_PIN2, -height, moveTime);
  delay(500);
}

void loop() {
  Serial.println("Drawing rectangle...");
  drawRectangle(1000, 1000, 2000); // Adjust steps as needed
  Serial.println("Rectangle complete.");
  while(true);
}
