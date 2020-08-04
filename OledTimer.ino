#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

void setup(void) {
  u8g2.begin();
}

void loop(void) {
  unsigned long time;
  u8g2.clearBuffer();          // clear the internal menory
  u8g2.setFont(u8g2_font_logisoso32_tr);  // choose a suitable font
  u8g2.setCursor (0, 32);
  time = (millis()/10);
  u8g2.print(time);
  u8g2.sendBuffer();          // transfer internal memory to the display
  delay(10);
}
