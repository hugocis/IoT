/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */

#include "Arduino.h"

void IRAM_ATTR debounce();

const int flashbutton = 0;
volatile bool Bflag = false;
int counter = 0;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;
bool ledstate = LED_BUILTIN;
enum Estados{ //Estados
    Fijo,
    Apagado
  };
Estados estado_actual = Fijo;

void setup() {
  pinMode(LED_BUILTIN,OUTPUT);
  pinMode(flashbutton, INPUT_PULLUP);
  digitalWrite(LED_BUILTIN, HIGH);
  attachInterrupt(digitalPinToInterrupt(flashbutton), debounce, FALLING); //Interrupción
  Serial.begin(115200);
}

void loop()
{
  switch (estado_actual)
  {
  case Apagado:
      digitalWrite(LED_BUILTIN,HIGH);
      if(Bflag){
        estado_actual = Fijo;
        Bflag = false;
      }
    break;

  case Fijo:
      digitalWrite(LED_BUILTIN,LOW);
      if(Bflag){
        estado_actual = Apagado;
        Bflag = false;
      }
    break;
  
  default:
  break;
  }
  
  Serial.println(estado_actual);
}

void IRAM_ATTR debounce() {
  if (millis() - lastDebounceTime > debounceDelay) {
    Bflag = true;
    lastDebounceTime = millis();
  }
}


