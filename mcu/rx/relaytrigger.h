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
    digitalWrite( _relayPin, LOW ); // WARNING: Start ON when LOW
  }

  virtual ~RelayTrigger()
  {

  }

  virtual void onActivation()
  {
    digitalWrite( _relayPin, LOW );
  }

  virtual void onDeactivation()
  {
    digitalWrite( _relayPin, HIGH );
  }

private:
  int _relayPin;

};

#endif
