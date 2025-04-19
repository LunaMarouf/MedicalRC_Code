#include <IRremote.h>
#include <Servo.h>

#define IN1 7
#define IN2 8
#define SERVO_PIN 6
#define JOY_X A0
#define JOY_SW 4
#define RECV_PIN 11

// Ultrasonic Sensor
#define TRIG_PIN 2
#define ECHO_PIN 3

// Buzzer
#define BUZZER_PIN 12


Servo servomotor;
int servoPos = 90;
bool motorState = false;
bool lastJoyButton = HIGH;

void setup() {
  Serial.begin(9600);
  IrReceiver.begin(RECV_PIN, ENABLE_LED_FEEDBACK);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);

  pinMode(JOY_SW, INPUT_PULLUP);

  servomotor.attach(SERVO_PIN);
  servomotor.write(servoPos); // center

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  // === Ultrasonic Distance Check ===
  float distance = getDistanceCM();
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (distance < 40.0) {
    setMotor(false);
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("Obstacle detected! Motor stopped. Buzzer ON.");
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  // === IR Remote ===
  if (IrReceiver.decode()) {
    String irNum = String(IrReceiver.decodedIRData.command, HEX);
    Serial.print("IR Code: ");
    Serial.println(irNum);

    if (irNum == "40") {
      servoPos = 180;      // Right
    } else if (irNum == "44") {m
      servoPos = 0; // Left
    } else if (irNum == "46") {
      servoPos = 90; // center 
    } else if (irNum == "43") {
      motorState = true;
      setMotor(motorState);
    } else if (irNum == "5e") {
      motorState = false;
      setMotor(motorState);
    }

    IrReceiver.resume();
  }
  // === Joystick Steering ===
  int joyX = analogRead(JOY_X);
  if (joyX < 470 || joyX > 570) {
    if (joyX < 470) servoPos = 180;
    else if (joyX > 570) servoPos = 0;
  }
  servomotor.write(servoPos);

  bool joyBtn = digitalRead(JOY_SW);
  if (joyBtn == LOW && lastJoyButton == HIGH) {
    motorState = !motorState;
    setMotor(motorState);
    delay(300); // debounce
  }
  lastJoyButton = joyBtn;
}

// === Motor ON/OFF ===
void setMotor(bool on) {
  if (on) {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    Serial.println("Motor ON");
  } else {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    Serial.println("Motor OFF");
  }
}

// === Ultrasonic Distance Function ===
float getDistanceCM() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30 ms timeout

  if (duration == 0) {
    return 999; // No object detected
  }

  return duration * 0.034 / 2;
} 
