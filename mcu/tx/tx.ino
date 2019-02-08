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
#include "frameparser.h"

// 

// Config interface
FrameParser configFrameParser;

// Radio interface
FrameParser radioFrameParser;

// TX Device Default Settings
uint8_t __device_id = 0x01;
uint8_t __device_type = 0x01;
uint8_t __radioChannel = 0x00;
uint8_t __radioPower = 0x00;

int led1 = LED_BUILTIN; // more portable
int needBlink = 1;

void setup() {
  Serial.begin(9600);

  // Setup the 3 pins as OUTPUT
  pinMode(led1, OUTPUT);

  // Add "loop2" and "loop3" to scheduling.
  // "loop" is always started by default.
  Scheduler.startLoop(loop2);
  Scheduler.startLoop(loop3);
}

// Task no.1: blink LED with 1 second delay.
void loop() {
  // needBlink = 1;
  Scheduler.delay(1000);
  // needBlink = 0;
  Scheduler.delay(1000);
}

// Task no.2: blink LED with 0.1 second delay.
void loop2() {
  if( needBlink == 1)
  {
  digitalWrite(led1, HIGH);
  Scheduler.delay(50);
  digitalWrite(led1, LOW);
  Scheduler.delay(50);
  }
  else
  {
    yield();
  }
}

// Task no.3: accept commands from Serial port
// '0' turns off LED
// '1' turns on LED
void loop3() {
if (Serial.available()) {
    char c = Serial.read();
    if( configFrameParser.addByte( c ) )
    {
      Frame f = configFrameParser.getFrame();

      // If received PING, send PONG
      if( f.opcode == 0x00 )
      {
        uint8_t buf[ 32 ];
        Frame pongFrame = createPongFrame( __device_id, __device_type );
        int bsize = frameToBuffer( pongFrame, buf, 32 );
        Serial.write( buf, bsize );
        yield();
      }
    }
}
  yield();
}
