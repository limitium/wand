# Wand - Xiaomi sensors advertisement listener

[![Build Status](https://travis-ci.org/limitium/wand.svg?branch=master)](https://travis-ci.org/limitium/wand)
[![License](https://img.shields.io/badge/license-MIT%20License-blue.svg)](http://doge.mit-license.org)

Inspired by https://github.com/custom-components/sensor.mitemp_bt

## Features
- Simplest usage and api ever
- Supports a lot of Xiaomi sensors

## Quick start

```c++
void setup()
{
    Serial.begin(9600);

    WAND::Wand wand = WAND::Wand([](const char *sensorName, BLEAddress mac, const char *paramName, float value) {
        Serial.printf("Sensor: %s, param: %s, val: %.2f\n", sensorName, paramName, value);
    });

    wand.init();
}
```

## Supported sensors
- LYWSDCGQ

  (round body, segment LCD, broadcasts temperature, humidity and battery level, about 20 readings per minute)
  
  ![LYWSDCGQ](https://github.com/custom-components/sensor.mitemp_bt/raw/master/pictures/LYWSDCGQ.jpg)
  
- CGG1

  (round body, E-Ink, broadcasts temperature, humidity and battery level, about 20 readings per minute)

  ![CGG1](https://github.com/custom-components/sensor.mitemp_bt/raw/master/pictures/CGG1.png)

- LYWSD02

  (rectangular body, E-Ink, broadcasts temperature and humidity, about 20 readings per minute, no battery info)

  ![LYWSD02](https://github.com/custom-components/sensor.mitemp_bt/raw/master/pictures/LYWSD02.jpeg)
  
- LYWSD03MMC

  (small square body, segment LCD, broadcasts temperature and humidity once in about 10 minutes and battery level once in an hour, advertisements are encrypted, therefore you need to upload custom [firmware](https://github.com/atc1441/ATC_MiThermometer)
  
  ![LYWSD03MMC](https://github.com/custom-components/sensor.mitemp_bt/raw/master/pictures/LYWSD03MMC.jpg)

- MHO-C303

  (Alarm clock, rectangular body, E-Ink, broadcasts temperature, humidity and battery level, about 20 readings per minute)
  
  ![MHO-C303](https://github.com/custom-components/sensor.mitemp_bt/raw/master/pictures/MHO-C303.png)

- JQJCY01YM

  (Xiaomi Honeywell Formaldehyde Sensor, OLED display, broadcasts temperature, humidity, formaldehyde (mg/m³) and battery level, about 50 messages per minute)
  
  ![supported sensors](https://github.com/custom-components/sensor.mitemp_bt/raw/master/pictures/JQJCY01YM.jpg)

- HHCCJCY01

  (MiFlora, broadcasts temperature, moisture, illuminance, conductivity, 1 reading per minute, no battery info with firmware v3.2.1)
  
  ![HHCCJCY01](https://github.com/custom-components/sensor.mitemp_bt/raw/master/pictures/HHCCJCY01.jpg)

- GCLS002

  (VegTrug Grow Care Garden, similar to MiFlora HHCCJCY01)

  ![GCLS002](https://github.com/custom-components/sensor.mitemp_bt/raw/master/pictures/GCLS002.jpg)

- HHCCPOT002

  (FlowerPot, RoPot, broadcasts moisture and conductivity, 2 readings per minute, no battery info with firmware v1.2.6)
  
  ![HHCCPOT002](https://github.com/custom-components/sensor.mitemp_bt/raw/master/pictures/HHCCPOT002.jpg)

- WX08ZM

  (Xiaomi Mija Mosquito Repellent, Smart version, broadcasts switch state, tablet resource, battery level, about 50 messages per minute)
 
  ![supported sensors](https://github.com/custom-components/sensor.mitemp_bt/raw/master/pictures/WX08ZM.jpg)


## Exmaple
- [Basic](https://github.com/limitium/wand/tree/master/examples/basic) - basic=full usage example

## Dependencies
* [ESP32_BLE_Arduino](https://github.com/nkolban/ESP32_BLE_Arduino)

## License
Wand is open-source software licensed under the [MIT license](http://opensource.org/licenses/MIT)
