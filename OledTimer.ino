#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);     // setting up correct display for u8g2

void setup() {
  Serial.begin(9600);
  u8g2.begin();
}

void loop() {
  screenTimer();
}

void screenTimer() {
  unsigned long time, mseconds, seconds, minutes;

  u8g2.clearBuffer();                       // clears the internal memory (buffer)
  u8g2.setFont(u8g2_font_logisoso32_tr);    // font selection

  time = millis();                          // starts time counting using millis()

  mseconds = time;
  if (mseconds >= 1000) {
    mseconds -= 1000 * (time / 1000);       // keeps milliseconds a 2 digit number
  }
  u8g2.setCursor (90, 32);                  // sets "starting point" for milliseconds digits
  u8g2.print(u8x8_u16toa(mseconds, 2));     // draws milliseconds to that point (in memory)

  u8g2.setCursor (81, 32);                
  u8g2.print(";");                          // sets "starting point" and draws divider for seconds and milliseconds

  seconds = (time / 1000);
  if (seconds >= 60) {
    seconds -= 60 * ((time / 1000) / 60);   // keeps seconds under 60
  }
  u8g2.setCursor (44, 32);                
  u8g2.print(u8x8_u16toa(seconds, 2));      // same formula repeats itself with seconds

  u8g2.setCursor (36, 32);
  u8g2.print(":");                        

  minutes = ((time / 1000) / 60);
  u8g2.setCursor (0, 32);                
  u8g2.print(u8x8_u16toa(minutes, 2));      // same formula repeats itself with minutes
  u8g2.sendBuffer();                        // sends internal memory (buffer) to the display
}
