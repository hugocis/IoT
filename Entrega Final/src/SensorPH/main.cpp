#include <Arduino.h>
#define PH_PIN 32           // Pin donde está conectado PO
// Definir los puntos de calibración
float voltage_at_ph7 = 2.5; // Voltaje medido a pH 7 (ajustar tras calibración)
float voltage_at_ph4 = 3.0; // Voltaje medido a pH 4 (ajustar tras calibración)

// Calcular PH_step (pendiente del voltaje por unidad de pH)
float ph_step = (voltage_at_ph7 - voltage_at_ph4) / (7.0 - 4.0); // Diferencia de voltaje dividido por rango de pH

// Definir el número de lecturas para el promedio
const int numReadings = 10; 
float readings[numReadings]; // Array para almacenar lecturas
int currentReading = 0;      // Índice de la lectura actual

void setup() {
  Serial.begin(115200);
  // Inicializar el array de lecturas en 0
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
}

void loop() {
  // Leer el valor analógico del sensor
  int adcValue = analogRead(32); 
  double voltage = adcValue * 3.3 / 4095.0; // Convertir ADC a voltaje

  // Guardar la lectura en el array
  readings[currentReading] = voltage;
  currentReading = (currentReading + 1) % numReadings; // Incrementar el índice circularmente

  // Calcular la media de las lecturas
  float voltageSum = 0;
  for (int i = 0; i < numReadings; i++) {
    voltageSum += readings[i];
  }
  float averageVoltage = voltageSum / numReadings;

  // Calcular el pH basado en la media del voltaje
  float pH = 7.0 - ((voltage_at_ph7 - averageVoltage) / ph_step);

  // Mostrar valores en el Monitor Serial
  Serial.print("Voltaje promedio: ");
  Serial.print(averageVoltage, 3); // Voltaje promedio con 3 decimales
  Serial.print(" V, pH: ");
  Serial.println(pH, 2); // Valor de pH con 2 decimales

  delay(1000); // Esperar 1 segundo entre lecturas
}
