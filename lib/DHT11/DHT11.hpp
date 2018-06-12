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

#include <functional>

#include <Arduino.h>
#include <Ticker.h>
#include <FunctionalInterrupt.h>
#include <Schedule.h>

typedef std::function<void(int8_t)> Callback;

class DHT11 {
 public:
  DHT11();
  void setPin(uint8_t pin);
  void setCallback(Callback cb);
  void read();
  const int8_t ready() const;
  int8_t getTemp();
  int8_t getHumid();
  int8_t getChecksum();
  const char* getError();
  // TODO(bertmelis) remove getMicros after debugging
  void getMicros(uint32_t* array) { for (uint8_t i = 0; i < 41; ++i) array[i] = _micros[i]; }
 private:
  uint8_t _pin;
  Callback _callback;
  Ticker _timer;
  int8_t _result;
  static void ICACHE_RAM_ATTR _handleRead(DHT11* instance);
  void ICACHE_RAM_ATTR _handleAck();
  void ICACHE_RAM_ATTR _handleData();
  static void ICACHE_RAM_ATTR _timeout(DHT11* instance);
private:
  char _errorStr[5];
  volatile int8_t _data[5];
  volatile uint8_t _counter;
  volatile uint32_t _previousMicros;
  // TODO(bertmelis) remove _micros after debugging
  uint32_t _micros[41];
};