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

/*
Because the callbacks are called from an interrupt, you are limited
in what you can do. To overcome you can "schedule" a function to run after
the next loop() using 'schedule_function()'.
*/


#include <Arduino.h>
#include <Ticker.h>
#include <Schedule.h>

#include <DHT.h>

Ticker ticker;
DHT22 sensor;  // change to DHT11 if needed!

void readDHT() {
  sensor.read();
}

void setup() {
  Serial.begin(74880);
  sensor.setup(D4);
  /*
  The following lines use a lambda function in which schedule_function() is called.
  This function in it's turn takes a argument-less function which will be executes after
  the next loop(). So another lambda is created with captured variables humidity and
  temperature.
  */
  sensor.onData([](float humidity, float temperature) {
    schedule_function([humidity, temperature]() {
      Serial.printf("Temp: %gdegC\n", temperature);
      Serial.printf("Humid: %g%%\n", humidity);
    });
  });
  sensor.onError([](uint8_t e) {
    schedule_function([e]() {
      Serial.printf("Error: %s\n", sensor.getError());
    });
  });
  ticker.attach(30, readDHT);
}

void loop() {
  // the callback will be run after the next loop()
}
