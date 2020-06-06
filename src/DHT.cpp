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
  _status(0),
  _data{0},
  _pin(0),
  _onData(nullptr),
  _onError(nullptr),
  _timer(),
  _counter(0),
  _previousMicros(0) {}

void DHT::setup(uint8_t pin) {
  _pin = pin;
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, HIGH);
}

void DHT::onData(DHTInternals::OnData_CB callback) {
  _onData = callback;
}

void DHT::onError(DHTInternals::OnError_CB callback) {
  _onError = callback;
}

void DHT::read() {
  _data[0] = _data[1] = _data[2] = _data[3] = _data[4] = 0;
  _counter = -2;
  _status = 0;
  digitalWrite(_pin, LOW);
  _timer.once_ms(20, &DHT::_handleRead, this);
  attachInterruptArg(_pin, &DHT::_handleData, this, FALLING);
}

const char* DHT::getError() const {
  if (_status == 1) {
    return "TO";
  } else if (_status == 2) {
    return "NACK";
  } else if (_status == 3) {
    return "DATA";
  } else if (_status == 4) {
    return "CS";
  }
  return "OK";
}

void DHT::_handleRead(DHT* a) {
  a->_timer.once_ms(1000, &DHT::_timeout, a);
  // attachInterrupt(instance->_pin, std::bind(&DHT::_handleData, instance), FALLING);
  pinMode(a->_pin, INPUT);
  a->_previousMicros = micros();
}

void DHT::_handleData(void* a) {
  DHT* d = reinterpret_cast<DHT*>(a);
  uint32_t delta = micros() - d->_previousMicros;
  d->_previousMicros = micros();
  if (d->_counter < -1) {  // pin pulled low by sensor to start ACK
    ++(d->_counter);
    return;
  }
  if (d->_counter < 0) {  // pin pulled low by sensor to end ACK
    if (delta < 130 || delta > 190) {  // relaxed datasheet limits with +/-20µs
      d->_stop(2);  // nack signal
    }
    ++(d->_counter);
    return;
  }
  // from here the usable bit pattern will come in
  if (delta > 50 && delta < 160) {  // relaxed datasheet limits with +/-20µs
    d->_data[d->_counter / 8] <<= 1;  // shift left (+ add 0)
    if (delta > 120) {
      d->_data[d->_counter / 8] |= 1;
    }
  } else {
    d->_stop(3);  // data error
  }
  ++(d->_counter);
  if (d->_counter == 40) {
    if (d->_data[4] == ((d->_data[0] + d->_data[1] + d->_data[2] + d->_data[3]) & 0xFF)) {
      d->_stop(0);  // succes
    } else {
      d->_stop(4);  // checksum error
    }
  }
}

void DHT::_stop(uint8_t status) {
  _status = status;
  detachInterrupt(_pin);
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, HIGH);
  _timer.detach();
  _tryCallback();
}

void DHT::_timeout(DHT* a) {
  a->_timer.detach();
  detachInterrupt(a->_pin);
  pinMode(a->_pin, OUTPUT);
  digitalWrite(a->_pin, HIGH);
  a->_status = 1;  // timeout
  a->_tryCallback();
}

void DHT::_tryCallback() {
  if (_status == 0) {
    if (_onData) _onData(_getHumidity(), _getTemperature());
  } else {
    if (_onError) _onError(_status);
  }
}

float DHT11::_getHumidity() {
  return static_cast<float>(_data[0]);
}

float DHT11::_getTemperature() {
  return static_cast<float>(_data[2]);
}

float DHT22::_getHumidity() {
  return word(_data[0], _data[1]) * 0.1;
}


float DHT22::_getTemperature() {
  float temp = word(_data[2] & 0x7F, _data[3]) * 0.1;
  if (_data[2] & 0x80) {  // set high bit for negative temperature
    temp = -temp;
  }
  return temp;
}
