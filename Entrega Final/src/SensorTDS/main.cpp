#include <Arduino.h> 

#define TDS_PIN 35 // Definimos el pin en el que se conecta el sensor TDS

const float TdsFactor = 0.5; // Factor de conversión para calcular TDS en ppm
const int VREF = 3300; // Voltaje de referencia
const int ADC_RESOLUTION = 4095; // Resolución del ADC del ESP32 (12 bits)

// Definimos los estados del código
enum Estado { ESPERANDO, LEYENDO, CALCULANDO, IMPRIMIENDO }; 
Estado estadoActual = ESPERANDO; // Iniciamos con el primer estado del programa

// Variables globales para almacenar datos de lectura y cálculo
int analogValue = 0; // Valor analógico leído del sensor
float voltage = 0.0; // Voltaje calculado en milivoltios
float tdsValue = 0.0; // Valor TDS calculado en ppm

void setup() {
  Serial.begin(115200);
  pinMode(TDS_PIN, INPUT);
  Serial.println("Sensor TDS conectado. Leyendo valores..."); 
}

void loop() {
  switch (estadoActual) {
    case ESPERANDO: // Espera
      estadoActual = LEYENDO;
      break;

    case LEYENDO: // Lectura del sensor
      analogValue = analogRead(TDS_PIN);
      estadoActual = CALCULANDO;
      break;

    case CALCULANDO: // Calculamps el voltage y el valor TDS final
      voltage = (analogValue * VREF) / ADC_RESOLUTION;
      tdsValue = (voltage / TdsFactor) * 1000;
      estadoActual = IMPRIMIENDO;
      break;

    case IMPRIMIENDO: // Se muestra en terminal el resultado
      Serial.print("TDS (ppm): ");
      Serial.println(tdsValue);
      estadoActual = ESPERANDO;
      break;
  }
}
