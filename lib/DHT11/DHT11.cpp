/*

Copyright 2018 Bert Melis

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be included
in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#include <DHT11.hpp>  // NOLINT

DHT11::DHT11() :
  _pin(0),
  _timer(),
  _result(0),
  _micros{0},
  _currentMicros(0) {}

void DHT11::setPin(uint8_t pin) {
  _pin = pin;
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, HIGH);
}

void DHT11::read() {
  memset(_micros, 0, 40);
  _result = 0;
  _currentMicros = 0;
  digitalWrite(_pin, LOW);
  _timer.once_ms(20, &DHT11::_handleRead, this);
}

const int8_t DHT11::ready() const {
  return _result;
}

float DHT11::getTemp() {
  _result = 0;
  return 26.1;
}

const char* DHT11::getError() {
  if (_result == 1) {
    strncpy(_errorStr, "OK", sizeof(_errorStr));
  } else if (_result == 0) {
    strncpy(_errorStr, "BUSY", sizeof(_errorStr));
  } else if (_result == -1) {
    strncpy(_errorStr, "TO", sizeof(_errorStr));
  }
  _result = 0;
  return _errorStr;
}

void DHT11::_handleRead(DHT11* instance) {
  instance->_timer.once(1, &DHT11::_timeout, instance);
  attachInterrupt(instance->_pin, std::bind(&DHT11::_handleAck, instance), FALLING);
  pinMode(instance->_pin, INPUT);
}

void DHT11::_handleAck() {
  _micros[_currentMicros++] = micros();
  if (_currentMicros == 2) {
    if (_micros[1] - _micros[0] < 160) {
      detachInterrupt(_pin);
      pinMode(_pin, OUTPUT);
      digitalWrite(_pin, HIGH);
      _timer.detach();
      _result = -2;  // nack signal
    } else {
      detachInterrupt(_pin);
      attachInterrupt(_pin, std::bind(&DHT11::_handleData, this), FALLING);
      _currentMicros = 0;
    }
  }
}

void DHT11::_handleData() {
  _micros[_currentMicros++] = micros();
  if (_currentMicros == 39) {
    detachInterrupt(_pin);
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, HIGH);
    _timer.detach();
    _result = 1;
  }
}

void DHT11::_timeout(DHT11* instance) {
  instance->_timer.detach();
  detachInterrupt(instance->_pin);
  instance->_result = -1;  // timeout
}
