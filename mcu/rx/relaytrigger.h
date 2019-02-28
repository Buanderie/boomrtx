#ifndef __RELAYTRIGGER_H
#define __RELAYTRIGGER_H

#include "trigger.h"

class RelayTrigger : public Trigger
{
public:
  RelayTrigger( int relayPin  = 0 )
  :_relayPin(relayPin)
  {
    // Start deactivated
    pinMode( _relayPin, OUTPUT );
    digitalWrite( _relayPin, LOW );
  }

  virtual ~RelayTrigger()
  {

  }

  virtual void onActivation()
  {
    digitalWrite( _relayPin, HIGH );
  }

  virtual void onDeactivation()
  {
    digitalWrite( _relayPin, LOW );
  }

private:
  int _relayPin;

};

#endif
