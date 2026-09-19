#include <TMCStepper.h>

#define EN_PIN1     26  
#define STEP_PIN1   14  
#define DIR_PIN1    27  

#define EN_PIN2     25  
#define STEP_PIN2   33  
#define DIR_PIN2    32  

#define UART_TX     17  
#define UART_RX     16  

#define JOY_X       34  
#define JOY_Y       35  

#define DRIVER_ADDRESS1 0b00 
#define DRIVER_ADDRESS2 0b01 
#define R_SENSE 0.11f       

#define DEADZONE 200  

#define MIN_SPEED  800  
#define MAX_SPEED  100  

HardwareSerial SERIAL_PORT(1); 

TMC2209Stepper driver1(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS1);
TMC2209Stepper driver2(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS2);

void setup() {
  Serial.begin(115200);
  while (!Serial);

  pinMode(EN_PIN1, OUTPUT);
  pinMode(STEP_PIN1, OUTPUT);
  pinMode(DIR_PIN1, OUTPUT);
  digitalWrite(EN_PIN1, HIGH);

  pinMode(EN_PIN2, OUTPUT);
  pinMode(STEP_PIN2, OUTPUT);
  pinMode(DIR_PIN2, OUTPUT);
  digitalWrite(EN_PIN2, HIGH);

  pinMode(JOY_X, INPUT);
  pinMode(JOY_Y, INPUT);

  SERIAL_PORT.begin(115200, SERIAL_8N1, UART_RX, UART_TX);

  driver1.begin();
  driver1.toff(4);          
  driver1.rms_current(1300); 
  driver1.microsteps(64);   
  driver1.en_spreadCycle(true); 
  driver1.intpol(true);     
  driver1.tbl(2);           
  driver1.hysteresis_end(10);
  driver1.hysteresis_start(6);

  driver2.begin();
  driver2.toff(4);
  driver2.rms_current(1300);
  driver2.microsteps(64);
  driver2.en_spreadCycle(true);
  driver2.intpol(true);
  driver2.tbl(2);
  driver2.hysteresis_end(10);
  driver2.hysteresis_start(6);

  Serial.println("Motors ready, waiting for joystick input...");
}

void loop() {
  int joyX = analogRead(JOY_X);  
  int joyY = analogRead(JOY_Y);  

  bool motor1_active = false, motor2_active = false;
  int speed1 = MIN_SPEED, speed2 = MIN_SPEED;

  if (abs(joyX - 2048) > DEADZONE) { 
    digitalWrite(EN_PIN1, LOW);  
    digitalWrite(DIR_PIN1, joyX > 2048 ? HIGH : LOW); 
    speed1 = map(abs(joyX - 2048), DEADZONE, 2048, MIN_SPEED, MAX_SPEED); 
    motor1_active = true;
  } else {
    digitalWrite(EN_PIN1, HIGH); 
  }

  if (abs(joyY - 2048) > DEADZONE) { 
    digitalWrite(EN_PIN2, LOW);  
    digitalWrite(DIR_PIN2, joyY > 2048 ? HIGH : LOW); 
    speed2 = map(abs(joyY - 2048), DEADZONE, 2048, MIN_SPEED, MAX_SPEED); 
    motor2_active = true;
  } else {
    digitalWrite(EN_PIN2, HIGH); 
  }

  if (motor1_active) {
    for (int i = 0; i < 10; i++) {  
      digitalWrite(STEP_PIN1, HIGH);
      delayMicroseconds(2);
      digitalWrite(STEP_PIN1, LOW);
      delayMicroseconds(speed1);
    }
  }

  if (motor2_active) {
    for (int i = 0; i < 10; i++) {
      digitalWrite(STEP_PIN2, HIGH);
      delayMicroseconds(2);
      digitalWrite(STEP_PIN2, LOW);
      delayMicroseconds(speed2);
    }
  }

  Serial.print("JoyX: "); Serial.print(joyX);
  Serial.print(" | JoyY: "); Serial.print(joyY);
  Serial.print(" | Speed1: "); Serial.print(speed1);
  Serial.print(" | Speed2: "); Serial.println(speed2);
}