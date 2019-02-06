#ifndef FRAME_H__
#define FRAME_H__

#include "crc16.h"

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

typedef enum
{
    OP_PING = 0
} OpCode;

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

Frame createPingFrame()
{
    Frame ret;
    ret.payload_size = 0;
    ret.opcode = OP_PING;
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
}

#endif
