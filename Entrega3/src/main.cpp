/*
 * ======================================================================================
 * Proyecto:      Conjunto 3 - Comunicación por Sockets 
 * Autores:       Hugo Cisneros, Alicia García, Alfonso Moreno y Fernando Teba
 * Asignatura:    Internet of things
 * ======================================================================================
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>

// Configuración WiFi
const char* ssid = "RUT_A836_2G";
const char* password = "iot_wifi_2g"; 

// Configuración del servidor TCP
const char* server = "10.10.0.161";
const uint16_t port = 17010;

WiFiClient client;

// Configuración del botón
const int buttonPin = D3; 
bool buttonPressed = false; 

// Estados del programa
enum State {
  CONNECT_WIFI,
  CONNECT_SERVER,
  SEND_MESSAGE,
  WAIT_RESPONSE,
  IDLE
};
State currentState = CONNECT_WIFI;

const char* getStateName(State state) {
  switch (state) {
    case CONNECT_WIFI: return "CONECTANDO_A_LA_RED_WIFI";
    case CONNECT_SERVER: return "CONECTANDO_AL_SERVIDOR_TCP";
    case SEND_MESSAGE: return "ENVIAR_MENSAJE";
    case WAIT_RESPONSE: return "ESPERANDO_RESPUESTA";
    case IDLE: return "INACTIVO/ESPERANDO";
    default: return "ESTADO_DESCONOCIDO";
  }
}

Ticker stateTicker; // Ticker para gestionar la periodicidad de estados
bool tickFlag = false; // Bandera activada por el Ticker
Ticker debounceTicker;     // Ticker para manejar el debounce
bool debounceLock = false; // Bandera para bloquear el botón durante el debounce

// Mensaje a enviar
String message = "";

// Función que activa la bandera para el siguiente tick
void handleTicker() {
  tickFlag = true;
}

// Función para desbloquear el botón después del tiempo de debounce
void unlockButton() {
  debounceLock = false; // Desbloquea el botón
}

// Interrupción para detectar pulsación del botón
void IRAM_ATTR handleButtonPress() {
  
  if (!debounceLock) { // Verifica si el botón no está bloqueado por debounce
    buttonPressed = true; // Marca el botón como presionado
    debounceLock = true;  // Bloquea el botón
    debounceTicker.once_ms(200, unlockButton); // Programa el desbloqueo en 200 ms
  }
}

// Función para cambiar de estado
void setState(State newState) {
  currentState = newState;
  Serial.print("Cambiando de estado: ");
  Serial.println(getStateName(currentState));
}

// Función de envío del mensaje al servidor
void sendMessage() {
  // Crear el mensaje
  message = "Hi there!, sending message from " + WiFi.localIP().toString() + ". I yield to no one, so I get rebooted by the watchdo.";

  uint16_t opcode = htons(0x0001);                 // OPCODE en formato de red
  uint16_t payloadLen = htons(message.length());  // Longitud del payload en formato de red

  // Crear un buffer para el encabezado (4 bytes)
  uint8_t header[4];
  memcpy(header, &opcode, 2);                     // Copiar OPCODE al buffer
  memcpy(header + 2, &payloadLen, 2);             // Copiar PayloadLen al buffer

  // Enviar encabezado completo (4 bytes)
  client.write(header, sizeof(header));

  // Enviar el Payload (mensaje)
  client.write(message.c_str(), message.length());

  Serial.println("¡Mensaje enviado!");
  Serial.print("Contenido del mensaje: ");
  Serial.println(message);
  Serial.print("Longitud del payload: ");
  Serial.println(message.length());
}

// Función para recibir la respuesta del servidor
void receiveMessage() {
  if (client.available() >= 4) { // Verifica que al menos lleguen OPCODE (2 bytes) y PayloadLen (2 bytes)
    uint16_t opcode;
    client.read((uint8_t*)&opcode, sizeof(opcode));
    opcode = ntohs(opcode);

    uint16_t payloadLen;
    client.read((uint8_t*)&payloadLen, sizeof(payloadLen));
    payloadLen = ntohs(payloadLen);

    char payload[payloadLen + 1]; 
    client.readBytes(payload, payloadLen);
    payload[payloadLen] = '\0'; // Añadir terminador nulo

    // Mostrar respuesta
    Serial.print("OPCODE recibido: ");
    Serial.println(opcode, HEX);
    Serial.print("Payload recibido: ");
    Serial.println(payload);

    setState(IDLE); 
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\nConectando a la wifi...");

  WiFi.begin(ssid, password);

  // Se configura el botón
  pinMode(buttonPin, INPUT_PULLUP); // Botón con resistencia pull-up
  attachInterrupt(digitalPinToInterrupt(buttonPin), handleButtonPress, FALLING);

  // Se configura el Ticker para verificar cada 100 ms
  stateTicker.attach(0.1, handleTicker);
}

void loop() {
  if (tickFlag) { // Verifica si el ticker activó la bandera
    tickFlag = false; // Reinicia la bandera

    switch (currentState) {
      case CONNECT_WIFI:
        if (WiFi.status() == WL_CONNECTED) {
          Serial.println("WiFi conectada.");
          Serial.print("Dirección IP: ");
          Serial.println(WiFi.localIP());
          setState(CONNECT_SERVER);
        }
        break;

      case CONNECT_SERVER:
        if (client.connect(server, port)) {
          Serial.println("Conectado al servidor TCP!");
          setState(SEND_MESSAGE);
        } else {
          Serial.println("Fallo al conectar al servidor. Volviendo a intentar...");
        }
        break;

      case SEND_MESSAGE:
        sendMessage();
        setState(WAIT_RESPONSE);
        break;

      case WAIT_RESPONSE:
        receiveMessage();
        break;

      case IDLE:
        if (buttonPressed) { 
          buttonPressed = false; // Reinicia la bandera del botón
          Serial.println("Botón presionado! Volviendo a mandar el mensaje...");
          setState(CONNECT_SERVER); // Volver a conectar y enviar mensaje
        }
        break;
    }
  }
}
