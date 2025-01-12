#include <Arduino.h>
#define PH_PIN 32

float voltage_at_ph7 = 2.5; // Calibración pH 7
float voltage_at_ph4 = 3.0; // Calibración pH 4

float ph_step = (voltage_at_ph7 - voltage_at_ph4) / (7.0 - 4.0);

// Lecturas medias
const int numReadings = 10; 
float readings[numReadings];
int currentReading = 0;

enum Estado { ESPERANDO, LEYENDO, CALCULANDO, IMPRIMIENDO };
Estado estadoActual = ESPERANDO;

int adcValue = 0;
float voltage = 0.0;
float averageVoltage = 0.0;
float pH = 0.0;

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
}

void loop() {
  switch (estadoActual) {
    case ESPERANDO:
      estadoActual = LEYENDO;
      break;

    case LEYENDO:
      adcValue = analogRead(PH_PIN);
      voltage = adcValue * 3.3 / 4095.0;
      readings[currentReading] = voltage;
      currentReading = (currentReading + 1) % numReadings;
      estadoActual = CALCULANDO;
      break;

    case CALCULANDO:
      // Transformación: cálculo del pH basado en el voltaje promedio y los puntos de calibración´
      float voltageSum = 0;
      for (int i = 0; i < numReadings; i++) {
        voltageSum += readings[i];
      }
      averageVoltage = voltageSum / numReadings;
      pH = 7.0 - ((voltage_at_ph7 - averageVoltage) / ph_step);
      estadoActual = IMPRIMIENDO;
      break;

    case IMPRIMIENDO:
      Serial.print("Voltaje promedio: ");
      Serial.print(averageVoltage, 3);
      Serial.print(" V, pH: ");
      Serial.println(pH, 2);
      estadoActual = ESPERANDO;
      break;
  }
}
