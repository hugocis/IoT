#include <Arduino.h>
#define ORP_PIN 33
#define ADC_RESOLUTION 4095
#define VREF 3.3
#define OFFSET 150

enum State { WAITING, READING, PROCESSING, PRINTING };
State currentState = WAITING;

int adcValue = 0;
float voltage = 0.0;
float orpValue = 0.0;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  Serial.println("Lectura del sensor ORP iniciada...");
}

void loop() {
  switch (currentState) {
    case WAITING:
      currentState = READING;
      break;

    case READING:
      adcValue = analogRead(ORP_PIN);
      currentState = PROCESSING;
      break;

    case PROCESSING:
      // Transformación: convierte el valor ADC (0-4095) a voltaje (0-3.3V) y luego a ORP en mV
      voltage = (adcValue / float(ADC_RESOLUTION)) * VREF;
      orpValue = (voltage * 1000) - OFFSET;
      currentState = PRINTING;
      break;

    case PRINTING:
      Serial.print("ORP: ");
      Serial.print(orpValue, 1);
      Serial.println(" mV");
      currentState = WAITING;
      break;
  }
}
