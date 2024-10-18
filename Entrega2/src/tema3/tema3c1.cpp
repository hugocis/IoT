#include <SPI.h>
#include <Ticker.h>
#include <Arduino.h>


const int ssPin = 15; 
Ticker ticker;
byte dataToSend = 0xFF; 

void sendSPIData(); 


void setup() {
  pinMode(ssPin, OUTPUT);

  Serial.begin(115200);
  
  SPI.begin();

  Serial.println("SPI Loopback Test iniciado.");

  ticker.attach_ms(10, sendSPIData); 
}

void sendSPIData() {
  if (Serial.available() > 0) {
    dataToSend = Serial.read();  
  } else {
    dataToSend = 0xFF; 
  }

  digitalWrite(ssPin, LOW);

  byte receivedData = SPI.transfer(dataToSend);

  digitalWrite(ssPin, HIGH);

  if (receivedData != 0xFF) {
    Serial.print("Recibido: ");
    Serial.println(receivedData, HEX);
  }
}

void loop() {
}