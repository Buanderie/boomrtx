/*
 Multiple Blinks

 Demonstrates the use of the Scheduler library for the Arduino Due
 
 Hardware required :
 * LEDs connected to pins 11, 12, and 13

 created 8 Oct 2012
 by Cristian Maglie
 Modified by 
 Scott Fitzgerald 19 Oct 2012
  
 This example code is in the public domain
 
 http://arduino.cc/en/Tutorial/MultipleBlinks

 Modified by Fabrice Oudert 8 Jan 2013 
 https://code.google.com/p/arduino-scoop-cooperative-scheduler-arm-avr/
 
 */

// Include Scheduler since we want to manage multiple tasks.
#include "SchedulerARMAVR.h"

#define HWSERIAL Serial1

// Teensy 2.0 has the LED on pin 11
// Teensy++ 2.0 has the LED on pin 6
// Teensy 3.x / Teensy LC have the LED on pin 13
const int ledPin = 6;

void setup() {

  pinMode(ledPin, OUTPUT);
  Serial.begin(9600);

  // Add "loop2" and "loop3" to scheduling.
  // "loop" is always started by default.
  Scheduler.startLoop(loop2);
  Scheduler.startLoop(loop3);
}

// Task no.1: blink LED with 1 second delay.
void loop() {
  Scheduler.delay(1000);
}

// Task no.2: blink LED with 0.1 second delay.
void loop2() {
  digitalWrite(ledPin, HIGH);   // set the LED on
  delay(1000);                  // wait for a second
  digitalWrite(ledPin, LOW);    // set the LED off
  delay(1000);                  // wait for a second
}

// Task no.3: accept commands from Serial port
// '0' turns off LED
// '1' turns on LED
void loop3() {

        int incomingByte;
        
  if (Serial.available() > 0) {
    incomingByte = Serial.read();
    Serial.print("USB received: ");
    Serial.println(incomingByte, DEC);
    HWSERIAL.print("USB received:");
    HWSERIAL.println(incomingByte, DEC);
  }
  if (HWSERIAL.available() > 0) {
    incomingByte = HWSERIAL.read();
    Serial.print("UART received: ");
    Serial.println(incomingByte, DEC);
    HWSERIAL.print("UART received:");
    HWSERIAL.println(incomingByte, DEC);
  }

  // IMPORTANT:
  // We must call 'yield' at a regular basis to pass
  // control to other tasks.
  yield();
}
