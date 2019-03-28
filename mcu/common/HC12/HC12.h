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

    void enableSetMode( bool value )
    {
      if( value != _setMode )
      {
        if( !value )
          digitalWrite(_setPin, HIGH); // SET MODE = OFF
        else
          digitalWrite(_setPin, LOW); // SET MODE = OFF
        delay(100);
        _setMode = value;
      }
    }

    void setChannel( int value )
    {
      enableSetMode( true );
      char tbuffer[ 16 ];
      int r = snprintf(tbuffer, 1023, "AT+C%03d", 1);
      _output->write( tbuffer, r );

      delay(500);
      while (_output->available() > 0) {}

      enableSetMode( false );
    }

    void setPowerLevel( int value )
    {
      enableSetMode( true );
      char tbuffer[ 16 ];
      int r = snprintf(tbuffer, 1023, "AT+P%01d", 1);
      _output->write( tbuffer, r );

      delay(500);
      while (_output->available() > 0) {}

      enableSetMode( false );
    }

    int getChannel()
    {
      enableSetMode( true );

      /*
      char tbuffer[ 16 ];
      int r = snprintf(tbuffer, 1023, "AT+P%01d", 1);
      _output->write( tbuffer, r );

      delay(500);
      while (_output->available() > 0) {}
      */

      enableSetMode( false );
    }

    int getPowerLevel()
    {
      enableSetMode( true );

      /*
      char tbuffer[ 16 ];
      int r = snprintf(tbuffer, 1023, "AT+P%01d", 1);
      _output->write( tbuffer, r );

      delay(500);
      while (_output->available() > 0) {}
      */
      
      enableSetMode( false );
    }

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
