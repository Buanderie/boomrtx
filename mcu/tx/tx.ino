
// Defines
#define TRIGGER_DURATION_MS 1000
#define QUALITY_LED_PIN 3
#define FIRE_ACK_LED_PIN 4
#define NUM_TARGET_DEVICES 2
#define USE_HC12
#define TARGET_DEVICES_EEPROM_ADDR 0x0100

#include <EEPROM.h>

// Include Scheduler since we want to manage multiple tasks.
#include "frameparser.h"
#include "linkquality.h"
#ifdef USE_HC12
#include "HC12.h"
#endif

//
#include "AceRoutine.h"
using namespace ace_routine;

// Config interface
FrameParser configFrameParser;

// Radio interface
FrameParser radioFrameParser;

// TX Device Default Settings
uint8_t __device_id = 0x00;
uint8_t __device_type = 0x00;
uint8_t __radioChannel = 0x05;
uint8_t __radioPower = 0xf0;

bool __useMechanicalTargetSelection = true;
uint8_t __targetDeviceIds[ NUM_TARGET_DEVICES ];
uint8_t __targetDeviceSlot = 0;
uint8_t __targetRelay = 0;
uint8_t __radioLinkOK = 0;
uint8_t __fireLEDON = 0;

// Link Quality measure
LinkQuality< 40 > linkQuality;

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

// Task no.1: Send PINGs into the AIIIIR
COROUTINE(pingRoutine) {
    COROUTINE_LOOP() {
        uint8_t buf[ 32 ];
        Frame pongFrame = createPingFrame( __targetDeviceIds[ __targetDeviceSlot ] );
        int bsize = frameToBuffer( pongFrame, buf, 32 );
        __radioInterface->write( buf, bsize );
        linkQuality.pushPing();
        COROUTINE_DELAY(100);
    }
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
                    uint8_t target_device_id = f.payload[ 0 ];
                    if( target_device_id == 0xff || target_device_id == __device_id )
                    {
                      Frame pongFrame = createPongFrame( __device_id, __device_type );
                      int bsize = frameToBuffer( pongFrame, buf, 32 );
                      Serial.write( buf, bsize );
                    }
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
                else if( f.opcode == OP_GET_RADIO_QUALITY )
                {
                  uint8_t qvalue = (uint8_t)(linkQuality.quality() * 255.0);
                  Frame radioQualityAckFrame = createRadioQualityAckFrame( __device_id, qvalue );
                  int bsize = frameToBuffer( radioQualityAckFrame, buf, 32 );
                  Serial.write( buf, bsize );
                }
                else if( f.opcode == OP_GET_TARGET_ID )
                {
                  uint8_t targetIdx = 0xff;
                  int slot = f.payload[ 1 ];
                  if( slot < NUM_TARGET_DEVICES )
                  {
                    targetIdx = __targetDeviceIds[ slot ];
                  }
                  Frame targetIdAckFrame = createTargetIdAckFrame( __device_id, slot, targetIdx );
                  int bsize = frameToBuffer( targetIdAckFrame, buf, 32 );
                  Serial.write( buf, bsize );
                }
                else if( f.opcode == OP_SET_TARGET_ID )
                {
                  int slot = f.payload[ 1 ];
                  uint8_t value = f.payload[ 2 ];
                  uint8_t retValue = 0xff;
                  if( slot < NUM_TARGET_DEVICES )
                  {
                    __targetDeviceIds[ slot ] = value;
                    EEPROM.write( TARGET_DEVICES_EEPROM_ADDR + slot, value );
                    retValue = __targetDeviceIds[ slot ];
                  }
                  Frame targetIdAckFrame = createTargetIdAckFrame( __device_id, slot, retValue );
                  int bsize = frameToBuffer( targetIdAckFrame, buf, 32 );
                  Serial.write( buf, bsize );
                }
                else if( f.opcode == OP_TRIGGER_FIRE )
                {
                  int output_relay = f.payload[ 0 ];
                  uint8_t value = f.payload[ 1 ];
                  double valueMs = ((double)value * 0.05) * 1000.0;
                  Frame ff = createFireFrame( __targetDeviceIds[ __targetDeviceSlot ], output_relay, valueMs );
                  int bsize = frameToBuffer( ff, buf, 32 );
                  if( !__useMechanicalTargetSelection )
                  {
                    __targetRelay = output_relay;
                  }
                  __radioInterface->write( buf, bsize );
                }
                else if( f.opcode == OP_TX_TOGGLE_MECHANICAL_TARGET_SELECTION )
                {
                  if( f.payload[ 0 ] == 0x00 )
                  {
                    __useMechanicalTargetSelection = false;
                  }
                  else if( f.payload[ 0 ] == 0x01 )
                  {
                    __useMechanicalTargetSelection = true;
                  }
                  Frame fack = createTxSelectTargetAckFrame( __targetDeviceSlot );
                  int bsize = frameToBuffer( fack, buf, 32 );
                  Serial.write( buf, bsize );
                }
                else if( f.opcode == OP_TX_SELECT_TARGET )
                {
                  uint8_t target_slot = f.payload[ 0 ];
                  if( target_slot < NUM_TARGET_DEVICES )
                  {
                    __targetDeviceSlot = target_slot;
                  }
                  Frame fack = createTxSelectTargetAckFrame( __targetDeviceSlot );
                  int bsize = frameToBuffer( fack, buf, 32 );
                  Serial.write( buf, bsize );
                }
            }
            // COROUTINE_DELAY(20);
        }
        COROUTINE_YIELD();
    }
}

// Task...
COROUTINE(checkQualityRoutine) {
    COROUTINE_LOOP() {
        if( linkQuality.quality() > 0.8 )
        {
          if( __radioLinkOK != 1 )
            digitalWrite(QUALITY_LED_PIN, HIGH);
          __radioLinkOK = 1;
        }
        else
        {
          if( __radioLinkOK != 0 )
            digitalWrite(QUALITY_LED_PIN, LOW);
          __radioLinkOK = 0;
        }
        COROUTINE_DELAY(50);
    }
}

// Task...
COROUTINE(checkSwitchesRoutine) {
    COROUTINE_LOOP() {
        if( digitalRead(12) == HIGH )
        {
          // Nothing
        }
        else
        {
          // FIRE !
          uint8_t buf[ 32 ];
          Frame ff = createFireFrame( __targetDeviceIds[ __targetDeviceSlot ], __targetRelay, TRIGGER_DURATION_MS );
          int bsize = frameToBuffer( ff, buf, 32 );
          if( __radioLinkOK == 1 )
          {
            __radioInterface->write( buf, bsize );
          }
        }

        if( digitalRead(22) == HIGH )
        {
          // TARGET 1
          // digitalWrite(4, HIGH);
          if( __useMechanicalTargetSelection )
          {
            __targetDeviceSlot = 0;
          }
        }
        else
        {
          // TARGET 2
          // digitalWrite(4, LOW);
          if( __useMechanicalTargetSelection )
          {
            __targetDeviceSlot = 1;
          }
        }

        if( digitalRead(23) == HIGH )
        {
          // RELAY 1
          if( __useMechanicalTargetSelection )
            __targetRelay = 0;
        }
        else
        {
          // RELAY 2
          if( __useMechanicalTargetSelection )
            __targetRelay = 1;
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
                if( f.opcode == OP_PONG )
                {
                  uint8_t device_id = __targetDeviceIds[ __targetDeviceSlot ];
                  if( device_id == __targetDeviceIds[ __targetDeviceSlot ] )
                  {
                    linkQuality.pushPong();
                  }
                }
                else if( f.opcode == OP_FIRE_ACK )
                {
                  uint8_t device_id = f.payload[ 0 ];
                  uint8_t relay_slot = f.payload[ 1 ];
                  uint8_t is_active = f.payload[ 2 ];
                  if( device_id == __targetDeviceIds[ __targetDeviceSlot ] )
                  {
                    // Relay to config interface...
                    int bsize = frameToBuffer( f, buf, 32 );
                    Serial.write( buf, bsize );
                    if( is_active > 0 && relay_slot == __targetRelay )
                    {
                      // Light up RED
                      if( __fireLEDON != 1 )
                        digitalWrite( FIRE_ACK_LED_PIN, HIGH );
                      __fireLEDON = 1;
                    }
                    else
                    {
                      if( is_active == 0 )
                      {
                        if( __fireLEDON != 0 )
                          digitalWrite( FIRE_ACK_LED_PIN, LOW );
                        __fireLEDON = 0;
                      }
                    }
                  }
                }
            }
        }
        COROUTINE_YIELD();
    }
}

void setup() {

    // Set-up switches
    // FIRE BUTTON
    pinMode( 12, INPUT_PULLUP);

    // TARGET SELECT
    pinMode( 22, INPUT_PULLUP);

    // RELAY SELECT
    pinMode( 23, INPUT_PULLUP);

    // Set-up LEDS
    pinMode( 3, OUTPUT );
    pinMode( 4, OUTPUT );

    // Retrieve config from EEPROM
    __device_id = EEPROM.read(0x00);
    __radioChannel = EEPROM.read(0x01);
    __radioPower = EEPROM.read(0x02);

    // Load target device IDs
    for( int i = 0; i < NUM_TARGET_DEVICES; ++i )
    {
      __targetDeviceIds[ i ] = EEPROM.read(TARGET_DEVICES_EEPROM_ADDR + i );
    }

    Serial.begin(9600);
    #ifdef USE_HC12
    // Nothing !
    #else
    Serial1.begin(9600);
    #endif

    // Setup the 3 pins as OUTPUT
    pinMode(led1, OUTPUT);

    // Setup coroutine scheduler
    CoroutineScheduler::setup();

}

void loop() {

    // Tick coroutine scheduler
    CoroutineScheduler::loop();

}
