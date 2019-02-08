#ifndef COMMCONTROLLER_H
#define COMMCONTROLLER_H

#include "frame.h"
#include "frameparser.h"
#include "circularbuffer.h"

template< size_t S >
class CommController
{
public:
    CommController()
    {

    }

    virtual ~CommController()
    {

    }

    void pushByte( uint8_t b )
    {
        if( _parser.addByte(b) )
        {
            Frame f = _parser.getFrame();
            onNewFrame( f );
        }
    }

    void pushData( uint8_t* buffer, size_t bufferSize )
    {
        for( int k = 0; k < bufferSize; ++k )
        {
            pushByte( buffer[k] );
        }
    }

    virtual void onNewFrame( Frame& f )
    {

    }

    virtual void sendData( uint8_t* data, size_t size )
    {

    }

    void sendFrame( Frame& f )
    {
        int bsize = frameToBuffer( f, buf, FRAME_MAX_PAYLOAD_SIZE + 16 );
        sendData( buf, bsize );
    }

private:
    FrameParser _parser;
    uint8_t buf[ FRAME_MAX_PAYLOAD_SIZE + 16 ];

protected:

};

#endif // COMMCONTROLLER_H
