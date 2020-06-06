# DHT

[![Build Status](https://travis-ci.com/bertmelis/DHT.svg?branch=master)](https://travis-ci.com/bertmelis/DHT)

DHT temperature and humidity sensor library for the Arduino framework for ESP8266.  
For ESP32, please look into this repo [esp32DHT](https://github.com/bertmelis/esp32DHT)

This is yet another DHT library but it does come with a difference. It is completely non blocking so doesn't use `delay()`, anywhere. The inner working of the lib relies completely on pin interrupts and timers.

This library aims to be minimal. No bounds checks are in place, no extra calculation methods are available.

## Installation

* For Arduino IDE: see the [Arduino Guide](https://www.arduino.cc/en/Guide/Libraries#toc4)
* For Platformio: see the [Platfomio guide](http://docs.platformio.org/en/latest/projectconf/section_env_library.html)

## Usage

2 simple examples are included. I tested on a Wemos D1 mini (with Wemos DHT11 shield and a seperate DHT22 sensor).
Keep in mind that this library is interrupt driven. Your callback function will also be called from within an interrupt.Therefore, the callback should be short, fast and not contain any interrupt driven tasks (like Serial).
You can read the values and set a "flag" or bind your callback to "Schedule" and run your code at the next loop()-call.

Simple example:

```C++
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

```

## History

I was tired exploring all the DHT libraries only to find out they were blocking, using `delay()` or had some sort of other restrictions to read the sensor. So I ended up creating my own. Although I created this lib from scratch I did look into the other libraries to see how they were coded. Nevertheless I'm not going to give a full list of credits because I'll probably forget a few.
