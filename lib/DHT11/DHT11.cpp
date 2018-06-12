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
  _errorStr{0},
  _data{0},
  _counter(0),
  _previousMicros(0),
  _micros{0} {}

void DHT11::setPin(uint8_t pin) {
  _pin = pin;
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, HIGH);
}

void DHT11::setCallback(Callback cb) {
  _callback = cb;
}

void DHT11::read() {
  _data[0] = _data[1] = _data[2] = _data[3] = _data[4] = 0;
  _counter = 0;
  _result = 0;
  memset(_micros, 0, sizeof(_micros));
  digitalWrite(_pin, LOW);
  _timer.once_ms(20, &DHT11::_handleRead, this);
}

const int8_t DHT11::ready() const {
  return _result;
}

int8_t DHT11::getTemp() {
  _result = 0;
  return _data[2];
}

int8_t DHT11::getHumid() {
  _result = 0;
  return _data[0];
}

int8_t DHT11::getChecksum() {
  _result = 0;
  return _data[4];
}

const char* DHT11::getError() {
  if (_result == 1) {
    strncpy(_errorStr, "OK", sizeof(_errorStr));
  } else if (_result == 0) {
    strncpy(_errorStr, "BUSY", sizeof(_errorStr));
  } else if (_result == -1) {
    strncpy(_errorStr, "TO", sizeof(_errorStr));
  } else if (_result == -2) {
    strncpy(_errorStr, "NACK", sizeof(_errorStr));
  } else if (_result == -3) {
    strncpy(_errorStr, "DATA", sizeof(_errorStr));
  }
  _result = 0;
  return _errorStr;
}

void DHT11::_handleRead(DHT11* instance) {
  instance->_timer.once_ms(1000, &DHT11::_timeout, instance);
  attachInterrupt(instance->_pin, std::bind(&DHT11::_handleAck, instance), FALLING);
  instance->_previousMicros = 0;
  instance->_previousMicros = micros();
  pinMode(instance->_pin, INPUT);
}

void DHT11::_handleAck() {
  if (_counter == 0) {
    ++_counter;
  } else if (micros() - _previousMicros < 160) {
      detachInterrupt(_pin);
      pinMode(_pin, OUTPUT);
      digitalWrite(_pin, HIGH);
      _timer.detach();
      _result = -2;  // nack signal
      schedule_function(std::bind(_callback, _result));
  } else {
    detachInterrupt(_pin);
    _previousMicros = micros();
    attachInterrupt(_pin, std::bind(&DHT11::_handleData, this), FALLING);
    _counter = 0;
  }
}

void DHT11::_handleData() {
  uint32_t delta = micros() - _previousMicros;
  // TODO(bertmelis) remove next line after debugging
  _micros[_counter] = delta;
  _previousMicros = micros();
  if (delta > 120) {
    _data[_counter / 8] = (_data[_counter / 8] << 1) + 1;  // shift left and add 1
  } else if (delta > 70) {  // add 0
    _data[_counter / 8] = _data[_counter / 8] << 1;  // only shift lef (add zero)
  } else {
    _timer.detach();
    detachInterrupt(_pin);
    _result = -3;  // data error
    schedule_function(std::bind(_callback, _result));
  }
  ++_counter;
  if (_counter == 39) {
    detachInterrupt(_pin);
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, HIGH);
    _timer.detach();
    // TODO(bertmelis) add checksum check
    _result = 1;
    schedule_function(std::bind(_callback, _result));
  }
}

void DHT11::_timeout(DHT11* instance) {
  instance->_timer.detach();
  detachInterrupt(instance->_pin);
  instance->_result = -1;  // timeout
  schedule_function(std::bind(instance->_callback, instance->_result));
}
