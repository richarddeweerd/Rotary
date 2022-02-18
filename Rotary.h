/////////////////////////////////////////////////////////////////
/*
  Based on code from Lennart Hennigs
  ESP/Arduino Library for reading rotary encoder values.
  Copyright 2017-2021 Lennart Hennigs
*/
/////////////////////////////////////////////////////////////////
#pragma once

#ifndef Rotary_h
#define Rotary_h

/////////////////////////////////////////////////////////////////

#include "Arduino.h"

/////////////////////////////////////////////////////////////////

#define RE_RIGHT 1
#define RE_LEFT 255

#define TIME_LIST_LENGTH 4
/////////////////////////////////////////////////////////////////
class RotaryCounter
{
private:
  int _lower_bound;
  int _upper_bound;
  byte _steps_per_click;
  bool _roll_over;
  int _step_counter;
  int _last_position;
  int _position;
  byte direction;

  typedef void (*CallbackFunction)(RotaryCounter &);
  CallbackFunction change_cb = NULL;
  CallbackFunction right_cb = NULL;
  CallbackFunction left_cb = NULL;
  CallbackFunction lower_cb = NULL;
  CallbackFunction upper_cb = NULL;

public:
  RotaryCounter();
  RotaryCounter(int lower_bound, int upper_bound, byte steps_per_click);
  RotaryCounter(int lower_bound, int upper_bound, byte steps_per_click, int initial_pos);
  RotaryCounter(int lower_bound, int upper_bound, byte steps_per_click, int initial_pos, bool roll_over);
  void begin(int lower_bound, int upper_bound, byte steps_per_click, int initial_pos, bool roll_over);
  void setRollOver(bool roll_over);
  void updateCounter(int steps);
  int getPosition();
  byte getDirection();
  String directionToString(byte direction);

  void resetPosition(int p = 0, bool fireCallback = true);

  void setUpperBound(int upper_bound);
  void setLowerBound(int lower_bound);
  int getUpperBound();
  int getLowerBound();

  void setStepsPerClick(int steps);
  int getStepsPerClick();

  void setChangedHandler(CallbackFunction f);
  void setRightRotationHandler(CallbackFunction f);
  void setLeftRotationHandler(CallbackFunction f);
  void setUpperOverflowHandler(CallbackFunction f);
  void setLowerOverflowHandler(CallbackFunction f);
};

class Rotary
{
protected:
  byte _pin1;
  byte _pin2;

  byte state;
  int increment;

  byte _acceleration;

  RotaryCounter *_counter;
  unsigned long _time_list[TIME_LIST_LENGTH];
  byte _time_list_pos;

  int getCurentDelta();

public:
  Rotary();
  Rotary(byte pin1, byte pin2);

  void begin(byte pin1, byte pin2, byte acceleration, RotaryCounter *counter = nullptr);

  void setAcceleration(byte acceleration);
  byte getAcceleration();

  // void setChangedHandler(CallbackFunction f);
  // void setRightRotationHandler(CallbackFunction f);
  // void setLeftRotationHandler(CallbackFunction f);
  // void setUpperOverflowHandler(CallbackFunction f);
  // void setLowerOverflowHandler(CallbackFunction f);

  void loop();
};

/////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////
