#include <Servo.h>

// ขาเซนเซอร์ GY-31
#define S0 9
#define S1 10
#define S2 7
#define S3 6
#define OUT 5
#define LED 8

// Servo
#define SERVO_CENTER_PIN 11
#define SERVO_SELETE_PIN 12

Servo servoCenter;
Servo servoSelete;

int red, green, blue;
String lastColor = "";
int sameColorCount = 0;
const int colorThreshold = 3;

bool isRunning = false;  // เริ่มต้นยังไม่ทำงาน

// ====== Struct และฐานข้อมูลสี ======
struct ColorRef {
  String name;
  int r, g, b;
};

ColorRef knownColors[] = {
  {"แดง", 6, 7, 6},
  {"เขียว", 6, 6, 6},
  {"เหลือง", 5, 6, 6},
  {"ส้ม", 5, 7, 6},
  {"น้ำตาล", 7, 7, 6}
};
// ====================================

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
  Serial.println("ระบบพร้อมแล้ว รอคำสั่งจากคุณ...");
  Serial.println("พิมพ์ 1 เพื่อเริ่ม / พิมพ์ 0 เพื่อหยุด");

  servoCenter.attach(SERVO_CENTER_PIN);
  servoSelete.attach(SERVO_SELETE_PIN);
}

void loop() {
  // ตรวจสอบคำสั่งจาก Serial Monitor
  if (Serial.available()) {
    char command = Serial.read();
    if (command == '1') {
      if (!isRunning) {
        isRunning = true;
        Serial.println("🟢 เริ่มทำงาน");
        startCycle();  // เริ่มรอบแรกเฉพาะตอนสั่ง
      }
    } else if (command == '0') {
      isRunning = false;
      Serial.println("🛑 หยุดทำงาน");
    }
  }

  if (!isRunning) {
    delay(200); // ประหยัด CPU
    return;
  }

  // อ่านค่าสี
  red = readColorAvg(LOW, LOW);
  green = readColorAvg(HIGH, HIGH);
  blue = readColorAvg(LOW, HIGH);

  Serial.print("RGB = [");
  Serial.print(red); Serial.print(", ");
  Serial.print(green); Serial.print(", ");
  Serial.print(blue); Serial.print("]");

  String detectedColor = detectColor(red, green, blue);
  Serial.print(" → สีที่พบ: ");
  Serial.println(detectedColor);

  if (detectedColor == lastColor && detectedColor != "ไม่สามารถระบุสีได้") {
    sameColorCount++;
  } else {
    sameColorCount = 1;
    lastColor = detectedColor;
  }

  if (sameColorCount >= colorThreshold) {
    int angle = getColorAngle(detectedColor);
    Serial.print("ยืนยันสี: "); Serial.print(detectedColor);
    Serial.print(" → หมุน servo selete ไปที่ "); Serial.println(angle);

    servoSelete.write(angle);
    delay(1000);

    servoCenter.write(0);
    Serial.println("→ servo center ไปที่ 0°");
    delay(1000);

    startCycle(); // รีเซ็ตใหม่หลังทำงานเสร็จ
  }

  delay(500);
}

void startCycle() {
  Serial.println("\n=== เริ่มรอบใหม่ ===");
  sameColorCount = 0;
  lastColor = "";

  servoCenter.write(180);
  delay(2000);
  servoCenter.write(85);
}

int readColorAvg(bool s2Val, bool s3Val) {
  const int numReads = 10;
  long sum = 0;

  digitalWrite(S2, s2Val);
  digitalWrite(S3, s3Val);
  delay(5);

  for (int i = 0; i < numReads; i++) {
    sum += pulseIn(OUT, LOW);
    delay(2);
  }

  return sum / numReads;
}

String detectColor(int r, int g, int b) {
  float minDist = 99999;
  String closest = "ไม่สามารถระบุสีได้";

  for (ColorRef color : knownColors) {
    float dist = sqrt(pow(r - color.r, 2) + pow(g - color.g, 2) + pow(b - color.b, 2));
    if (dist < minDist) {
      minDist = dist;
      closest = color.name;
    }
  }

  return closest;
}

int getColorAngle(String color) {
  if (color == "แดง") return 0;
  if (color == "เหลือง") return 45;
  if (color == "เขียว") return 90;
  if (color == "ส้ม") return 135;
  if (color == "น้ำตาล") return 180;
  return 90;
}
