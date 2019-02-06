#ifndef FRAMEPARSER_H__
#define FRAMEPARSER_H__

#include "frame.h"
#include "crc16.h"

class FrameParser
{
	public:
		typedef enum
		{
			WAITING_START_SYMBOL = 0,
			WAITING_FLAGS,
			WAITING_OPCODE,
			WAITING_PAYLOAD_SIZE,
			WAITING_PAYLOAD,
			WAITING_CRC,
			WAITING_END_SYMBOL
		} State;

		FrameParser()
		{
				_curState = WAITING_START_SYMBOL;
				_curBufferOffset = 0;
		}

		virtual ~FrameParser()
		{

		}

		bool addByte( uint8_t b )
		{
			bool ret = false;
			switch( _curState )
			{
				case WAITING_START_SYMBOL:
				{
					if( b == FRAME_START_SYMBOL )
					{
						_curState = WAITING_FLAGS;
					}
					break;
				}

				case WAITING_FLAGS:
				{
					_curFrame.flags = b;
					_curBufferOffset = 0;
					_curState = WAITING_OPCODE;
					break;
				}

				case WAITING_OPCODE:
				{
					_curFrame.opcode = b;
					_buffer[ _curBufferOffset ] = b;
					_curBufferOffset++;
					_curState = WAITING_PAYLOAD_SIZE;
					break;
				}

				case WAITING_PAYLOAD_SIZE:
				{
					_curFrame.payload_size;
					_buffer[ _curBufferOffset ] = b;
					_curBufferOffset++;
					_curState = WAITING_PAYLOAD;
					_curPayloadOffset = 0;
					break;
				}

				case WAITING_PAYLOAD:
				{
					_buffer[ _curBufferOffset ] = b;
					_curBufferOffset++;
					_curFrame.payload[ _curPayloadOffset ] = b;
					_curPayloadOffset++;
					if( _curPayloadOffset >= _curFrame.payload_size )
					{
						_curState = WAITING_CRC;
						_curCRCOffset = 0;
					}
					break;
				}

				case WAITING_CRC:
				{
					_crcBuffer[ _curCRCOffset ] = b;
					_curCRCOffset++;
					if( _curCRCOffset >= sizeof(uint16_t) )
					{
						bool isCRCValid = false;
						// Check CRC
						if( _curBufferOffset > 0 )
						{
							uint16_t compCRC = _crc.processBuffer( (const char*)_buffer, _curBufferOffset );
							uint16_t recCRC;
							memcpy( &recCRC, _crcBuffer, sizeof(uint16_t) );
							if( recCRC == compCRC )
							{
								isCRCValid = true;
							}
						}
						else
						{
							isCRCValid = false;
						}

						if( isCRCValid )
						{
							// SUCCESS
							_curState = WAITING_END_SYMBOL;
						}
						else
						{
							// FAILURE
							_curState = WAITING_START_SYMBOL;
						}
						
					}
					break;
				}

				case WAITING_END_SYMBOL:
				{
					if( b == FRAME_END_SYMBOL )
					{
						ret = true;
						_curState = WAITING_START_SYMBOL;
					}
					break;
				}

				default:
					break;

			}
				return ret;
		}

		Frame getFrame()
		{
			return _curFrame;
		}

	private:
		State _curState;
		Frame _curFrame;
		int		_curPayloadOffset;

		uint8_t _crcBuffer[ 2 ];
		int _curCRCOffset;

		CRC16 _crc;

		uint8_t _buffer[ FRAME_MAX_PAYLOAD_SIZE + 8 ];
		int _curBufferOffset;

};

#endif
