#include <Arduino.h>
#define TURBIDITY_PIN 34  // Pin al que conectaste el sensor (GPIO34)

void setup() {
  Serial.begin(115200); // Inicializa el monitor serie
  pinMode(TURBIDITY_PIN, INPUT); // Configura el pin como entrada
}

void loop() {
  int sensorValue = analogRead(TURBIDITY_PIN); // Lee el valor analógico del sensor
  float voltage = sensorValue * (3.3 / 4095.0); // Convierte el valor a voltaje (para ESP32, resolución ADC de 12 bits)
  
  // Imprime el valor analógico y el voltaje en el monitor serie
  Serial.print("Analog Value: ");
  Serial.print(sensorValue);
  Serial.print(" | Voltage: ");
  Serial.println(voltage);
  
  delay(1000); // Espera 1 segundo antes de la siguiente lectura
}
