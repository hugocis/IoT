/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */
#include "Arduino.h"
#include <Ticker.h>

void Blink();

bool estado = true;

Ticker ticker;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  ticker.attach(2, Blink);
  Serial.begin(115200);
}

void loop()
{
  if(estado){
  Serial.println("Estado del Led: OFF");
  }else{
  Serial.println("Estado del Led: ON");
  }
}

void Blink() {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  estado = !estado;
}