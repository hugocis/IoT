#include <Arduino.h>
#include <Ticker.h>
#include <Wire.h>
#include <MPU6050.h>

const int ldrPin = A0;       // Pin analógico para el LDR
const int ledPin = 2;        // Pin del LED integrado en la placa (GPIO2)
Ticker tickerLDR;            // Ticker para tomar muestras del LDR cada 10 ms
Ticker tickerMovimiento;     // Ticker para detectar movimiento cada 100 ms
Ticker tickerSerial;         // Ticker para enviar lecturas a la consola cada 1 segundo

int16_t axPrev = 0, ayPrev = 0, azPrev = 0; // Variables para almacenar las lecturas anteriores
int16_t ax, ay, az;                          // Variables actuales del acelerómetro
MPU6050 mpu;                                 // Crear un objeto MPU6050
int ldrValue = 0;                            // Variable para almacenar el valor del LDR
bool movimientoDetectado = false;            // Para verificar si hay movimiento

// Función que toma muestras del LDR y ajusta el brillo del LED
void sampleLDR() {
  ldrValue = analogRead(ldrPin);  // Leer el valor del LDR (0-1023)

  // Ajustar el brillo del LED: a menos luz (valor alto de ldr), más brillo en el LED
  analogWrite(ledPin, ldrValue / 4);  // Convertir el rango del LDR (0-1023) a PWM (0-255)
}

// Función que envía las lecturas del LDR a la consola serie cada 1s
void sendSerialData() {
  // Enviar la lectura del LDR a la consola
  Serial.print("LDR Value: ");
  Serial.println(ldrValue);

  // Si el nivel de luz es bajo y hay movimiento, activar alarma
  if (ldrValue < 512 && movimientoDetectado) {
    Serial.println("¡Alarma! Luz baja y movimiento detectado.");
  }
}

// Función que detecta movimiento desde el MPU6050
void detectarMovimiento() {
  // Leer los datos del MPU6050
  mpu.getAcceleration(&ax, &ay, &az);

  // Verificar si hay un cambio significativo en los ejes x o y, o si z supera los 300
  if (abs(ax - axPrev) > 300 || abs(ay - ayPrev) > 300 || abs(az - azPrev) > 300) {
    movimientoDetectado = true;
    Serial.println("Movimiento detectado.");
  } else {
    movimientoDetectado = false;
  }

  // Actualizar las lecturas anteriores solo si se detecta un cambio
  axPrev = ax;
  ayPrev = ay;
  azPrev = az;
}

void setup() {
  pinMode(ledPin, OUTPUT);  // Configurar el LED como salida
  analogWriteRange(255);    // Establecer el rango de PWM (0-255)
  Serial.begin(115200);     // Iniciar la comunicación serie

  // Inicializar el MPU6050
  Wire.begin();
  mpu.initialize();

  if (mpu.testConnection()) {
    Serial.println("MPU6050 conectado correctamente.");
  } else {
    Serial.println("Error al conectar el MPU6050.");
  }

  // Configurar el ticker para tomar muestras del LDR cada 10 ms
  tickerLDR.attach_ms(10, sampleLDR);
  
  // Configurar el ticker para detectar movimiento cada 100 ms
  tickerMovimiento.attach_ms(100, detectarMovimiento);

  // Configurar el ticker para enviar lecturas a la consola cada 1 segundo
  tickerSerial.attach_ms(1000, sendSerialData);
}

void loop() {
  // El bucle principal no hace nada, todo se maneja con los tickers
}
