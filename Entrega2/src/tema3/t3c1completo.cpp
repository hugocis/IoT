#include <SPI.h>
#include <Ticker.h>
#include <Wire.h>
#include <MPU6050.h>
#include <Arduino.h>

Ticker ticker;
MPU6050 mpu; // Crear un objeto MPU6050
byte dataToSend = 0xFF; // Valor por defecto

volatile bool movimientoDetectado = false; // Variable para indicar si hay un movimiento
int16_t axPrev = 0, ayPrev = 0, azPrev = 0; // Variables para almacenar las lecturas anteriores
unsigned long lastMessageTime = 0; // Almacena el tiempo del último mensaje
const unsigned long messageInterval = 2000; // Intervalo para mostrar el mensaje (2 segundos)
int16_t ax, ay, az;


void sendSerialMPUData(); // Declaración de la función
void verificarMovimiento(); // Declaración de la función para verificar movimiento

void setup() {
  Serial.begin(115200);

  Wire.begin();

  // Iniciar el bus I2C para el MPU6050
  mpu.initialize();

  // Verificar si el MPU6050 está conectado correctamente
  if (mpu.testConnection()) {
    Serial.println("MPU6050 conectado correctamente.");
  } else {
    Serial.println("Error al conectar el MPU6050.");
  }

  // Configurar el ticker para enviar datos cada 100 ms
  ticker.attach_ms(100, sendSerialMPUData);


}

void sendSerialMPUData() {
  // Leer los datos del MPU6050
  mpu.getAcceleration(&ax, &ay, &az);

  // Verificar si hay un cambio significativo en los ejes x o y, o si z supera los 200
  if (abs(ax - axPrev) > 300 || abs(ay - ayPrev) > 300 || (abs(az - azPrev) > 300)) {
      movimientoDetectado = true;
    }else {
      movimientoDetectado = false;
  }
  
  // Actualizar las lecturas anteriores
  axPrev = ax;
  ayPrev = ay;
  azPrev = az;
}


void loop() {
  if(movimientoDetectado){
    Serial.println("Movimiento detectado.");
  }
  delay(1000);
}
