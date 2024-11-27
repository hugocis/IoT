// -----------------------------------------------------------------------------------------------------
// Proyecto: Control de brillo de un LED con efecto de fundido y detección de pulsación de botón a través de estados.
// Autor: Hugo Cisneros Amengual, 4ºA IoT
// -----------------------------------------------------------------------------------------------------
#include <arduino.h>  
#include <Ticker.h>  // Librería para manejar el temporizador Ticker

// -----------------------------------------------------------------------------------------------------
//       Pines del sistema
// -----------------------------------------------------------------------------------------------------
const int ledPin = D4;      // Pin del LED
const int buttonPin = D3;   // Pin del botón

// -----------------------------------------------------------------------------------------------------
//       Variables de control
//       Info para el ticker: https://forum.arduino.cc/t/how-to-use-ticker-correctly/1021125 y porque uso funciones
// -----------------------------------------------------------------------------------------------------
Ticker pwmTicker;                     // Ticker para controlar el ciclo PWM del LED
volatile bool buttonPressed = false;  // Variable para detectar si el botón fue presionado (usada en la ISR)
int pwmValue = 0;                     // Valor del PWM para controlar el brillo del LED
bool increasing = true;               // Dirección del fundido (aumentando o disminuyendo brillo)
int fadeSpeed = 1;                   // Velocidad de variación del brillo

// -----------------------------------------------------------------------------------------------------
//       Variables estados
// -----------------------------------------------------------------------------------------------------
bool estadoBoton = false;    // Estado del botón (presionado o no presionado)
bool estadoPWM = true;       // Estado del ciclo PWM (aumentando o disminuyendo)

// -----------------------------------------------------------------------------------------------------
//       Función: handleButtonPress (Interrupción del botón)
//       Descripción: Detecta la pulsación del botón y cambia su estado.
//       Info del uso de ICACHE_RAM_ATTR y volatile : https://arduinokitproject.com/handling-esp8266-gpio-interrupts-tutorial/
// -----------------------------------------------------------------------------------------------------
void IRAM_ATTR handleButtonPress() 
{
  buttonPressed = true;  // Marca que el botón fue presionado (se usa volatile por que se usa en la ISR y ICACHE_RAM_ATTR para que se ejecute en la RAM de la placa)
}

// -----------------------------------------------------------------------------------------------------
//       Función: adjustPWM
//       Descripción: Ajusta el valor PWM del LED según el estado actual (aumentando o disminuyendo brillo).
//       Info: https://arduinokitproject.com/esp8266-pwm-arduino-ide/
// -----------------------------------------------------------------------------------------------------
void adjustPWM() 
{
  // Ajusta el valor PWM dependiendo del estado de incremento o decremento
  if (estadoPWM) 
  {
    pwmValue += fadeSpeed;

    if (pwmValue >= 1023) 
    {
      pwmValue = 1023;
      estadoPWM = false;  // Cambia el estado a "disminuir brillo"
    }

  } 
  else 
  {
    pwmValue -= fadeSpeed;

    if (pwmValue <= 0) 
    {
      pwmValue = 0;
      estadoPWM = true;  // Cambia el estado a "aumentar brillo"
    }

  }
 
  analogWrite(ledPin, pwmValue);  // Actualiza el brillo del LED
}

// -----------------------------------------------------------------------------------------------------
//       Función: cambiarVelocidad
//       Descripción: Cambia la velocidad del fundido del LED según el estado del botón.
//       Para info del porque de las velocidades: https://forum.arduino.cc/t/setting-pwm-frequency-5hz/908715/4
// -----------------------------------------------------------------------------------------------------
void cambiarVelocidad() 
{
  if (estadoBoton) 
  {  
    // Si el botón está presionado, modo normal
    fadeSpeed = 5;
    Serial.println("Modo normal activado. Velocidad de fundido: 5");
  } 
  else 
  {           
    // Si el botón no está presionado, modo rápido
    fadeSpeed = 20;
    Serial.println("Modo rápido activado. Velocidad de fundido: 20");
  }
}

// -----------------------------------------------------------------------------------------------------
//       Función: setup
//       Descripción: Configura los pines, inicializa la interrupción del botón y el Ticker PWM.
// -----------------------------------------------------------------------------------------------------
void setup() 
{
  // Inicializa el monitor serie
  Serial.begin(115200);
  // Serial.println("Iniciando sistema...");

  // Configura el pin del LED como salida
  pinMode(ledPin, OUTPUT);
  Serial.println("Pin del LED configurado como salida.");

  // Configura el pin del botón como entrada con pull-up
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.println("Pin del botón configurado como entrada con pull-up interno.");

  // Configuración de la interrupción para detectar cuando se presiona el botón
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonPress, FALLING);
  Serial.println("Interrupción del botón configurada.");

  // Configuración del Ticker para ajustar el PWM del LED
  pwmTicker.attach_ms(2, adjustPWM);  // Ejecuta la función adjustPWM cada 2 ms para controlar el fundido
  Serial.println("Ticker PWM iniciado, ajustando brillo del LED cada 20 ms.");
  Serial.println("Sistema listo.\n");
}

// -----------------------------------------------------------------------------------------------------
//       Función: loop
//       Descripción: Detecta cambios de estado según la pulsación del botón y ajusta la velocidad del fundido.
// -----------------------------------------------------------------------------------------------------
void loop() 
{
  if (buttonPressed) 
  {  
    // Si el botón fue presionado
    buttonPressed = false;  // Reinicia la variable de interrupción

    if (estadoBoton) 
    {
      estadoBoton = false;
    } 
    else 
    {
      estadoBoton = true;
    }
    
    // Ajusta la velocidad del fundido dependiendo del nuevo estado del botón
    cambiarVelocidad();
    
    Serial.print("Cambio de estado del led: ");
    
    if (estadoBoton) 
    {
      Serial.println("Normal");
    } 
    else 
    {
      Serial.println("Rápido");
    } 
  }
}