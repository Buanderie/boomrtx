
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

#include <EEPROM.h>

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
uint8_t __radioChannel = 0x05;
uint8_t __radioPower = 0xf0;

int led1 = LED_BUILTIN; // more portable
int needBlink = 1;
bool ledState = false;

#define RECEIVER

void setup() {

  // Retrieve config from EEPROM
  __device_id = EEPROM.read(0x00);
  __radioChannel = EEPROM.read(0x01);
  __radioPower = EEPROM.read(0x02);
  
  Serial.begin(9600);
  Serial1.begin(9600);
  
  // Setup the 3 pins as OUTPUT
  pinMode(led1, OUTPUT);

  // Add "loop2" and "loop3" to scheduling.
  // "loop" is always started by default.
  Scheduler.startLoop(loop2);
  Scheduler.startLoop(loop3);
  Scheduler.startLoop(loop4);
  
}

// Task no.1: Send PINGs into the AIIIIR
void loop() {

  uint8_t buf[ 32 ];
  Frame pongFrame = createPingFrame( __device_type );
  int bsize = frameToBuffer( pongFrame, buf, 32 );
  Serial1.write( buf, bsize );
  Scheduler.delay(100);
  yield();
  
}

// Task no.2: blink LED with 0.1 second delay.
void loop2() {

  Scheduler.delay(1000);
  needBlink = 0;
  Scheduler.delay(1000);  
  needBlink = 1;
}

void activityBlink()
{
      if( needBlink == 0 )
      {
        digitalWrite( led1, LOW );
        return;
      }
        
      ledState = !ledState;
      if( ledState )
        digitalWrite(led1, HIGH);
      else
        digitalWrite(led1, LOW);
}

// Task no.3: accept commands from CONFIG serial port
void loop3() {
  
while (Serial.available()) {
    char c = Serial.read();
    if( configFrameParser.addByte( c ) )
    {
      uint8_t buf[ 32 ];
      Frame f = configFrameParser.getFrame();
      // If received PING, send PONG
      if( f.opcode == OP_PING )
      {
        Frame pongFrame = createPongFrame( __device_id, __device_type );
        int bsize = frameToBuffer( pongFrame, buf, 32 );
        Serial.write( buf, bsize );
        // activityBlink();
      }
      else if( f.opcode == OP_SET_DEVICE_ID )
      {
        __device_id = f.payload[0];
        EEPROM.write( 0x00, __device_id );
        // activityBlink();
      }
      else if( f.opcode == OP_GET_RADIO_CHANNEL )
      {
        Frame radioChannelAckFrame = createRadioChannelAckFrame( __device_id, __radioChannel );
        int bsize = frameToBuffer( radioChannelAckFrame, buf, 32 );
        Serial.write( buf, bsize );
        // activityBlink();
      }
      else if( f.opcode == OP_SET_RADIO_CHANNEL )
      {
        __radioChannel = f.payload[1];
        EEPROM.write( 0x01, __radioChannel );
        Frame radioChannelAckFrame = createRadioChannelAckFrame( __device_id, __radioChannel );
        int bsize = frameToBuffer( radioChannelAckFrame, buf, 32 );
        Serial.write( buf, bsize );
        // activityBlink();
      }
      else if( f.opcode == OP_GET_RADIO_POWER )
      {
        Frame radioPowerAckFrame = createRadioPowerAckFrame( __device_id, __radioPower );
        int bsize = frameToBuffer( radioPowerAckFrame, buf, 32 );
        Serial.write( buf, bsize );
        // activityBlink();
      }
      else if( f.opcode == OP_SET_RADIO_POWER )
      {
        __radioPower = f.payload[1];
        EEPROM.write( 0x02, __radioPower );
        Frame radioPowerAckFrame = createRadioPowerAckFrame( __device_id, __radioPower );
        int bsize = frameToBuffer( radioPowerAckFrame, buf, 32 );
        Serial.write( buf, bsize );
        // activityBlink();
      }
    yield();
  }
}
yield();
}

// Task no.3: accept commands from CONFIG serial port
void loop4() {

while (Serial1.available()) {
    // activityBlink();
    char c = Serial1.read();
    if( configFrameParser.addByte( c ) )
    {
      Frame f = configFrameParser.getFrame();
      if( f.opcode == OP_PONG )
      {
        digitalWrite(led1, HIGH);
      }
    }
    Serial1.write( c );
}
Scheduler.delay(100);
// yield();
}
