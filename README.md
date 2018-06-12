# DHT

[![Build Status](https://travis-ci.org/bertmelis/DHT.svg?branch=master)](https://travis-ci.org/bertmelis/DHT)

DHT temperature and humidity sensor library for the Arduino framework for ESP8266.

This is yet another DHT library but it does come with a difference. It is completely non blocking so doesn't use `delay()`, anywhere. Reading the sensor's values is possible using the Arduino `loop()` or using a callback function.
The inner working of the lib relies completely on pin interrupts and timers.

## Installation

* For Arduino IDE: see [the Arduino Guide](https://www.arduino.cc/en/Guide/Libraries#toc4)
* For Platformio: see the [Platfomio guide](http://docs.platformio.org/en/latest/projectconf/section_env_library.html)

## Usage

2 simple examples are included. I tested on a Wemos D1 mini (with Wemos DHT11 shield).

Reading the sensor using a callback (mind the `loop()` being empty):
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
  dht11.setCallback([](int8_t result) {
    if (result > 0) {
      Serial.printf("Temp: %i°C\n", dht11.getTemperature());
      Serial.printf("Humid: %i%%\n", dht11.getHumidity());
    } else {
      Serial.printf("Error: %s\n", dht11.getError());
    }
  });
  ticker.attach(30, readDHT);
}

void loop() {}
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
