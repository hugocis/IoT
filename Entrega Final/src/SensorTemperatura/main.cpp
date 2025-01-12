#include <OneWire.h> // Biblioteca para manejar el protocolo OneWire
#include <DallasTemperature.h> // Biblioteca para sensores de temperatura Dallas DS18B20

#define ONE_WIRE_BUS 4 // Definimos el pin en el que se conecta el sensor

// configuramos el bus OneWire y el sensor DS18B20
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Definimos los estados del código
enum Estado { ESPERANDO, LEYENDO, IMPRIMIENDO };
Estado estadoActual = ESPERANDO; // Iniciamos con el primer estado del programa

// Variable para almacenar la temperatura medida
float tempC = 0.0;

void setup() {
  Serial.begin(115200);
  sensors.begin(); // Inicializamos el sensor DS18B20
  Serial.println("Buscando sensores DS18B20..."); 
}

void loop() {
  switch (estadoActual) {
    case ESPERANDO: // Espera
      estadoActual = LEYENDO;
      break;

    case LEYENDO: // Lee el valor del sensory lo ajusta 
      sensors.requestTemperatures();
      tempC = sensors.getTempCByIndex(0) - 0.4;
      estadoActual = IMPRIMIENDO;
      break;

    case IMPRIMIENDO: // Se muestra en terminal el resultado
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

