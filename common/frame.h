#ifndef FRAME_H__
#define FRAME_H__

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#include "crc16.h"
#include "protocol.h"

#define FRAME_START_SYMBOL 0xBE
#define FRAME_END_SYMBOL 0xEF
#define FRAME_MAX_PAYLOAD_SIZE 255

#define FRAME_ENCRYPTION_FLAG 0x10

typedef struct
{
    uint8_t		flags;
    uint8_t 	opcode;
    uint8_t 	payload_size;
    uint8_t 	payload[ FRAME_MAX_PAYLOAD_SIZE ];
    uint16_t  	crc;
} Frame;

Frame createFrame( uint8_t opcode, uint8_t flags, uint8_t* payload, size_t payload_size, bool encryption = false )
{
    Frame ret;
    ret.opcode = opcode;
    ret.flags = flags;
    if( payload_size <= FRAME_MAX_PAYLOAD_SIZE )
    {
        ret.payload_size = payload_size;
        memcpy( ret.payload, payload, ret.payload_size );
    }
    else
    {
        ret.payload_size = 0;
    }

    if( encryption )
    {
        ret.flags |= FRAME_ENCRYPTION_FLAG;
    }
    return ret;
}

Frame createPingFrame( uint8_t target_device_id = 0xff, uint8_t flags = 0x00 )
{
    Frame ret;
    ret.flags = flags;
    ret.payload_size = 1;
    ret.payload[ 0 ] = target_device_id;
    ret.opcode = OP_PING;
    return ret;
}

Frame createPongFrame( uint8_t device_id, uint8_t device_type, uint8_t flags = 0x00 )
{
    Frame ret;
    ret.flags = flags;
    ret.payload_size = 2;
    ret.payload[ 0 ] = device_id;
    ret.payload[ 1 ] = device_type;
    ret.opcode = OP_PONG;
    return ret;
}

Frame createSetDeviceIdFrame( uint8_t device_id, uint8_t flags = 0x00 )
{
    Frame ret;
    ret.flags = flags;
    ret.payload_size = 1;
    ret.payload[ 0 ] = device_id;
    ret.opcode = OP_SET_DEVICE_ID;
    return ret;
}

Frame createGetRadioChannelFrame( uint8_t device_id, uint8_t flags = 0x00 )
{
    Frame ret;
    ret.flags = flags;
    ret.payload_size = 1;
    ret.payload[ 0 ] = device_id;
    ret.opcode = OP_GET_RADIO_CHANNEL;
    return ret;
}

Frame createSetDeviceChannelFrame( uint8_t device_id, uint8_t radio_channel, uint8_t flags = 0x00 )
{
    Frame ret;
    ret.flags = flags;
    ret.payload_size = 2;
    ret.payload[ 0 ] = device_id;
    ret.payload[ 1 ] = radio_channel;
    ret.opcode = OP_SET_RADIO_CHANNEL;
    return ret;
}

Frame createRadioChannelAckFrame( uint8_t device_id, uint8_t radio_channel, uint8_t flags = 0x00 )
{
    Frame ret;
    ret.flags = flags;
    ret.payload_size = 2;
    ret.payload[ 0 ] = device_id;
    ret.payload[ 1 ] = radio_channel;
    ret.opcode = OP_RADIO_CHANNEL_ACK;
    return ret;
}

Frame createGetRadioPowerFrame( uint8_t device_id, uint8_t flags = 0x00 )
{
    Frame ret;
    ret.flags = flags;
    ret.payload_size = 1;
    ret.payload[ 0 ] = device_id;
    ret.opcode = OP_GET_RADIO_POWER;
    return ret;
}

Frame createSetDevicePowerFrame( uint8_t device_id, uint8_t radio_power, uint8_t flags = 0x00 )
{
    Frame ret;
    ret.flags = flags;
    ret.payload_size = 2;
    ret.payload[ 0 ] = device_id;
    ret.payload[ 1 ] = radio_power;
    ret.opcode = OP_SET_RADIO_POWER;
    return ret;
}

Frame createRadioPowerAckFrame( uint8_t device_id, uint8_t radio_power, uint8_t flags = 0x00 )
{
    Frame ret;
    ret.flags = flags;
    ret.payload_size = 2;
    ret.payload[ 0 ] = device_id;
    ret.payload[ 1 ] = radio_power;
    ret.opcode = OP_RADIO_POWER_ACK;
    return ret;
}

Frame createGetRadioQualityFrame( uint8_t device_id, uint8_t flags = 0x00 )
{
    Frame ret;
    ret.flags = flags;
    ret.payload_size = 1;
    ret.payload[ 0 ] = device_id;
    ret.opcode = OP_GET_RADIO_QUALITY;
    return ret;
}

Frame createRadioQualityAckFrame( uint8_t device_id, uint8_t quality, uint8_t flags = 0x00 )
{
    Frame ret;
    ret.flags = flags;
    ret.payload_size = 2;
    ret.payload[ 0 ] = device_id;
    ret.payload[ 1 ] = quality;
    ret.opcode = OP_RADIO_QUALITY_ACK;
    return ret;
}

Frame createGetTargetIdFrame( uint8_t device_id, uint8_t target_slot_idx, uint8_t flags = 0x00 )
{
    Frame ret;
    ret.flags = flags;
    ret.payload_size = 2;
    ret.payload[ 0 ] = device_id;
    ret.payload[ 1 ] = target_slot_idx;
    ret.opcode = OP_GET_TARGET_ID;
    return ret;
}

Frame createSetTargetIdFrame( uint8_t device_id, uint8_t target_slot_idx, uint8_t target_device_id, uint8_t flags = 0x00 )
{
    Frame ret;
    ret.flags = flags;
    ret.payload_size = 3;
    ret.payload[ 0 ] = device_id;
    ret.payload[ 1 ] = target_slot_idx;
    ret.payload[ 2 ] = target_device_id;
    ret.opcode = OP_SET_TARGET_ID;
    return ret;
}

Frame createTargetIdAckFrame( uint8_t device_id, uint8_t target_slot_idx, uint8_t target_device_id, uint8_t flags = 0x00 )
{
    Frame ret;
    ret.flags = flags;
    ret.payload_size = 3;
    ret.payload[ 0 ] = device_id;
    ret.payload[ 1 ] = target_slot_idx;
    ret.payload[ 2 ] = target_device_id;
    ret.opcode = OP_TARGET_ID_ACK;
    return ret;
}

Frame createDebugMsgFrame( uint8_t device_id, const char* msg, size_t msg_len, uint8_t flags = 0x00 )
{
  Frame ret;
  ret.flags = flags;
  size_t msize = MIN( msg_len, 250 );
  ret.payload_size = msize + 1;
  int offset = 0;
  ret.payload[ offset++ ] = device_id;
  for( int i = 0; i < msize; ++i )
  {
    ret.payload[ offset++ ] = msg[i];
  }
  ret.payload[ offset++ ] = 0x00;
  ret.opcode = OP_DEBUG_MESSAGE;
  return ret;
}

Frame createFireFrame( uint8_t device_id, uint8_t output_relay_idx, double durationMilliseconds, uint8_t flags = 0x00 )
{
  Frame ret;
  ret.flags = flags;
  ret.payload_size = 3;
  ret.payload[ 0 ] = device_id;
  ret.payload[ 1 ] = output_relay_idx;
  uint8_t dvalue = (uint8_t)((durationMilliseconds / 1000.0) / 0.05);
  ret.payload[ 2 ] = dvalue;
  ret.opcode = OP_FIRE;
  return ret;
}

Frame createFireAckFrame( uint8_t device_id, uint8_t output_relay_idx, bool activated, uint8_t flags = 0x00 )
{
  Frame ret;
  ret.flags = flags;
  ret.payload_size = 3;
  ret.payload[ 0 ] = device_id;
  ret.payload[ 1 ] = output_relay_idx;
  uint8_t state = 0x00;
  if( activated )
    state = 0x01;
  ret.payload[ 2 ] = state;
  ret.opcode = OP_FIRE_ACK;
  return ret;
}

Frame createTriggerFireFrame( uint8_t output_relay_idx, double durationMilliseconds, uint8_t flags = 0x00 )
{
  Frame ret;
  ret.flags = flags;
  ret.payload_size = 2;
  ret.payload[ 0 ] = output_relay_idx;
  uint8_t dvalue = (uint8_t)((durationMilliseconds / 1000.0) / 0.05);
  ret.payload[ 1 ] = dvalue;
  ret.opcode = OP_TRIGGER_FIRE;
  return ret;
}

Frame createTxToggleMechanicalSelectionFrame( bool value, uint8_t flags = 0x00 )
{
  Frame ret;
  ret.flags = flags;
  ret.payload_size = 1;
  if( value )
    ret.payload[ 0 ] = 1;
  else
    ret.payload[ 0 ] = 0;
  ret.opcode = OP_TX_TOGGLE_MECHANICAL_TARGET_SELECTION;
  return ret;
}

Frame createTxSelectTargetFrame( uint8_t target_idx, uint8_t flags = 0x00 )
{
  Frame ret;
  ret.flags = flags;
  ret.payload_size = 1;
  ret.payload[ 0 ] = target_idx;
  ret.opcode = OP_TX_SELECT_TARGET;
  return ret;
}

  Frame createTxSelectTargetAckFrame( uint8_t target_idx, uint8_t flags = 0x00 )
{
  Frame ret;
  ret.flags = flags;
  ret.payload_size = 1;
  ret.payload[ 0 ] = target_idx;
  ret.opcode = OP_TX_SELECT_TARGET_ACK;
  return ret;
}

size_t frameToBuffer( Frame& f, uint8_t* buffer, size_t bufferSize )
{
    int offset = 0;
    int sizeRequirement = 7 + f.payload_size;
    if( bufferSize < sizeRequirement )
    {
        // Fuck
    }
    else
    {
        buffer[ offset++ ] = FRAME_START_SYMBOL;
        buffer[ offset++ ] = f.flags;
        buffer[ offset++ ] = f.opcode;
        buffer[ offset++ ] = f.payload_size;
        for( int k = 0; k < f.payload_size; ++k )
        {
            buffer[ offset++ ] = f.payload[k];
        }

        // Compute CRC
        CRC16 crc;
        uint16_t compCRC = crc.processBuffer( (const char*)(buffer + 2), f.payload_size + 2 );
        memcpy( buffer + offset, &compCRC, sizeof(uint16_t) );
        offset += sizeof(uint16_t);

        buffer[ offset++ ] = FRAME_END_SYMBOL;

        return offset;

    }
    return 0;
}

#endif
