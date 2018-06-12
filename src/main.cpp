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

void setDHTFlag() {
  DHTFlag = true;
}

void setup() {
  Serial.begin(74880);
  dht11.setPin(D4);
  dht11.setCallback([](int8_t result) {
    if (result > 0) {
      uint32_t micros[40];
      dht11.getMicros(micros);
      Serial.print("Raw data:\n");
      for (uint8_t i = 1; i < 40; ++i) {
        Serial.println(micros[i]);
      }
      Serial.printf("Temp: %i°C\n", dht11.getTemp());
      Serial.printf("Humid: %i%%\n", dht11.getHumid());
    } else {
      Serial.printf("Error: %s\n", dht11.getError());
    }
  });
  ticker.attach(30, setDHTFlag);
}

void loop() {
  if (DHTFlag) {
    DHTFlag = false;
    Serial.print("\nStart acquiring\n");
    dht11.read();
  }
}
