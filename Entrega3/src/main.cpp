#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>

// Timeout para la conexion WiFi.
const uint32_t connectTimeoutMs = 5000;
WiFiClient client; 

const char* serverIP = "10.10.0.164";
const uint16_t serverPort = 17010; 

const short OPCODE = 0x0001;

const char* NombreRedWiFi = "RUT_A836_2G";
const char* ContraRedWiFi = "iot_wifi_2g";

void setup()
{
  Serial.begin(115200);
  Serial.println();

  // Credenciales de acceso a la red WiFi.
  WiFi.begin(NombreRedWiFi, ContraRedWiFi);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) // Esperar conexion
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: "); // Mensaje de verificacion de conexion
  Serial.println(WiFi.localIP());

  if (client.connect(serverIP, serverPort)) {
    Serial.println("Conectado al servidor.");
    
    // Mensaje con datos dinámicos
    String equipoNombre = "LOOPERS";
    String ipPort = WiFi.localIP().toString() + ":" + String(serverPort);
    String saludo = "¡Oh poderoso profesor!, " + equipoNombre + " desde " + ipPort + " te saluda";

    short payloadLen = saludo.length();
    
    // Buffer para el encabezado y el mensaje
    uint8_t buffer[4 + payloadLen];

    // Convertir OPCODE y PayloadLen a big-endian antes de copiarlos al buffer
    buffer[0] = (OPCODE >> 8) & 0xFF;    // byte alto de OPCODE
    buffer[1] = OPCODE & 0xFF;           // byte bajo de OPCODE
    buffer[2] = (payloadLen >> 8) & 0xFF; // byte alto de PayloadLen
    buffer[3] = payloadLen & 0xFF;       // byte bajo de PayloadLen

    // Coloca el Payload en los bytes restantes
    memcpy(buffer + 4, saludo.c_str(), payloadLen);

    // Envía el buffer completo en un solo comando write para evitar fragmentación
    client.write(buffer, sizeof(buffer));

    Serial.println("Mensaje enviado al servidor.");
  
  } else {
    Serial.println("Error al conectar al servidor.");
  }

}

void loop()
{
      // Recepción de respuesta del servidor (si la hay)
  if (client.available()) {
      // Buffer para leer el encabezado de la respuesta
      uint8_t header[4];
      client.read(header, 4);  // Lee los primeros 4 bytes para OPCODE y PayloadLen

      // Reconstruir OPCODE y PayloadLen en big-endian
      uint16_t responseOpcode = (header[0] << 8) | header[1];
      uint16_t responsePayloadLen = (header[2] << 8) | header[3];

      // Verificar si el OPCODE de respuesta es 0x0002
      if (responseOpcode == 0x0002) {
        Serial.println("Respuesta recibida con OPCODE 0x0002.");

        // Leer el mensaje (Payload) de la longitud especificada
        char payload[responsePayloadLen + 1];
        client.readBytes(payload, responsePayloadLen);
        payload[responsePayloadLen] = '\0';  // Añadir el terminador nulo para imprimir como cadena

        Serial.print("Payload recibido: ");
        Serial.println(payload);
      } else {
        Serial.println("Error: OPCODE de respuesta no coincide.");
      }
  }
}
