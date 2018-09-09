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

#pragma once

#include <functional>

#include <Arduino.h>
#include <Ticker.h>
#include <FunctionalInterrupt.h>

typedef std::function<void(int8_t)> Callback;

class DHT {
 public:
  DHT();
  void setPin(uint8_t pin);
  void setCallback(Callback cb);
  void read();
  const int8_t ready() const;
  virtual float getTemperature() = 0;
  virtual float getHumidity() = 0;
  const char* getError();

 protected:
  volatile int8_t _result;
  volatile uint8_t _data[5];

 private:
  uint8_t _pin;
  Callback _callback;
  Ticker _timer;
  char _errorStr[5];
  volatile uint8_t _counter;
  volatile uint32_t _previousMicros;
  static void ICACHE_RAM_ATTR _handleRead(DHT* instance);
  void ICACHE_RAM_ATTR _handleAck();
  void ICACHE_RAM_ATTR _handleData();
  static void ICACHE_RAM_ATTR _timeout(DHT* instance);
  void ICACHE_RAM_ATTR _tryCallback();
};

class DHT11 : public DHT {
 public:
  DHT11();
  float getTemperature();
  float getHumidity();
};

class DHT22 : public DHT {
 public:
  DHT22();
  float getTemperature();
  float getHumidity();
};
