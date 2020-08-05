#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);     // setting up correct display for u8g2 library

// statemachine variables for switch 1
int state_s1 = 0;
int state_prev_s1 = 0;
int pin_s1 = 2;
int value_s1 = 0;
unsigned long t_s1 = 0;
unsigned long t_0_s1 = 0;
unsigned long bounce_delay_s1 = 5;
int press_s1 = 0;

// statemachine variables for switch 2
int state_s2 = 0;
int state_prev_s2 = 0;
int pin_s2 = 3;
int value_s2 = 0;
unsigned long t_s2 = 0;
unsigned long t_0_s2 = 0;
unsigned long bounce_delay_s2 = 5;
int press_s2 = 0;

// statemachine variables for timer
int state_timer = 0;
int state_prev_timer = 0;
unsigned long t_timer = 0;
unsigned long t_0_timer = 0;



void setup() {
  Serial.begin(9600);
  u8g2.begin();
  u8g2.setFont(u8g2_font_logisoso32_tr);
  pinMode(pin_s1, INPUT_PULLUP);
  pinMode(pin_s2, INPUT_PULLUP);
}

void loop() {
  SM_s1();
  SM_s2();
  if (state_s1 == 4) {
    Serial.println("Button #1 pressed");
    ++press_s1;
  }
  if (state_s2 == 4) {
    Serial.println("Button #2 pressed");
    ++press_s2;
  }
  screenTimer();
}

void screenTimer() {
    unsigned long startTime, time, currentTime, mseconds, seconds, minutes;

    u8g2.clearBuffer();                         // clears the internal memory (buffer)
    startTime = millis();                       // starts counting milliseconds using millis()

    mseconds = 0;
    u8g2.setCursor (90, 32);                    // sets "starting point" for milliseconds digits
    u8g2.print(u8x8_u16toa(mseconds, 2));       // draws milliseconds to that point (in memory)

    u8g2.setCursor (81, 32);                
    u8g2.print(";");                            // sets "starting point" and draws divider for seconds and milliseconds

    seconds = 0;
    u8g2.setCursor (44, 32);                
    u8g2.print(u8x8_u16toa(seconds, 2));        // same formula repeats itself with seconds
    
    u8g2.setCursor (36, 32);
    u8g2.print(":");                        

    minutes = 0;
    u8g2.setCursor (0, 32);                
    u8g2.print(u8x8_u16toa(minutes, 2));        // same formula repeats itself with minutes
    u8g2.sendBuffer();                          // sends internal memory (buffer) to the display (00:00;00)

    while (press_s1 == 1 && press_s2 == 1) {
      u8g2.clearBuffer();                         // clears the internal memory (buffer)
      currentTime = millis();                     // starts counting milliseconds using millis()
      time = currentTime - startTime;             // sets time to 0
      mseconds = time / 10;
      if (mseconds >= 100) {
        mseconds -= 100 * (time / 1000);          // keeps milliseconds a 2 digit number
      }
      u8g2.setCursor (90, 32);                    // sets "starting point" for milliseconds digits
      u8g2.print(u8x8_u16toa(mseconds, 2));       // draws milliseconds to that point (in memory)

      u8g2.setCursor (81, 32);                
      u8g2.print(";");                            // sets "starting point" and draws divider for seconds and milliseconds

      seconds = (time / 1000);
      if (seconds >= 60) {
        seconds -= 60 * ((time / 1000) / 60);     // keeps seconds under 60
      }
      u8g2.setCursor (44, 32);                
      u8g2.print(u8x8_u16toa(seconds, 2));        // same formula repeats itself with seconds

      u8g2.setCursor (36, 32);
      u8g2.print(":");                        

      minutes = ((time / 1000) / 60);
      u8g2.setCursor (0, 32);                
      u8g2.print(u8x8_u16toa(minutes, 2));        // same formula repeats itself with minutes
      u8g2.sendBuffer();                          // sends internal memory (buffer) to the display
    }
}

void SM_s1() {
  switch (state_s1) {
    case 0:     // reset
      state_s1 = 1;
    break;

    case 1:     // start
      value_s1 = digitalRead(pin_s1);
      if (value_s1 == LOW) {state_s1 = 2;}
    break;

    case 2:     // go
      t_0_s1 = millis();
      state_s1 = 3;
    break;

    case 3:     // wait
      value_s1 = digitalRead(pin_s1);
      t_s1 = millis();

      if (value_s1 == HIGH) {state_s1 = 0;}
      if (t_s1 - t_0_s1 > bounce_delay_s1) {
        state_s1 = 5;
      }
    break;

    case 4:     // pressed
      state_s1 = 0;
    break;

    case 5:     // armed
      value_s1 = digitalRead(pin_s1);
      if (value_s1 == HIGH) {state_s1 = 4;}
  }
}

void SM_s2() {
  switch (state_s2) {
    case 0:     // reset
      state_s2 = 1;
    break;

    case 1:     // start
      value_s2 = digitalRead(pin_s2);
      if (value_s2 == LOW) {state_s2 = 2;}
    break;

    case 2:     // go
      t_0_s2 = millis();
      state_s2 = 3;
    break;

    case 3:     // wait
      value_s2 = digitalRead(pin_s2);
      t_s2 = millis();

      if (value_s2 == HIGH) {state_s2 = 0;}
      if (t_s2 - t_0_s2 > bounce_delay_s2) {
        state_s2 = 5;
      }
    break;

    case 4:     // pressed
      state_s2 = 0;
    break;

    case 5:     // armed
      value_s2 = digitalRead(pin_s2);
      if (value_s2 == HIGH) {state_s2 = 4;}
  }
}

/*
void SM_timer() {
  switch (state_timer) {
    case 0:
      
    break;
  }
}
*/