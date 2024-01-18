#include <SPI.h>

#define L6470_SS_PIN 10

bool forward = true;
int rotationCount = 0;

void setup() {
  SPI.begin();
  pinMode(L6470_SS_PIN, OUTPUT);
  digitalWrite(L6470_SS_PIN, HIGH);

  L6470_setup();

  Serial.begin(9600);
}

void loop() {
  // Perform 10 forward and 10 reverse rotations
  if (rotationCount < 2) {
    rotateAndDelay(forward, 7.0, 10, 900); // Forward rotation
  } else if (rotationCount < 4) {
    rotateAndDelay(!forward, 7.0, 10, 900); // Reverse rotation
  } else {
    rotationCount = 0;
  }
}

void rotateAndDelay(bool direction, float kaiten, int speed, int delayTime) {
  if (direction) {
    rotateMotorMicrostep(kaiten, speed); // Forward rotation
  } else {
    rotateMotorMicrostep(-kaiten, speed); // Reverse rotation
  }

  delay(delayTime);

  stopMotor();
  delay(2000);

  rotationCount++;
}

void L6470_send(unsigned char value) {
  digitalWrite(L6470_SS_PIN, LOW);
  SPI.transfer(value);
  digitalWrite(L6470_SS_PIN, HIGH);
}

void L6470_setup() {
  L6470_send(0x0B); 
  L6470_send(0x0D); 
}

void rotateMotorMicrostep(float kaiten, int speed) {
  if (kaiten > 0) {
    L6470_send(0x50); // 正転
  } else {
    L6470_send(0x51); // 逆転
    kaiten = -kaiten; // Adjust the rotation angle for reverse direction
  }

  speed = constrain(speed, 0, 1023);
  byte speedHigh = speed / 256;
  byte speedLow = speed % 256;
  L6470_send(speedHigh);
  L6470_send(speedLow);

  int steps = kaiten * 128 * 4 * 200; // Use the corrected rotation angle
  for (int i = 0; i < steps; i++) {
    L6470_send(0x55); 
  }
}

void stopMotor() {
  L6470_send(0xB8); // モーターを停止
}
