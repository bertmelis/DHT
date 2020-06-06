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

namespace DHTInternals {

typedef std::function<void(float, float)> OnData_CB;
typedef std::function<void(uint8_t)> OnError_CB;

}  // end namespace DHTInternals

class DHT {
 public:
  DHT();
  void setup(uint8_t pin);
  void onData(DHTInternals::OnData_CB callback);
  void onError(DHTInternals::OnError_CB callback);
  void read();
  const char* getError() const;

 protected:
  volatile uint8_t _status;
  volatile uint8_t _data[5];

 private:
  uint8_t _pin;
  DHTInternals::OnData_CB _onData;
  DHTInternals::OnError_CB _onError;
  Ticker _timer;
  volatile int8_t _counter;
  volatile uint32_t _previousMicros;
  static void ICACHE_RAM_ATTR _handleRead(DHT* a);
  static void ICACHE_RAM_ATTR _handleData(void* a);
  void ICACHE_RAM_ATTR _stop(uint8_t status);
  static void ICACHE_RAM_ATTR _timeout(DHT* a);
  void ICACHE_RAM_ATTR _tryCallback();
  virtual float _getHumidity() = 0;
  virtual float _getTemperature() = 0;
};

class DHT11 : public DHT {
 private:
  float _getHumidity();
  float _getTemperature();
};

class DHT22 : public DHT {
 private:
  float _getHumidity();
  float _getTemperature();
};
