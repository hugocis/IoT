#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 4

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

enum Estado { ESPERANDO, LEYENDO, IMPRIMIENDO };
Estado estadoActual = ESPERANDO;

float tempC = 0.0;

void setup() {
  Serial.begin(115200);
  sensors.begin();
  Serial.println("Buscando sensores DS18B20...");
}

void loop() {
  switch (estadoActual) {
    case ESPERANDO:
      estadoActual = LEYENDO;
      break;

    case LEYENDO:
      sensors.requestTemperatures();
      tempC = sensors.getTempCByIndex(0) - 0.4; // Transformación: Calibración del sensor restando 0.4
      estadoActual = IMPRIMIENDO;
      break;

    case IMPRIMIENDO:
      if (tempC == DEVICE_DISCONNECTED_C) {
        Serial.println("Error: Sensor DS18B20 desconectado.");
      } else {
        Serial.print("Temperatura = ");
        Serial.print(tempC);
        Serial.println(" °C");
      }
      estadoActual = ESPERANDO;
      break;
  }
}
