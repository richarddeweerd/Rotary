/////////////////////////////////////////////////////////////////
/*
  ESP8266/Arduino Library for reading rotary encoder values.
  Copyright 2017-2021 Lennart Hennigs.
*/
/////////////////////////////////////////////////////////////////

#include "Rotary.h"

/////////////////////////////////////////////////////////////////

Rotary::Rotary()
{
}

/////////////////////////////////////////////////////////////////

Rotary::Rotary(byte pin1, byte pin2)
{
  begin(pin1, pin2, 0);
}

/////////////////////////////////////////////////////////////////

void Rotary::begin(byte pin1, byte pin2, byte acceleration, RotaryCounter *counter)
{
  this->_pin1 = pin1;
  this->_pin2 = pin2;
  pinMode(pin1, INPUT_PULLUP);
  pinMode(pin2, INPUT_PULLUP);
  setAcceleration(acceleration);
  _counter = counter;

  loop();
}

void Rotary::setAcceleration(byte acceleration)
{
  _acceleration = acceleration;

  if (_acceleration > 0)
  {
    _time_list[0] = millis();
    for (int i = 1; i < TIME_LIST_LENGTH; i++)
    {
      _time_list[i] = _time_list[0];
    }
    _time_list_pos = 0;
  }
}

int Rotary::getCurentDelta()
{
  if (_acceleration != 0)
  {
    unsigned long _now = millis();
    unsigned long time_delta = _now - _time_list[_time_list_pos];
    // Serial.println(time_delta);
    _time_list[_time_list_pos] = _now;
    _time_list_pos += 1;
    if (_time_list_pos >= TIME_LIST_LENGTH)
    {
      _time_list_pos = 0;
    }
    if (time_delta < 100)
    {
      return _acceleration;
    }
    if (time_delta < 200)
    {
      return _acceleration/2;
    }
  }

  return 1;
}

byte Rotary::getAcceleration()
{
  return _acceleration;
}

void Rotary::loop()
{
  int s = state & 3;
  if (digitalRead(_pin1))
    s |= 4;
  if (digitalRead(_pin2))
    s |= 8;

  int delta_steps = getCurentDelta();
  switch (s)
  {
  case 0:
  case 5:
  case 10:
  case 15:
    break;
  case 1:
  case 7:
  case 8:
  case 14:
    // position += increment;
    _counter->updateCounter(delta_steps);
    // Serial.println(position);

    break;
  case 2:
  case 4:
  case 11:
  case 13:
    // position -= increment;
    // Serial.println(delta_steps);
    _counter->updateCounter(delta_steps * -1);
    break;
  case 3:
  case 12:
    // position += 2 * increment;
    Serial.print("e");
    break;
  default:
    // position -= 2 * increment;
    Serial.print("E");
    break;
  }
  state = (s >> 2);

  // if (position != last_position && (abs(position - last_position) >= _steps_per_click * increment))
  // {
  //   int current_position = getPosition();
  //   if (current_position >= _lower_bound && current_position <= _upper_bound)
  //   {
  //     if (position > last_position)
  //     {
  //       direction = RE_RIGHT;
  //       if (right_cb != NULL)
  //         right_cb(*this);
  //     }
  //     else
  //     {
  //       direction = RE_LEFT;
  //       if (left_cb != NULL)
  //         left_cb(*this);
  //     }
  //     last_position = position;
  //     if (change_cb != NULL)
  //       change_cb(*this);
  //   }
  //   else
  //   {
  //     position = last_position;
  //     if (current_position < _lower_bound && lower_cb != NULL)
  //       lower_cb(*this);
  //     if (current_position > _upper_bound && upper_cb != NULL)
  //       upper_cb(*this);
  //   }
  // }
}

RotaryCounter::RotaryCounter()
{
  begin(0, 255, 4, 0, false);
}

RotaryCounter::RotaryCounter(int lower_bound, int upper_bound, byte steps_per_click)
{
  begin(lower_bound, upper_bound, steps_per_click, 0, false);
}

RotaryCounter::RotaryCounter(int lower_bound, int upper_bound, byte steps_per_click, int initial_pos)
{
  begin(lower_bound, upper_bound, steps_per_click, initial_pos, false);
}

RotaryCounter::RotaryCounter(int lower_bound, int upper_bound, byte steps_per_click, int inital_pos, bool roll_over)
{
  begin(lower_bound, upper_bound, steps_per_click, inital_pos, roll_over);
}

void RotaryCounter::begin(int lower_bound, int upper_bound, byte steps_per_click, int initial_pos, bool roll_over)
{
  setLowerBound(lower_bound);
  setUpperBound(upper_bound);
  setStepsPerClick(steps_per_click);
  _roll_over = roll_over;
  _step_counter = initial_pos * steps_per_click;
  _position = initial_pos;
}

void RotaryCounter::setRollOver(bool roll_over)
{
  _roll_over = roll_over;
}

void RotaryCounter::updateCounter(int steps)
{
  // Serial.println("");
  // Serial.println(steps);
  _last_position = _position;


  _step_counter += steps;

  if (_step_counter < _lower_bound)
  {
    if (_roll_over)
    {
      int delta = _step_counter - _lower_bound;
      _step_counter = (_upper_bound - 1) + delta;
    }
    else
    {
      _step_counter = _lower_bound;
    }
  }
  else
  {
    if (_step_counter >= _upper_bound)
    {
      if (_roll_over)
      {
        _step_counter = _step_counter - _upper_bound;
      }
      else
      {
        _step_counter = _upper_bound - 1;
      }
    }
  }
  _position = _step_counter / _steps_per_click;
  // Serial.println(position);

  // Serial.println(_step_counter);

  if (_position > _last_position)
  {
    direction = RE_RIGHT;
    if (right_cb != NULL)
      right_cb(*this);
  }
  else
  {
    if (_position < _last_position)
    {
      direction = RE_LEFT;
      if (left_cb != NULL)
        left_cb(*this);
    }
  }
  if (_position != _last_position)
  {
    if (change_cb != NULL)
      change_cb(*this);
  }
}

void RotaryCounter::setChangedHandler(CallbackFunction f)
{
  change_cb = f;
}

/////////////////////////////////////////////////////////////////

void RotaryCounter::setRightRotationHandler(CallbackFunction f)
{
  right_cb = f;
}

/////////////////////////////////////////////////////////////////

void RotaryCounter::setLeftRotationHandler(CallbackFunction f)
{
  left_cb = f;
}

/////////////////////////////////////////////////////////////////

void RotaryCounter::setUpperOverflowHandler(CallbackFunction f)
{
  upper_cb = f;
}

/////////////////////////////////////////////////////////////////

void RotaryCounter::setLowerOverflowHandler(CallbackFunction f)
{
  lower_cb = f;
}

int RotaryCounter::getPosition()
{
  return _position;
}

/////////////////////////////////////////////////////////////////

byte RotaryCounter::getDirection()
{
  return direction;
}

/////////////////////////////////////////////////////////////////

String RotaryCounter::directionToString(byte direction)
{
  if (direction == RE_LEFT)
  {
    return "LEFT";
  }
  else
  {
    return "RIGHT";
  }
}

/////////////////////////////////////////////////////////////////

void RotaryCounter::resetPosition(int p /* = 0 */, bool fireCallback /* = true */)
{

  int new_pos = p * getStepsPerClick();

  if (new_pos > _upper_bound)
  {
    new_pos = _upper_bound;
  }
  else
  {
    if (new_pos < _lower_bound)
    {
      new_pos = _lower_bound;
    }
  }

  if (new_pos != _last_position)
  {
    _last_position = new_pos;
    if (fireCallback && change_cb != NULL)
      change_cb(*this);
  }
  direction = 0;
}

/////////////////////////////////////////////////////////////////

void RotaryCounter::setStepsPerClick(int steps)
{
  if (steps < 1)
  {
    _steps_per_click = 1;
  }
  else
  {
    _steps_per_click = steps;
  }
}

/////////////////////////////////////////////////////////////////

int RotaryCounter::getStepsPerClick()
{
  return _steps_per_click;
}

/////////////////////////////////////////////////////////////////

void RotaryCounter::setUpperBound(int upper_bound)
{
  upper_bound += 1;
  if ((upper_bound * _steps_per_click) > _lower_bound)
  {
    _upper_bound = upper_bound * _steps_per_click;
  }
  else
  {
    _upper_bound = _lower_bound;
  }
}

/////////////////////////////////////////////////////////////////

void RotaryCounter::setLowerBound(int lower_bound)
{
  if ((lower_bound * _steps_per_click) < _upper_bound)
  {

    _lower_bound = lower_bound * _steps_per_click;
  }
  else
  {
    _lower_bound = _upper_bound;
  }
}

/////////////////////////////////////////////////////////////////

int RotaryCounter::getUpperBound()
{
  return this->_upper_bound;
}

/////////////////////////////////////////////////////////////////

int RotaryCounter::getLowerBound()
{
  return this->_lower_bound;
}