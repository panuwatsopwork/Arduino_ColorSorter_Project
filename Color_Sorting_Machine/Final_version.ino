/* Connect SCL    to analog 5
   Connect SDA    to analog 4
   Connect VDD    to 3.3V DC
   Connect GROUND to common ground */

#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Servo.h>

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
Servo servo1;  // ตัวกลาง
Servo servo2;  // ตัวแยกสี

bool systemEnabled = false;

// องศาตามลำดับสี: แดง เขียว เหลือง ส้ม น้ำตาล
int slotAngles[5] = {165, 125, 90, 45, 0};

void setup() {
  Serial.begin(9600);
  if (!tcs.begin()) {
    Serial.println("ไม่พบเซ็นเซอร์สี");
    while (1);
  }

  servo1.attach(8);
  servo2.attach(9);

  servo1.write(174);
  servo2.write(90);

  Serial.println("พิมพ์ 1 เพื่อเริ่มทำงานวนลูป | พิมพ์ 0 เพื่อหยุด");
}

void loop() {
  // รับคำสั่งเปิด/ปิดระบบ
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == '1') {
      Serial.println("▶ เริ่มทำงาน (วนลูป)");
      systemEnabled = true;
    } else if (cmd == '0') {
      Serial.println("⏹ หยุดทำงาน");
      systemEnabled = false;
      servo1.write(174);
      servo2.write(90);
    }
  }

  if (!systemEnabled) return;

  // STEP 1: Servo1 ไปรับ Skittle
  servo1.write(174);
  delay(2000);

  // STEP 2: เคลื่อนมาอ่านสี
  servo1.write(80);
  delay(1000);
    servo1.write(86);
  delay(800);


  // STEP 3: ตรวจจับชื่อสีซ้ำ 3 ครั้ง
  String lastColor = "";
  int sameCount = 0;

  while (systemEnabled && sameCount < 3) {
    uint16_t r, g, b, c;
    tcs.getRawData(&r, &g, &b, &c);

    String currentColor = detectColor(r, g, b);

    Serial.print("R: "); Serial.print(r);
    Serial.print(" G: "); Serial.print(g);
    Serial.print(" B: "); Serial.print(b);
    Serial.print(" → สี: "); Serial.println(currentColor);

    if (currentColor == lastColor && currentColor != "ไม่ทราบ") {
      sameCount++;
      Serial.print("✅ สี "); Serial.print(currentColor);
      Serial.print(" ตรงกัน ("); Serial.print(sameCount); Serial.println("/3)");
    } else {
      sameCount = 1;
      lastColor = currentColor;
      Serial.println("🔁 เริ่มนับสีใหม่");
    }

    delay(300);
  }

  if (!systemEnabled) return;

  // STEP 4: เคลื่อน Servo2 ไปยังช่องสี
  int index = getColorIndex(lastColor);
  if (index == -1) {
    Serial.println("⚠ ไม่สามารถระบุสีได้");
    servo2.write(90);
  } else {
    Serial.print("📦 เคลื่อน Servo2 ไปยังช่อง: "); Serial.println(slotAngles[index]);
    servo2.write(slotAngles[index]);
  }

  delay(700);

  // STEP 5: Servo1 ปล่อยของที่ตำแหน่ง 0°
  servo1.write(0);
  Serial.println("🌀 ปล่อย Skittle");

  delay(1000);

  // STEP 6: Reset ตำแหน่ง
  servo2.write(90);
  delay(500);
}

// ===== ฟังก์ชันช่วย =====
String detectColor(uint16_t r, uint16_t g, uint16_t b) {
  // เหลือง: R และ G สูงมาก, B ต่ำ
  if (r > 7000 && g > 5000 && b < 3000) return "เหลือง";

  // ส้ม: R สูง, G กลาง, B ต่ำ
  if (r > 6500 && g > 2000 && g < 4000 && b < 2000) return "ส้ม";

  // เขียว: G เด่นสุด, R และ B ต่ำกว่า
  if (g > 2800 && g > r + 800 && g > b + 800) return "เขียว";

  // แดง: R เด่น, G/B ต่ำ
  if (r > 2000 && g < 1500 && b < 1500) return "แดง";

  // น้ำตาล: R/G/B ทั้งหมดต่ำ
  if (r < 1500 && g < 1000 && b < 800) return "น้ำตาล";

  return "ไม่ทราบ";
}



int getColorIndex(String color) {
  if (color == "แดง") return 0;
  if (color == "เขียว") return 1;
  if (color == "เหลือง") return 2;
  if (color == "ส้ม") return 3;
  if (color == "น้ำตาล") return 4;
  return -1;
}
