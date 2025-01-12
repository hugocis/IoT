#include <Arduino.h>

#define PH_PIN 32 // Definimos el pin en el que se conecta el sensor de pH

// Calibramos los límites del sensor
float voltage_at_ph7 = 2.5; // Voltaje correspondiente a pH 7
float voltage_at_ph4 = 3.0; // Voltaje correspondiente a pH 4

// Este valor se calcula en funcion de los voltages de diferentes niveles de pH conocidos para poder calcular más adelante los valores que obtengamos
float ph_step = (voltage_at_ph7 - voltage_at_ph4) / (7.0 - 4.0);

// Estas variables se utilizan para hacer un promedio de los voltages obtenidos por el sensor (de 10 en 10)
const int numReadings = 10;
float readings[numReadings];
int currentReading = 0;

// Definimos los estados del código
enum Estado { ESPERANDO, LEYENDO, CALCULANDO, IMPRIMIENDO };
Estado estadoActual = ESPERANDO; // Iniciamos con el primer estado del programa

// Variables globales para almacenar datos de lectura y cálculo
int adcValue = 0; // Valor analógico leído del sensor
float voltage = 0.0; // Voltaje calculado
float averageVoltage = 0.0; // Voltaje promedio calculado
float pH = 0.0; // Valor de pH calculado

void setup() {
  Serial.begin(115200); 
  
  for (int i = 0; i < numReadings; i++) { // Vaciamos el array
    readings[i] = 0;
  }
}

void loop() {
  switch (estadoActual) {
    case ESPERANDO: // Espera
      estadoActual = LEYENDO;
      break;

    case LEYENDO: // Lectura del sensor, en este estado se calcula el voltage detectado y se guarda para despues hacer la media
      adcValue = analogRead(PH_PIN);
      voltage = adcValue * 3.3 / 4095.0;
      readings[currentReading] = voltage;
      currentReading = (currentReading + 1) % numReadings;
      estadoActual = CALCULANDO;
      break;

    case CALCULANDO: // En este estado se suman los voltages y se calcula al media para obtener el nivel de pH
      float voltageSum = 0;
      for (int i = 0; i < numReadings; i++) {
        voltageSum += readings[i];
      }
      averageVoltage = voltageSum / numReadings;
      pH = 7.0 - ((voltage_at_ph7 - averageVoltage) / ph_step);
      estadoActual = IMPRIMIENDO;
      break;

    case IMPRIMIENDO: // Se muestra en terminal el resultado
      Serial.print("Voltaje promedio: ");
      Serial.print(averageVoltage, 3);
      Serial.print(" V, pH: ");
      Serial.println(pH, 2);
      estadoActual = ESPERANDO;
      break;
  }
}
