#include <Arduino.h>
#define ORP_PIN 33           // Pin ADC donde está conectado el sensor
#define ADC_RESOLUTION 4095  // Resolución del ADC (12 bits para ESP32)
#define VREF 3.3             // Voltaje de referencia del ADC en el ESP32
#define OFFSET 150           // Offset de calibración del sensor ORP (ajustable)

void setup() {
  Serial.begin(115200);
  analogReadResolution(12); // Configura el ADC a 12 bits
  Serial.println("Lectura del sensor ORP iniciada...");
}

void loop() {
  // Leer el valor analógico del sensor ORP
  int adcValue = analogRead(ORP_PIN); 

  // Convertir el valor ADC a voltaje (en voltios)
  float voltage = (adcValue / float(ADC_RESOLUTION)) * VREF;

  // Convertir el voltaje a milivoltios y aplicar el offset de calibración
  float orpValue = (voltage * 1000) - OFFSET; // ORP en mV

  // Imprimir los valores en el Monitor Serial
  Serial.print("ADC Value: ");
  Serial.print(adcValue);
  Serial.print(", Voltage: ");
  Serial.print(voltage, 3); // Voltaje con 3 decimales
  Serial.print(" V, ORP: ");
  Serial.print(orpValue, 1); // ORP en mV con 1 decimal
  Serial.println(" mV");

  delay(2000); // Esperar 1 segundo entre lecturas
}
