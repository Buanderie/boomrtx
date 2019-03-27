
#include <EEPROM.h>

// Defines
#define NUM_RELAYS 2
#define USE_HC12

// Include Scheduler since we want to manage multiple tasks.
#include "frameparser.h"
#include "linkquality.h"
#include "ledtrigger.h"
#include "relaytrigger.h"

#ifdef USE_HC12
#include "HC12.h"
#endif

//
#include "AceRoutine.h"
using namespace ace_routine;

// Triggers
Trigger * __triggers[ NUM_RELAYS ];

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

#ifdef USE_HC12
HC12 __hc12( &Serial1, 2 ); // Set Pin is 2, check RX schematics
Stream * __radioInterface = &__hc12;
#else
Stream * __radioInterface = &Serial1;
#endif

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

// Task no.2: accept commands from CONFIG serial port
COROUTINE(configRoutine) {
    COROUTINE_LOOP() {
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
            }
        }
        COROUTINE_YIELD();
    }
}

// Task no.3: accept commands from RADIO serial port
COROUTINE(radioRxRoutine) {
    COROUTINE_LOOP() {
        while (__radioInterface->available()) {
            char c = __radioInterface->read();
            if( radioFrameParser.addByte( c ) )
            {
                uint8_t buf[ 32 ];
                Frame f = radioFrameParser.getFrame();
                if( f.opcode == OP_PING )
                {
                  activityBlink();
                  uint8_t target_device_id = f.payload[ 0 ];
                  if( target_device_id == 0xff || target_device_id == __device_id )
                  {
                    activityBlink();
                    Frame pongFrame = createPongFrame( __device_id, __device_type );
                    int bsize = frameToBuffer( pongFrame, buf, 32 );
                    __radioInterface->write( buf, bsize );

                  }
                }
                else if( f.opcode == OP_FIRE )
                {
                  uint8_t target_device_id = f.payload[ 0 ];
                  if( target_device_id == __device_id )
                  {
                    uint8_t relay_idx = f.payload[ 1 ];
                    if( relay_idx < NUM_RELAYS )
                    {
                      uint8_t dvalue = f.payload[ 2 ];
                      double dmillis = ((double)dvalue * 0.05) * 1000.0;
                      Trigger * trig = __triggers[ relay_idx ];
                      trig->trigger( dmillis );
                    }
                  }
                }
            }
        }
        COROUTINE_YIELD();
    }
}

COROUTINE(triggerTickRoutine) {
    COROUTINE_LOOP() {
      uint8_t buf[ 32 ];
      for( int i = 0; i < NUM_RELAYS; ++i )
      {
        Trigger * t = __triggers[ i ];
        t->tick();
        /*
        Frame fack = createFireAckFrame( __device_id, (uint8_t)i, t->isActive() );
        int bsize = frameToBuffer( fack, buf, 32 );
        __radioInterface->write( buf, bsize );
        */
      }
      COROUTINE_DELAY(50);
    }
}

/*
COROUTINE(debug) {
    COROUTINE_LOOP() {
      activityBlink();
      COROUTINE_DELAY(150);
    }
}
*/

void setup() {

    // Initialize triggers
    const int relayPinOffset = 21; // See wiring schematics for RX side
    for( int i = 0; i < NUM_RELAYS; ++i )
    {
      __triggers[ i ] = new RelayTrigger( i + relayPinOffset );
    }

    // Retrieve config from EEPROM
    __device_id = EEPROM.read(0x00);
    __radioChannel = EEPROM.read(0x01);
    __radioPower = EEPROM.read(0x02);

    Serial.begin(9600);

    #ifdef USE_HC12
    // Nothing !
    #else
    Serial1.begin(9600);
    #endif

    // Setup the 3 pins as OUTPUT
    pinMode(led1, OUTPUT);

    CoroutineScheduler::setup();

}

void loop() {
    CoroutineScheduler::loop();
}
