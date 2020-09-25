#include <Wand.h>

void setup()
{
    Serial.begin(9600);

    WAND::Wand wand = WAND::Wand([](const char *sensorName, BLEAddress mac, const char *paramName, float value) {
        Serial.printf("Sensor: %s, param: %s, val: %.2f\n", sensorName, paramName, value);
    });

    wand.init();
}