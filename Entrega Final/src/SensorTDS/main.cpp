#include <Arduino.h>
#define TDS_PIN 35

const float TdsFactor = 0.5; // Factor de conversión TDS
const int VREF = 3300; // Voltaje de referencia en mV (3.3V)
const int ADC_RESOLUTION = 4095; // Resolución ADC del ESP32 (12 bits)

enum Estado { ESPERANDO, LEYENDO, CALCULANDO, IMPRIMIENDO };
Estado estadoActual = ESPERANDO;

int analogValue = 0;
float voltage = 0.0;
float tdsValue = 0.0;

void setup() {
  Serial.begin(115200);
  pinMode(TDS_PIN, INPUT);
  Serial.println("Sensor TDS conectado. Leyendo valores...");
}

void loop() {
  switch (estadoActual) {
    case ESPERANDO:
      estadoActual = LEYENDO;
      break;

    case LEYENDO:
      analogValue = analogRead(TDS_PIN);
      estadoActual = CALCULANDO;
      break;

    case CALCULANDO:
      // Transformación: convierte el valor ADC a voltaje en mV y calcula TDS en ppm
      voltage = (analogValue * VREF) / ADC_RESOLUTION;
      tdsValue = (voltage / TdsFactor) * 1000;
      estadoActual = IMPRIMIENDO;
      break;

    case IMPRIMIENDO:
      Serial.print("TDS (ppm): ");
      Serial.println(tdsValue);
      estadoActual = ESPERANDO;
      break;
  }
}
