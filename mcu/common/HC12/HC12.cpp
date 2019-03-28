#include <Arduino.h>
#include "HC12.h"

HC12::HC12(HardwareSerial *sws, uint8_t setPin, int baud )
:_baud(baud), _setPin(setPin), _setMode(true)
{
  this->setOutput(sws);
  pinMode(_setPin, OUTPUT);
  enableSetMode( false );
  this->_output->begin(_baud);
}

HC12::~HC12()
{

}

bool HC12::setOutput(HardwareSerial *sws)
{
  this->_output = sws;
  return true;
}
