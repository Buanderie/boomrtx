#ifndef HC12_h
#define HC12_h
#include <Arduino.h>
#include <HardwareSerial.h>
#include <Stream.h>

class HC12 : public Stream
{

public:
    // HC12(uint8_t setPin, int baud);
    HC12(HardwareSerial *sws, uint8_t setPin, int baud = 9600 );
    virtual ~HC12();
    bool setOutput(HardwareSerial *output);

    virtual int available()
    {
      if( _setMode )
        return 0;
      else
        return _output->available();
    }

    virtual int read()
    {
      if( _setMode )
        return -1;
      else
        return _output->read();
    }

    virtual size_t write(const uint8_t *buffer, size_t size)
    {
      if( _setMode )
        return 0;
      else
        return _output->write( buffer, size );
    }

    virtual size_t write( uint8_t b )
    {
      if( _setMode )
        return 0;
      else
        return _output->write( b );
    }

    virtual int peek()
    {
      if( _setMode )
        return -1;
      else
        return _output->peek();
    }

private:
    uint8_t _setPin;
    int _baud;
    bool _setMode;
    HardwareSerial * _output;

};

#endif
