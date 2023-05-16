#include <Arduino.h>
#include <WiFiClientSecure.h>
#include <SPIFFS.h>
#include "at_client.h"
#include "constants.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

// analog input pin for Soil Sensor (P32)
const int analogSoilPin = 32;
// analog input pin for light sensor (P35);
const int analogLightPin = 35;
// digital input pin for DHT11 (hum/temp) (P26)
const int DHTPIN = 26;

// initializing the dht sensor
DHT dht(DHTPIN, DHT11);

// intitializing AtClient variable
AtClient *at_client;

// initializing AtKeys for temperature, soil, humidity and light
AtKey *at_keyTemp;
AtKey *at_keySoil;
AtKey *at_keyHumid;
AtKey *at_keyLight;

void setup()
{
  dht.begin();
  delay(2000);
  Serial.begin(115200);

  // assigning key  for esp32
  const auto *esp32 = new AtSign("@hilariousbaboon");
  // assigning key for java
  const auto *java = new AtSign("@unpleasantwater");
  // reads the keys on the ESP32
  const auto keys = keys_reader::read_keys(*esp32);
  // assigning the AtClient object (allows us to run operations)
  at_client = new AtClient(*esp32, keys);
  // loging in to the wifi network
  at_client->pkam_authenticate(SSID, PASSWORD);
  // assigning the keys for temperature, soil, humidy and light
  at_keyTemp = new AtKey("temp", esp32, java);
  at_keySoil = new AtKey("soil", esp32, java);
  at_keyHumid = new AtKey("humid", esp32, java);
  at_keyLight = new AtKey("light", esp32, java);
}

void loop()
{
  // read the analog input soil and light
  int soilMoistureValue = analogRead(analogSoilPin);
  int lightSensorValue = analogRead(analogLightPin);

  // read the input for temperature and humidity
  float i = 0.0;
  float_t temperatureValue = dht.readTemperature();
  float_t humidityValue = dht.readHumidity();

  // if temperature or humidity fails to read assign value of 0.0
  if (isnan(temperatureValue) || isnan(humidityValue))
  {
    temperatureValue = 0.0;
    humidityValue = 0.0;
  }

  // string to send data out for temperature, soil, humidity and light
  std::string tempValue = std::to_string(temperatureValue);
  std::string soilValue = std::to_string(soilMoistureValue);
  std::string humidValue = std::to_string(humidityValue);
  std::string lightValue = std::to_string(lightSensorValue);

  // debugging printout
  Serial.print("Temperature from temp/hum sensor: ");
  Serial.print(temperatureValue);
  Serial.println(" C");
  Serial.print("Humidity: ");
  Serial.print(humidityValue);
  Serial.println(".");
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoistureValue);
  Serial.println(".");
  Serial.print("Light: ");
  Serial.println(lightSensorValue);

  // sendig data for temp
  at_client->put_ak(*at_keyTemp, tempValue);
  // sending data for soil
  at_client->put_ak(*at_keySoil, soilValue);
  // sending data for humidity
  at_client->put_ak(*at_keyHumid, humidValue);
  // sending data for light
  at_client->put_ak(*at_keyLight, lightValue);
  delay(10000);
}