#include <Arduino.h>
#define TDS_PIN 35 // Pin ADC para conectar AOUT del sensor

// Configuración de parámetros
const float TdsFactor = 0.5;  // Factor de conversión TDS
const int VREF = 3300;        // Voltaje de referencia en mV (3.3V)
const int ADC_RESOLUTION = 4095; // Resolución ADC del ESP32 (12 bits)

void setup() {
  Serial.begin(115200);
  pinMode(TDS_PIN, INPUT); // Configura el pin como entrada
  Serial.println("Sensor TDS conectado. Leyendo valores...");
}

void loop() {
  // Leer el valor analógico del pin
  int analogValue = analogRead(TDS_PIN);
  
  // Convertir a voltaje en mV
  float voltage = (analogValue * VREF) / ADC_RESOLUTION;

  // Calcular TDS en ppm
  float tdsValue = (voltage / TdsFactor) * 1000;

  // Imprimir resultados
  Serial.print("Valor Analógico: ");
  Serial.println(analogValue);
  Serial.print("Voltaje (mV): ");
  Serial.println(voltage);
  Serial.print("TDS (ppm): ");
  Serial.println(tdsValue);
  Serial.println("-----------------------");
  
  delay(1000); // Esperar 1 segundo antes de la siguiente lectura
}
