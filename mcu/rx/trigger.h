#ifndef __TRIGGER_H
#define __TRIGGER_H

class Trigger
{
public:

  typedef enum
  {
    ACTIVE = 0,
    INACTIVE
  } TriggerState;

  Trigger()
  :_triggerStart(0), _triggerDuration(0), _state(INACTIVE)
  {

  }

  virtual ~Trigger()
  {

  }

  void trigger( unsigned long durationMilliseconds )
  {
    _triggerStart = millis();
    _triggerDuration = durationMilliseconds;
    _state = ACTIVE;
    onActivation();
  }

  void tick()
  {
    unsigned long dt = (millis() - _triggerStart);
    if( dt >= _triggerDuration && _state == ACTIVE )
    {
      _state = INACTIVE;
      onDeactivation();
    }
  }

  bool isActive()
  {
    return (_state == ACTIVE);
  }

  virtual void onActivation()
  {

  }

  virtual void onDeactivation()
  {

  }

private:
  unsigned long _triggerStart;
  unsigned long _triggerDuration;
  TriggerState _state;

protected:

};

#endif
