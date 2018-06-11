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

#include <DHT11.h>

Ticker ticker;
bool DHTFlag = false;

DHT11 dht11;
uint32_t array[2][40];

void setDHTFlag() {
  DHTFlag = true;
}

void setup() {
  Serial.begin(74880);
  dht11.setPin(D4);
  ticker.attach(20, setDHTFlag);
}

void loop() {
  if (DHTFlag) {
    DHTFlag = false;
    Serial.print("Start acquiring\n");
    dht11.read();
  }
  if (dht11.ready() > 0) {
    dht11.getMicros(&array[0][0]);
    array[1][0] = 0;
    for (uint8_t i = 1; i < 40; ++i) {
      array[1][i] = array[0][i] - array[0][i-1];
    }
    Serial.printf("current temp is %.1f\n", dht11.getTemp());
    for (uint8_t i = 0; i < 40; ++i) {
      Serial.printf("%u: %u - %u\n", i, array[0][i], array[1][i]);
    }
  } else if (dht11.ready() < 0) {
    Serial.printf("Error: %s\n", dht11.getError());
  }
}
