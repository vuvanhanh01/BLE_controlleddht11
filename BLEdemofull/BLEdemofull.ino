#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <string>
#include <BLEServer.h>
#include <DHT.h>

#define TEMPERATURE_SERVICE_UUID "de1bf7ab-1ca8-40a3-b797-6221c2acb33d"
#define TEMPERATURE_CHARACTERISTIC_UUID "3559f95b-3857-43f1-a7e0-cc0ab0542afc"

#define HUMIDITY_SERVICE_UUID "b968a0b8-448b-439f-a9a9-9075cb3a57af"
#define HUMIDITY_CHARACTERISTIC_UUID "77eaa8c7-92ba-44c9-bb88-f2173df3e0b3"

#define LED1_SERVICE_UUID "48e8dc85-aa95-472c-9c21-88d0f4695b5f"
#define LED1_CHARACTERISTIC_UUID "a8b5682c-389a-46c9-9d83-fae7cf50ee6f"

#define LED2_SERVICE_UUID "a01c7ec4-f962-4c9d-9d4d-d90fbd1f0b35"
#define LED2_CHARACTERISTIC_UUID "6c83269a-9f9c-4ed3-b4a4-4e0389d9cb6b"

#define DHTPIN 4 // replace with the pin number to which the DHT11 sensor is connected
#define DHTTYPE DHT11 // replace with the DHT11 or DHT22 sensor type
#define Led1 15
#define Led2 21

DHT dht(DHTPIN, DHTTYPE);
BLECharacteristic* pTemperatureCharacteristic; 
BLECharacteristic* pHumidityCharacteristic;
BLECharacteristic* pLed1Characteristic;
BLECharacteristic* pLed2Characteristic;

class callBackBLE : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0)
        {
            Serial.println("**********");
            Serial.print("Received Value : ");
            for (int i = 0; i <= rxValue.length(); i++)
            {
                Serial.print(rxValue[i]);
            }
            Serial.println();
            Serial.println("**********");
        }

        if (pCharacteristic == pLed1Characteristic)
        {
            if (rxValue == "1")
            {
                Serial.println("LED1_ON");
                digitalWrite(Led1, HIGH);
            }

            if (rxValue == "0")
            {
                Serial.println("LED1_OFF");
                digitalWrite(Led1, LOW);
            }
        }

        if (pCharacteristic == pLed2Characteristic)
        {
            if (rxValue == "1")
            {
                Serial.println("LED2_ON");
                digitalWrite(Led2, HIGH);
            }

            if (rxValue == "0")
            {
                Serial.println("LED2_OFF");
                digitalWrite(Led2, LOW);
            }
        }
    }
};
void setup() {
  Serial.begin(115200);
  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  digitalWrite(Led1, LOW);
  digitalWrite(Led2, LOW);
  
  dht.begin();
  
  BLEDevice::init("ESP32_BLE_DHT11");
  BLEServer *pServer = BLEDevice::createServer();

  // Create the temperature service
  BLEService *pTemperatureService = pServer->createService(TEMPERATURE_SERVICE_UUID);

  // Create the temperature characteristic
  pTemperatureCharacteristic = pTemperatureService->createCharacteristic(
      TEMPERATURE_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ
  );

  // Create the humidity service
  BLEService *pHumidityService = pServer->createService(HUMIDITY_SERVICE_UUID);

  // Create the humidity characteristic
  pHumidityCharacteristic = pHumidityService->createCharacteristic(
      HUMIDITY_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_READ
  );

  // Create the LED1 service
  BLEService *pLed1Service = pServer->createService(LED1_SERVICE_UUID);

  // Create the LED1 characteristic
  pLed1Characteristic = pLed1Service->createCharacteristic(
      LED1_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_WRITE
  );
  pLed1Characteristic->setCallbacks(new callBackBLE());

  // Create the LED2 service
  BLEService *pLed2Service = pServer->createService(LED2_SERVICE_UUID);

  // Create the LED2 characteristic
  pLed2Characteristic = pLed2Service->createCharacteristic(
      LED2_CHARACTERISTIC_UUID,
      BLECharacteristic::PROPERTY_WRITE
  );
  pLed2Characteristic->setCallbacks(new callBackBLE());

  // Start the services
  pTemperatureService->start();
  pHumidityService->start();
  pLed1Service->start();
  pLed2Service->start();

  // Set the advertising parameters
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(TEMPERATURE_SERVICE_UUID);
  pAdvertising->addServiceUUID(HUMIDITY_SERVICE_UUID);
  pAdvertising->addServiceUUID(LED1_SERVICE_UUID);
  pAdvertising->addServiceUUID(LED2_SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMaxPreferred(0x12);
  BLEDevice::startAdvertising();

}

void loop() {
  // Read data from DHT11 sensor
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  // Check if any readings failed
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from DHT11 sensor!");
    return;
  }

  // Convert the temperature value to a string
  char temperatureStr[30];
  snprintf(temperatureStr, sizeof(temperatureStr), "Temperature: %.2f °C", temperature);

  // Update the temperature characteristic value
  pTemperatureCharacteristic->setValue((uint8_t*)temperatureStr, strlen(temperatureStr));

  // Convert the humidity value to a string
  char humidityStr[20];
  snprintf(humidityStr, sizeof(humidityStr), "Humidity: %.2f %%", humidity);

  // Update the humidity characteristic value
  pHumidityCharacteristic->setValue((uint8_t*)humidityStr, strlen(humidityStr));

  // Print the current temperature and humidity values to the Serial Monitor
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" °C, Humidity: ");
  Serial.print(humidity);
  Serial.println(" %");

  // Wait for some time before the next update
  delay(1000);
}


