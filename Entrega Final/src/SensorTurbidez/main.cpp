#include <Arduino.h>
#define TURBIDITY_PIN 34

const int numReadings = 30;
int readings[numReadings];
int readIndex = 0;
long total = 0;
float average = 0;

enum Estado { ESPERANDO, LEYENDO, ACTUALIZANDO, CALCULANDO, IMPRIMIENDO };
Estado estadoActual = ESPERANDO;

float voltage = 0.0;

void setup() {
  Serial.begin(115200);
  pinMode(TURBIDITY_PIN, INPUT);

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
      readings[readIndex] = analogRead(TURBIDITY_PIN);
      estadoActual = ACTUALIZANDO;
      break;

    case ACTUALIZANDO:
      total -= readings[readIndex];
      total += readings[readIndex];
      readIndex = (readIndex + 1) % numReadings;
      estadoActual = CALCULANDO;
      break;

    case CALCULANDO:
      // Transformación: cálculo del promedio de las lecturas y conversión a voltaje
      average = total / numReadings;
      voltage = average * (3.3 / 4095.0);
      estadoActual = IMPRIMIENDO;
      break;

    case IMPRIMIENDO:
      Serial.print("Average Analog Value: ");
      Serial.print(average);
      Serial.print(" | Voltage: ");
      Serial.println(voltage);
      estadoActual = ESPERANDO;
      break;
  }
}
