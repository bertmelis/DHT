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


#include <Arduino.h>
#include <Ticker.h>

#include <DHT.h>

Ticker ticker;
DHT22 sensor;  // change to DHT11 if needed!
volatile float humidity = 0;
volatile float temperature = 0;
volatile uint8_t error = 0;
volatile int8_t result = 0;

void readDHT() {
  sensor.read();
}

// this callback will be called from an interrupt
// it should be short and carry the ICACHE_RAM_ATTR attribute
void ICACHE_RAM_ATTR handleData(float h, float t) {
  humidity = h;
  temperature = t;
  result = 1;
}

// this callback will be called from an interrupt
// it should be short and carry the ICACHE_RAM_ATTR attribute
void ICACHE_RAM_ATTR handleError(uint8_t e) {
  error = e;
  result = -1;
}

void setup() {
  Serial.begin(74880);
  sensor.setup(D4);
  sensor.onData(handleData);
  sensor.onError(handleError);
  ticker.attach(30, readDHT);
}

void loop() {
  if (result > 0) {
    Serial.printf("Humid: %g%%\n", humidity);
    Serial.printf("Temp: %gdegC\n", temperature);
  } else if (result < 0) {
    Serial.printf("Error: %s\n", sensor.getError());
  }
  result = 0;
}
