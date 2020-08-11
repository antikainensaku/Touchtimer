#include <Arduino.h>
#include <U8g2lib.h>
#include <SPI.h>
#include <Wire.h>
#include <CapacitiveSensor.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);     // setting up correct display for u8g2 library

// variables for reset switch
const int pin_reset_s = 4;
int value_reset_s = 0;

// statemachine variables for switch 1
CapacitiveSensor cap_s1 = CapacitiveSensor(5,6);
int state_s1 = 0;
int release_s1 = 0;
long value_s1 = 0;
unsigned long t_s1 = 0;
unsigned long t_0_s1 = 0;

// statemachine variables for switch 2
CapacitiveSensor cap_s2 = CapacitiveSensor(8,7);
int state_s2 = 0;
int release_s2 = 0;
long value_s2 = 0;
unsigned long t_s2 = 0;
unsigned long t_0_s2 = 0;

// statemachine variables for timer
int state_timer = 0;
unsigned long time = 0;
unsigned long mseconds = 0;
unsigned long seconds = 0;
unsigned long minutes = 0;
unsigned long t_timer = 0;
unsigned long t_0_timer = 0;

// other variables
const unsigned long debounce_time = 5;   // debounce time in milliseconds
const long switch_limit = 500;           // value for capacitive sensor reading low vs high

void setup() {
  Serial.begin(9600);
  u8g2.begin();
  u8g2.setFont(u8g2_font_logisoso32_tr);
  pinMode(pin_reset_s, INPUT_PULLUP);
}

void loop() {
  SM_s1();
  SM_s2();
  SM_timer();
  if (state_s1 == 4) {
    ++release_s1;       // adds +1 to release_s1 variable (when button 1 is released)
  }
  if (state_s2 == 4) {
    ++release_s2;       // adds +1 to release_s2 variable (when button 2 is released)
  }
}

void SM_s1() {
  switch (state_s1) {
    case 0:     // reset
      state_s1 = 1;
    break;

    case 1:     // start 
      value_s1 = cap_s1.capacitiveSensor(30);
      if (value_s1 > switch_limit) {
        Serial.println("S1 touched");
        state_s1 = 2;}    // checks if the switch is pressed (LOW)
    break;

    case 2:     // debounce start time
      t_0_s1 = millis();                      // milliseconds timestamp saved to t_0_s1
      state_s1 = 3;
    break;

    case 3:     // debounce check
      value_s1 = cap_s1.capacitiveSensor(30);
      t_s1 = millis();                        // second timestamp saved to t_s1

      if (value_s1 < switch_limit) {state_s1 = 0;}   // going back to case 0 (reset) if button is depressed
      if (t_s1 - t_0_s1 > debounce_time) {     // moving to case 5 (button armed) if the switch state
        state_s1 = 5;                         // stays pressed (LOW) for longer than the debounce_time (5ms)
      }
    break;

    case 4:     // released
      state_s1 = 0;
    break;

    case 5:     // armed
      value_s1 = cap_s1.capacitiveSensor(30);
      if (value_s1 < switch_limit) {state_s1 = 4;}   // if the switch state changes to not pressed (HIGH) going to case 4 (button released)
  }
}

void SM_s2() {
  switch (state_s2) {
    case 0:     // reset
      state_s2 = 1;
    break;

    case 1:     // start
      value_s2 = cap_s2.capacitiveSensor(30);
      if (value_s2 > switch_limit) {
        Serial.println("S2 touched");
        state_s2 = 2;}    // checks if the switch is pressed (LOW)
    break;

    case 2:     // debounce start time
      t_0_s2 = millis();                      // milliseconds timestamp saved to t_0_s1
      state_s2 = 3;
    break;

    case 3:     // debounce check
      value_s2 = cap_s2.capacitiveSensor(30);
      t_s2 = millis();                        // second timestamp saved to t_s1

      if (value_s2 < switch_limit) {state_s2 = 0;}   // going back to case 0 (reset) if button is depressed
      if (t_s2 - t_0_s2 > debounce_time) {     // moving to case 5 (button armed) if the switch state
        state_s2 = 5;                         // stays pressed (LOW) for longer than the debounce_time (5ms)
      }
    break;

    case 4:     // released
      state_s2 = 0;
    break;

    case 5:     // armed
      value_s2 = cap_s2.capacitiveSensor(30);
      if (value_s2 < switch_limit) {state_s2 = 4;}   // if the switch state changes to not pressed (HIGH) going to case 4 (button released)
  }
}

void SM_timer() {
  switch (state_timer) {
    case 0:       // reset
      u8g2.clearBuffer();                         // clears the internal memory (buffer)
      state_timer = 1;
    break;

    case 1:       // start
      if (release_s1 <= 0 || release_s2 <= 0) {   // if one (or both) of the buttons haven't been released, timer displays 00:00;00
        time = 0;
        t_0_timer = millis();
        if (state_s1 == 5 || state_s2 == 5) {
          state_timer = 3;
        }
        if (state_s1 != 5 && state_s2 != 5) {     // if both of the buttons are depressed (or one of them just released)
          release_s1 = 0;                         // release counter for switch 1 & 2 is changed to 0
          release_s2 = 0;
          state_timer = 3;
        }
      }
      if (release_s1 >= 1 && release_s2 >= 1) {   // if both buttons have been released, goes to case 2 (the timer starts)
        state_timer = 2;
      }
    break;

    case 2:       // timer started
      t_timer = millis();
      time = t_timer - t_0_timer;
      if (state_s1 != 5 || state_s2 != 5) {       // while one (or both) of the buttons state is not pressed, timer continues to run
        state_timer = 3;
      }
      if (state_s1 == 5 && state_s2 == 5) {       // if both buttons pressed, goes to case 7 (stops timer and displays it)
        state_timer = 7;
      }
    break;

    case 3:     // calculates mseconds
      mseconds = time / 10;
      if (mseconds >= 100) {
        mseconds -= 100 * (time / 1000);          // keeps milliseconds a 2 digit number
      }
      u8g2.setCursor (90, 32);                    // sets "starting point" for milliseconds digits
      u8g2.print(u8x8_u16toa(mseconds, 2));       // draws milliseconds to that point (in memory)
      u8g2.setCursor (81, 32);                
      u8g2.print(";");                            // sets "starting point" and draws divider for seconds and milliseconds
      state_timer = 4;
    break;

    case 4:     // calculates seconds
      seconds = (time / 1000);
      if (seconds >= 60) {
        seconds -= 60 * ((time / 1000) / 60);     // keeps seconds under 60
      }
      u8g2.setCursor (44, 32);                
      u8g2.print(u8x8_u16toa(seconds, 2));        // same formula repeats itself with seconds
      u8g2.setCursor (36, 32);
      u8g2.print(":");
      state_timer = 5;
    break;

    case 5:     // calculates minutes
      minutes = ((time / 1000) / 60);
      u8g2.setCursor (0, 32);                
      u8g2.print(u8x8_u16toa(minutes, 2));        // same formula repeats itself with minutes
      state_timer = 6;
    break;

    case 6:     // sendBuffer (draws to display)
      u8g2.sendBuffer();                          // sends internal memory (buffer) to the display
      state_timer = 0;
    break;

    case 7:     // show end result
      value_reset_s = digitalRead(pin_reset_s);
      if (value_reset_s == HIGH) {}                   // does nothing (stops time and shows it on display)
      if (value_reset_s == LOW) {state_timer = 8;}    // going to case 8 (reset to 0) when reset button is pressed (LOW)
    break;

    case 8:     // reset to 0
      release_s1 = 0;
      release_s2 = 0;                             // setting button release counters to 0 for switch 1 & 2
      state_timer = 0;
    break;
  }
}
