#include <Servo.h>

#define S0 9
#define S1 8
#define S2 3
#define S3 4
#define OUT 5
#define LED 6
#define SERVO_CENTER_PIN 10

Servo servoCenter;

void setup() {
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(LED, OUTPUT);
  pinMode(OUT, INPUT);

  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH);
  digitalWrite(LED, HIGH);

  Serial.begin(9600);

  servoCenter.attach(SERVO_CENTER_PIN);
  servoCenter.write(85); // เริ่มที่ตำแหน่งอ่านสี
}

void loop() {
  // เช็คคำสั่งจาก Serial Monitor
  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == '1') {
      servoCenter.write(135);
      Serial.println("→ servoCenter ไปที่ 135 องศา (รับวัตถุ)");
    } else if (cmd == '0') {
      servoCenter.write(85);
      Serial.println("→ servoCenter กลับที่ 85 องศา (อ่านค่าสี)");
    }
  }

  // อ่านค่าสีโดยตรง
  int red, green, blue;

  // อ่านสีแดง
  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);
  delay(5);
  red = pulseIn(OUT, LOW);

  // อ่านสีเขียว
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  delay(5);
  green = pulseIn(OUT, LOW);

  // อ่านสีน้ำเงิน
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  delay(5);
  blue = pulseIn(OUT, LOW);

  // แสดงผลค่า
  Serial.print("RGB = [");
  Serial.print(red); Serial.print(", ");
  Serial.print(green); Serial.print(", ");
  Serial.print(blue); Serial.println("]");

  delay(1000); // อ่านทุก 1 วินาที
}
