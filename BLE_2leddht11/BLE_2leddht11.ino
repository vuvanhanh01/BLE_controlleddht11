#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <DHT.h>

#define SERVICE_UUID "de1bf7ab-1ca8-40a3-b797-6221c2acb33d"
#define CHARACTERISTIC_UUID "3559f95b-3857-43f1-a7e0-cc0ab0542afc"
#define DHTPIN 4 // replace with the pin number to which the DHT11 sensor is connected
#define DHTTYPE DHT11 // replace with the DHT11 or DHT22 sensor type
#define Led1 15
#define Led2 21

DHT dht(DHTPIN, DHTTYPE);
BLEService* pService;
BLECharacteristic* pCharacteristic; 

class callBackBLE : public BLECharacteristicCallbacks
{
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0)
        {
            Serial.println("**********");
            Serial.print("Received Value : ");
            for (int i = 0; i < rxValue.length(); i++)
            {
                Serial.print(rxValue[i]);
            }
            Serial.println();
            Serial.println("**********");
        }

        if (rxValue == "ON1")
        {
            Serial.println("LED1_ON");
            digitalWrite(Led1, HIGH);
        }

        if (rxValue == "OFF1")
        {
            Serial.println("LED1_OFF");
            digitalWrite(Led1, LOW);
        }

        if (rxValue == "ON2")
        {
            Serial.println("LED2_ON");
            digitalWrite(Led2, HIGH);
        }

        if (rxValue == "OFF2")
        {
            Serial.println("LED2_OFF");
            digitalWrite(Led2, LOW);
        }
    }
};

void setup()
{
    Serial.begin(9600);

    pinMode(Led1, OUTPUT);
    pinMode(Led2, OUTPUT);

    // Initialize BLE
    BLEDevice::init("ESP32_BLE LED AND DHT11");
    BLEServer *pServer = BLEDevice::createServer();
    pService = pServer->createService(SERVICE_UUID);
    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);

    // Set BLE characteristic callback
    pCharacteristic->setValue("Hello this is BLE default value");
    pCharacteristic->setCallbacks(new callBackBLE());

    // Start BLE service
    pService->start();
    BLEDevice::startAdvertising();

    // Initialize DHT sensor
    dht.begin();
    
    Serial.println("BLE and DHT11 sensor READY!!!");
}

void loop()
{
    // Read temperature and humidity from DHT sensor
    float temperature = dht.readTemperature();
    float humidity = dht.readHumidity();

    // Convert temperature and humidity to string and set as BLE characteristic value
    String data = "temperature:" + String(temperature) +"°C" + ", humidity:" + String(humidity)+"%";
    pCharacteristic->setValue(data.c_str());

    delay(1000);
}
