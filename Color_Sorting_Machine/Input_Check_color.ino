#include <Wire.h>
#include "Adafruit_TCS34725.h"
#include <Servo.h>

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
Servo servo1;

void setup() {
  Serial.begin(9600);

  if (!tcs.begin()) {
    Serial.println("ไม่พบเซ็นเซอร์ TCS34725");
    while (1);
  }

  servo1.attach(8);
  servo1.write(135);  // เริ่มต้นที่ 135°

  Serial.println("พิมพ์ 1 เพื่อทำงานครั้งเดียว | พิมพ์ 0 เพื่อรีเซ็ตกลับไปแช่ 135°");
}

void loop() {
  // รับค่าจาก Serial Monitor
  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == '1') {
      Serial.println("▶ เริ่มทำงาน 1 รอบ");

      // เคลื่อนไปที่ 135°
      servo1.write(135);
      delay(2000);

      // เคลื่อนไปที่ 86°
      servo1.write(86);

      Serial.println("✅ จบรอบ รอคำสั่งใหม่");
    }

    else if (cmd == '0') {
      Serial.println("⏹ รีเซ็ต → ไป 0°, แล้วกลับไป 135°");

      // ไป 0°
      servo1.write(0);
      delay(1000);

      // กลับมาแช่ที่ 135°
      servo1.write(135);

      Serial.println("🕒 แช่ที่ 135° รอคำสั่งใหม่");
    }
  }

  // อ่านค่าระหว่างรอ
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  Serial.print("R: "); Serial.print(r);
  Serial.print(" G: "); Serial.print(g);
  Serial.print(" B: "); Serial.println(b);

  delay(1000);
}
