/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */
#include "Arduino.h"

bool ida = true;
int intensidad = 0;

void setup()
{
  pinMode(LED_BUILTIN,OUTPUT);
}

void loop()
{
  if(ida and intensidad >= 1020){ //PWD
    ida = false;
  }else if(ida){
    intensidad++;
  }else if(intensidad<=0){
    ida = true;
  }else{
    intensidad--;
  }
  analogWrite(LED_BUILTIN,intensidad/4); //Para que vaya más lento 

  
}