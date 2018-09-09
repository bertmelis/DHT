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

#include <DHT.hpp>  // NOLINT

DHT::DHT() :
  _result(0),
  _data{0},
  _pin(0),
  _callback(nullptr),
  _timer(),
  _errorStr{0},
  _counter(0),
  _previousMicros(0) {}

void DHT::setPin(uint8_t pin) {
  _pin = pin;
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, HIGH);
}

void DHT::setCallback(Callback cb) {
  _callback = cb;
}

void DHT::read() {
  _data[0] = _data[1] = _data[2] = _data[3] = _data[4] = 0;
  _counter = 0;
  _result = 0;
  digitalWrite(_pin, LOW);
  _timer.once_ms(20, &DHT::_handleRead, this);
}

const int8_t DHT::ready() const {
  return _result;
}

const char* DHT::getError() {
  if (_result == 1) {
    strcpy(_errorStr, "OK");  // NOLINT
  } else if (_result == 0) {
    strcpy(_errorStr, "BUSY");  // NOLINT
  } else if (_result == -1) {
    strcpy(_errorStr, "TO");   // NOLINT
  } else if (_result == -2) {
    strcpy(_errorStr, "NACK");   // NOLINT
  } else if (_result == -3) {
    strcpy(_errorStr, "DATA");   // NOLINT
  } else if (_result == -4) {
    strcpy(_errorStr, "CS");   // NOLINT
  }
  _result = 0;
  return _errorStr;
}

void DHT::_handleRead(DHT* instance) {
  instance->_timer.once_ms(1000, &DHT::_timeout, instance);
  attachInterrupt(instance->_pin, std::bind(&DHT::_handleAck, instance), FALLING);
  instance->_previousMicros = micros();
  pinMode(instance->_pin, INPUT);
}

void DHT::_handleAck() {
  if (_counter == 0) {
    ++_counter;
  } else if (micros() - _previousMicros < 160) {
      detachInterrupt(_pin);
      pinMode(_pin, OUTPUT);
      digitalWrite(_pin, HIGH);
      _timer.detach();
      _result = -2;  // nack signal
      _tryCallback();
  } else {
    detachInterrupt(_pin);
    _previousMicros = micros();
    attachInterrupt(_pin, std::bind(&DHT::_handleData, this), FALLING);
    _counter = 0;
  }
}

void DHT::_handleData() {
  uint32_t delta = micros() - _previousMicros;
  _previousMicros = micros();
  if (delta > 50 && delta < 160) {  // 50µs is to allow first bit which comes in a bit "early".
    _data[_counter / 8] <<= 1;  // shift left
    if (delta > 120) {
      _data[_counter / 8] |= 1;
    }
  } else {
    detachInterrupt(_pin);
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, HIGH);
    _timer.detach();
    _result = -3;  // data error
    _tryCallback();
  }
  ++_counter;
  if (_counter == 40) {
    detachInterrupt(_pin);
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, HIGH);
    _timer.detach();
    if (_data[4] == ((_data[0] + _data[1] + _data[2] + _data[3]) & 0xFF)) {
      _result = 1;
    } else {
      _result = -4;  // checksum error
    }
    _tryCallback();
  }
}

void DHT::_timeout(DHT* instance) {
  instance->_timer.detach();
  detachInterrupt(instance->_pin);
  pinMode(instance->_pin, OUTPUT);
  digitalWrite(instance->_pin, HIGH);
  instance->_result = -1;  // timeout
  instance->_tryCallback();
}

void DHT::_tryCallback() {
  if (_callback) _callback(_result);
}

DHT11::DHT11() :
  DHT() {}

float DHT11::getTemperature() {
  _result = 0;
  return static_cast<float>(_data[2]);
}

float DHT11::getHumidity() {
  _result = 0;
  return static_cast<float>(_data[0]);
}

DHT22::DHT22() :
  DHT() {}

float DHT22::getTemperature() {
  _result = 0;
  float temp = word(_data[2] & 0x7F, _data[3]) * 0.1;
  if (_data[2] & 0x80) {  // negative temperature
    temp = -temp;
  }
  return temp;
}

float DHT22::getHumidity() {
  _result = 0;
  return word(_data[0], _data[1]) * 0.1;
}
