//----------------------------------------- PRÁCTICA 5 ------------------------------------------//
// Ejercicio 2. Haz un advertising con tu módulo siguiendo la identificación iBeacon 
//              incluyendo el número de grupo dentro del campo UUID

#include "sys/time.h"
// Se incluyen las bibliotectas ESP32 BLE
#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEServer.h"
// Se incluye la bibliotecta iBeacon que establece el ESP32 como iBeacon
#include "BLEBeacon.h" 
#include "esp_sleep.h"

#define GPIO_DEEP_SLEEP_DURATION     10  // duración del sueño para ESP32 en seg
RTC_DATA_ATTR static time_t last;        // remember last boot in RTC Memory
RTC_DATA_ATTR static uint32_t bootcount; // remember number of boots in RTC Memory

BLEAdvertising *pAdvertising;            // defino el tipo de publicidad BLE 
struct timeval now;                      // accedo a hora local

#define BEACON_UUID "87b99b2c-90fd-11e9-bc42-526af7764f64" // genero un UUID 

// función que contiene los atributos de iBeacon 
void setBeacon() {

  BLEBeacon oBeacon = BLEBeacon();
  oBeacon.setManufacturerId(0x4C00); // fake Apple 0x004C LSB (ENDIAN_CHANGE_U16!)
  oBeacon.setProximityUUID(BLEUUID(BEACON_UUID));
  oBeacon.setMajor((bootcount & 0xFFFF0000) >> 16);
  oBeacon.setMinor(bootcount & 0xFFFF);
  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  BLEAdvertisementData oScanResponseData = BLEAdvertisementData();

  oAdvertisementData.setFlags(0x04); // BR_EDR_NOT_SUPPORTED 0x04

  std::string strServiceData = ""; // Establecer datos publicitarios para su publicación.
  // Agregar una cadena tras otra para anunciar.
  strServiceData += (char)26;     // Len
  strServiceData += (char)0xFF;   // Type
  strServiceData += oBeacon.getData();
  oAdvertisementData.addData(strServiceData);
  // Iniciar la publicidad publicando datos
  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->setScanResponseData(oScanResponseData);
}

void setup() {

  Serial.begin(115200);
  gettimeofday(&now, NULL);
  Serial.printf("start ESP32 %d\n", bootcount++);
  Serial.printf("deep sleep (%lds since last reset, %lds since last boot)\n", now.tv_sec, now.tv_sec - last);
  last = now.tv_sec;  // almaceno la hora actual en la memoria

  // creo el dispositivo BLE 
  BLEDevice::init("ESP32 as iBeacon IRENE");
  // creo un servidor BLE para anunciar e iniciar publicidad.
  BLEServer *pServer = BLEDevice::createServer(); 
  pAdvertising = BLEDevice::getAdvertising();
  BLEDevice::startAdvertising();
  // configuro el ESP32 en modo iBeacon.
  setBeacon(); 
 
  pAdvertising->start();                                // empiezo a hacer publicidad, 
  Serial.println("Advertizing started...");
  delay(100);
  pAdvertising->stop();                                 // dejo de hacer publicidad 
  Serial.printf("enter deep sleep\n");
  esp_deep_sleep(1000000LL * GPIO_DEEP_SLEEP_DURATION); // duermo profundamente durante 10 segundos
  Serial.printf("in deep sleep\n");
}

void loop() {
}
