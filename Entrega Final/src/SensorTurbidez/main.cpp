#include <Arduino.h> 

#define TURBIDITY_PIN 34 // Definimos el pin en el que se conecta el sensor de turbidez

// Estas son una serie de variables que utilizaremos para calcular una media de los valores detectados por el sensor (de 30 en 30)
const int numReadings = 30;
int readings[numReadings];
int readIndex = 0;
long total = 0;
float average = 0;

// Definimos los estados del código
enum Estado { ESPERANDO, LEYENDO, ACTUALIZANDO, CALCULANDO, IMPRIMIENDO };
Estado estadoActual = ESPERANDO; // Iniciamos con el primer estado del programa

// Variable para almacenar el voltaje calculado a partir de las lecturas
float voltage = 0.0;

void setup() {
  Serial.begin(115200);
  pinMode(TURBIDITY_PIN, INPUT);

  // Vaciamos el array
  for (int i = 0; i < numReadings; i++) {
    readings[i] = 0;
  }
}

void loop() {
  switch (estadoActual) {
    case ESPERANDO: // Espera
      estadoActual = LEYENDO;
      break;

    case LEYENDO: // Lectura del sensor
      readings[readIndex] = analogRead(TURBIDITY_PIN);
      estadoActual = ACTUALIZANDO;
      break;

    case ACTUALIZANDO: // Se suma el valor nuevo a la lista para hacer la media
      total -= readings[readIndex];
      total += readings[readIndex];
      readIndex = (readIndex + 1) % numReadings;
      estadoActual = CALCULANDO;
      break;

    case CALCULANDO: // Valcula el voltage promedio
      average = total / numReadings;
      voltage = average * (3.3 / 4095.0);
      estadoActual = IMPRIMIENDO;
      break;

    case IMPRIMIENDO: // Se muestra en terminal el resultado
      Serial.print("Average Analog Value: ");
      Serial.print(average);
      Serial.print(" | Voltage: ");
      Serial.println(voltage);
      estadoActual = ESPERANDO;
      break;
  }
}
