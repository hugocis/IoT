#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4  // Pin GPIO donde está conectado el sensor

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void setup() {
  Serial.begin(115200);
  Serial.println("Buscando sensores DS18B20...");
  sensors.begin();
}

void loop() {
  int deviceCount = sensors.getDeviceCount(); // Número de sensores detectados

  if (deviceCount == 0) {
    Serial.println("Error: No se detectaron sensores DS18B20.");
  } else {
    Serial.print("Número de sensores detectados: ");
    Serial.println(deviceCount);
  }

  sensors.requestTemperatures();
  float tempC = sensors.getTempCByIndex(0); // Lee temperatura del primer sensor

  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Error: Sensor DS18B20 desconectado.");
  } else {
    Serial.print("Temperatura = ");
    Serial.print(tempC);
    Serial.println(" °C");
  }

  delay(1000); // Leer cada segundo
}
