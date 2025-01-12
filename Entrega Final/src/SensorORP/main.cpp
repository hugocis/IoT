#include <Arduino.h> 

#define ORP_PIN 33 // Definimos el pin en el que se conecta el sensor
#define ADC_RESOLUTION 4095 // Resolución máxima del ADC del microcontrolador (12 bits)
#define VREF 3.3 // Voltaje de referencia del microcontrolador
#define OFFSET 150 // Desplazamiento para ajustar la lectura del sensor ORP

// Definimos los estados del código
enum State { WAITING, READING, PROCESSING, PRINTING }; 
State currentState = WAITING; // Iniciamos con el primer estado del programa

// Variables para almacenar los datos del sensor
int adcValue = 0;
float voltage = 0.0;
float orpValue = 0.0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // Configuramos la resolución del ADC en 12 bits
  Serial.println("Lectura del sensor ORP iniciada...");
}

void loop() {
  switch (currentState) {
    case WAITING: // Espera
      currentState = READING;
      break;

    case READING: // Obtenemos el valor del sensor
      adcValue = analogRead(ORP_PIN);
      currentState = PROCESSING;
      break;

    case PROCESSING: // Procesamos el valor obtenido
      voltage = (adcValue / float(ADC_RESOLUTION)) * VREF;
      orpValue = (voltage * 1000) - OFFSET;
      currentState = PRINTING;
      break;

    case PRINTING: // Mostramso el resultado
      Serial.print("ORP: ");
      Serial.print(orpValue, 1);
      Serial.println(" mV");
      currentState = WAITING;
      break;
  }
}