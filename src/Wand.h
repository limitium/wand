#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <map>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <functional>

namespace WAND
{
#define ADV_CALLBACK_SIGNATURE std::function<void(const char *, BLEAddress, const char *, float)>

std::map<std::string, std::string> XIAOMI_SUPPORTED_SENSORS = {
    {"0a04", "WX08ZM"},
    {"4703", "CGG1"},
    {"5b04", "LYWSD02"},
    {"5b05", "LYWSD03MMC"},
    {"5d01", "HHCCPOT002"},
    // {"8703", "MHO-C401"},
    {"9800", "HHCCJCY01"},
    {"aa01", "LYWSDCGQ"},
    {"bc03", "GCLS002"},
    {"d306", "MHO-C303"},
    {"df02", "JQJCY01YM"}};

std::map<std::string, std::string> XIAOMI_VALUE_TYPES = {
    {"0410", "temperature"},
    {"0610", "humidity"},
    {"0710", "illuminance"},
    {"0810", "moisture"},
    {"0910", "fertility"},
    {"1010", "formaldehyde"},
    {"1210", "switch"},
    {"1310", "consumable"},
    {"0a10", "battery"},
    {"0d10", "temperature,humidity"}};

std::string uintToHex(uint16_t val)
{
  std::stringstream stream;
  stream << std::hex << val;
  return stream.str();
}
unsigned int hexToUint(const char *str)
{
  std::istringstream converter(str);
  unsigned int value;
  converter >> std::hex >> value;
  return value;
}

char *dataToHex(uint8_t *source, uint8_t start, uint8_t length)
{
  uint8_t *target = (uint8_t *)malloc(length * 2 + 1);
  char *startOfData = (char *)target;

  source += start;
  for (int i = 0; i < length; i++)
  {
    sprintf((char *)target, "%.2x", (char)*source);
    source++;
    target += 2;
  }

  return startOfData;
}
void parseAdvertisement(uint8_t *payload, size_t total_len)
{
  uint8_t length;
  uint8_t ad_type;
  uint8_t sizeConsumed = 0;
  bool finished = false;

  while (!finished)
  {
    length = *payload;          // Retrieve the length of the record.
    payload++;                  // Skip to type
    sizeConsumed += 1 + length; // increase the size consumed.

    if (length != 0)
    { // A length of 0 indicates that we have reached the end.
      ad_type = *payload;
      payload++;
      length--;

      char *pHex = BLEUtils::buildHexData(nullptr, payload, length);
      Serial.printf("Type: 0x%.2x (%s), length: %d, data: %s\n",
                    ad_type, BLEUtils::advTypeToString(ad_type), length, pHex);
      free(pHex);

      payload += length;
    } // Length <> 0

    if (sizeConsumed >= total_len)
      finished = true;

  } // !finished
}

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
public:
  ADV_CALLBACK_SIGNATURE callback;
  MyAdvertisedDeviceCallbacks(ADV_CALLBACK_SIGNATURE cb)
  {
    callback = cb;
  }
  void onResult(BLEAdvertisedDevice advertisedDevice)
  {
    // CGG1 dump
    // 0201-06 1516-95fe50304703279d0f11342d580d1004da00fe01
    // 0201-06 1316-95fe50304703269d0f11342d58061002fe01
    // 95fe 5030 4703 d2 9d0f11342d58 0610 02 1102
    // 95fe 5030 4703 cf 9d0f11342d58 0d10 04 db000902
    //
    // HHCCJCY01 dump
    // 0201-06 0302-95fe 1416-95fe71209800dafe986b8d7cc40d0910020000
    // 0201-06 0302-95fe 1516-95fe7120980050fe986b8d7cc40d071003000000
    // 95fe 7120 9800 aa fe986b8d7cc4 0d 0910 02 0000
    // 95fe 7120 9800 85 fe986b8d7cc4 0d 0810 01 00
    // 95fe 7120 9800 bf fe986b8d7cc4 0d 0410 02 e300
    // 95fe 7120 9800 bc fe986b8d7cc4 0d 0710 03 000000

    // UUID | 0 index Frame control | Sensor type | Frame count | MAC          | Capability | Data type | Len | Val
    // 95fe | 7120                  | 9800        | bc          | fe986b8d7cc4 | 0d         | 0710      | 03  | 000000
    // if (strcmp("58:2d:34:11:0f:9d", advertisedDevice.getAddress().toString().c_str()) == 0)
    // if (strcmp("c4:7c:8d:6b:98:fe", advertisedDevice.getAddress().toString().c_str()) == 0)
    // if (strcmp("a4:c1:38:a6:46:04", advertisedDevice.getAddress().toString().c_str()) == 0)

    if (!advertisedDevice.haveServiceData())
    {
      return;
    }

    //Xiaomi UUID
    if (strcmp("fe95", uintToHex(advertisedDevice.getServiceDataUUID().getNative()->uuid.uuid16).c_str()) != 0)
    {
      return;
    }

    std::string raw = advertisedDevice.getServiceData();
    uint8_t *data = reinterpret_cast<uint8_t *>((char *)raw.c_str());

    char *sensorTypeHex = dataToHex(data, 2, 2);

    if (!XIAOMI_SUPPORTED_SENSORS.count(sensorTypeHex))
    {
      //parseAdvertisement(advertisedDevice.getPayload(), advertisedDevice.getPayloadLength());
      return;
    }

    // Serial.printf("\nSensorType: %s\n", sensorTypeHex);

    //char *mac = dataToHex(data, 5, 6);
    uint8_t controlFrame = data[0];

    bool isEncoded = controlFrame & hexToUint("08");
    bool hasData = controlFrame & hexToUint("40");
    if (isEncoded || !hasData)
    {
      return;
    }
    bool hasCapability = controlFrame & hexToUint("20");
    uint dataIndex = hasCapability ? 12 : 11;

    char *valueTypeHex = dataToHex(data, dataIndex, 2);
    uint8_t valueLen = data[dataIndex + 2];
    byte valueIndex = dataIndex + 3;
    char *valueHex = dataToHex(data, valueIndex, valueLen);

    float realValue = 0;
    const char *valueType = XIAOMI_VALUE_TYPES[valueTypeHex].c_str();
    const char *sensorType = XIAOMI_SUPPORTED_SENSORS[sensorTypeHex].c_str();
    switch (valueLen)
    {
    case 1:
      realValue = data[valueIndex];
      callback(sensorType, advertisedDevice.getAddress(), valueType, realValue);
      break;
    case 2:
      realValue = data[valueIndex + 1] << 8 | data[valueIndex];
      if (strcmp("formaldehyde", valueType) == 0)
      {
        realValue /= 100;
      }
      if (strcmp("temperature", valueType) == 0)
      {
        realValue /= 10;
      }
      if (strcmp("humidity", valueType) == 0)
      {
        realValue /= 10;
      }
      callback(sensorType, advertisedDevice.getAddress(), valueType, realValue);
      break;
    case 3:
      realValue = data[valueIndex + 2] << 16 | data[valueIndex + 1] << 8 | data[valueIndex];
      callback(sensorType, advertisedDevice.getAddress(), valueType, realValue);
      break;
    case 4:
      realValue = data[valueIndex + 1] << 8 | data[valueIndex];
      realValue /= 10.; //temp
      callback(sensorType, advertisedDevice.getAddress(), "temperature", realValue);

      realValue = data[valueIndex + 3] << 8 | data[valueIndex + 2];
      realValue /= 10.; //humi
      callback(sensorType, advertisedDevice.getAddress(), "humidity", realValue);
      break;
    }
    free(valueTypeHex);
    free(valueHex);
    free(sensorTypeHex);
  }
};

class Wand
{
public:
  Wand(ADV_CALLBACK_SIGNATURE callback)
  {
    advertisedDeviceCallback = new MyAdvertisedDeviceCallbacks(callback);
  }
  void init()
  {
    BLEDevice::init("");
    BLEScan *pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(advertisedDeviceCallback, true);
    pBLEScan->setActiveScan(false); //active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(100); // less or equal setInterval value

    //Scan forever
    pBLEScan->start(0, nullptr, true);
  }
  // void setAdvertiseCallback(ADV_CALLBACK_SIGNATURE callback)
  // {
  // advertisedDeviceCallback->setAdvertiseCallback(callback);
  // }

private:
  MyAdvertisedDeviceCallbacks *advertisedDeviceCallback;
};

} // namespace WAND
