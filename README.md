# DHT

[![Build Status](https://travis-ci.com/bertmelis/DHT.svg?branch=master)](https://travis-ci.com/bertmelis/DHT)

DHT temperature and humidity sensor library for the Arduino framework for ESP8266.  
For ESP32, please look into this repo [esp32DHT](https://github.com/bertmelis/esp32DHT)

This is yet another DHT library but it does come with a difference. It is completely non blocking so doesn't use `delay()`, anywhere. Reading the sensor's values is possible using the Arduino `loop()` or using a callback function.
The inner working of the lib relies completely on pin interrupts and timers.

## WARNING:
This library is only stable using the git version of the Arduino core for ESP8266.
Please follow these instructions to get it going: [using git version](https://github.com/esp8266/Arduino#using-git-version)

## Installation

* For Arduino IDE: see [the Arduino Guide](https://www.arduino.cc/en/Guide/Libraries#toc4)
* For Platformio: see the [Platfomio guide](http://docs.platformio.org/en/latest/projectconf/section_env_library.html)

## Usage

2 simple examples are included. I tested on a Wemos D1 mini (with Wemos DHT11 shield and a seperate DHT22 sensor).
Keep in mind that this library is interrupt driven. Your callback function will also be called from within an interrupt. The callback therefore should be short, fast and not contain any interrupt driven tasks (like Serial).
To overcome you can bind your code to "Schedule" and run your code at the next loop()-call.

For DHT22, just replace `DHT11` by `DHT22` (and change the object's).

Reading the sensor using a callback:
```C++
#include <Arduino.h>
#include <Ticker.h>
#include <Schedule.h>

#include <DHT.h>

Ticker ticker;
DHT11 dht11;

void readDHT() {
  dht11.read();
}

void setup() {
  Serial.begin(74880);
  dht11.setPin(D4);
  dht11.setCallback([](int8_t result) {
    schedule_function(std::bind([](int8_t result){
      if (result > 0) {
        result = 0;
        Serial.printf("Temp: %g°C\n", dht11.getTemperature());
        Serial.printf("Humid: %g%%\n", dht11.getHumidity());
      } else {
        result = 0;
        Serial.printf("Error: %s\n", dht11.getError());
      }
    }, result));
  });
  ticker.attach(30, readDHT);
}

void loop() {
  // the callback will be ran after the next loop()
}

```

Read the sensor by polling the status in `loop()`:
```C++
#include <Arduino.h>
#include <Ticker.h>

#include <DHT11.h>

Ticker ticker;
DHT11 dht11;

void readDHT() {
  dht11.read();
}

void setup() {
  Serial.begin(74880);
  dht11.setPin(D4);
  ticker.attach(30, readDHT);
}

void loop() {
  int8_t result = dht11.ready();
  if (result > 0) {
    Serial.printf("Temp: %i°C\n", dht11.getTemperature());
    Serial.printf("Humid: %i%%\n", dht11.getHumidity());
  } else if (result < 0) {
    Serial.printf("Error: %s\n", dht11.getError());
  }
}
```

## History

I was tired exploring all the DHT libraries only to find out they were blocking, using `delay()` or had some sort of other restrictions to read the sensor. So I ended up creating my own. Although I created this lib from scratch I did look into the other libraries to see how they were coded. Nevertheless I'm not going to give a full list of credits because I'll probably forget a few.
