#ifndef __LEDTRIGGER_H
#define __LEDTRIGGER_H

#include "trigger.h"

class LEDTrigger : public Trigger
{
public:
  LEDTrigger( int ledPin )
  :_ledPin(ledPin)
  {

  }

  virtual ~LEDTrigger()
  {

  }

  virtual void onActivation()
  {
    digitalWrite( _ledPin, HIGH );
  }

  virtual void onDeactivation()
  {
    digitalWrite( _ledPin, LOW );
  }

private:
  int _ledPin;

};

#endif
