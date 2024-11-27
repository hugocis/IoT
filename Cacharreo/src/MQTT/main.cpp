#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>

// Configuración de red Wi-Fi
const char* ssid = "RUT_A836_2G";          // Reemplaza con tu SSID
const char* password = "iot_wifi_2g";  // Reemplaza con tu contraseña de Wi-Fi

// Configuración de MQTT
const char* mqtt_server = "10.10.0.161"; // IP de tu broker MQTT (Docker)
const int mqtt_port = 1883;              // Puerto MQTT estándar
const char* mqtt_topic = "test/topic";

// Instancias para WiFi, MQTT y Ticker
WiFiClient espClient;
PubSubClient client(espClient);
Ticker ticker;

// Función para reconectar al broker MQTT si la conexión se pierde
void reconnect() {
  while (!client.connected()) {
    Serial.print("Conectando al broker MQTT...");
    if (client.connect("ESP8266Client")) {
      Serial.println("conectado");
      client.subscribe(mqtt_topic); // Suscribirse al tópico después de conectar
    } else {
      Serial.print("Falló la conexión, rc=");
      Serial.print(client.state());
      Serial.println(" intentando de nuevo en 5 segundos");
      delay(5000); // Intento de reconexión en caso de fallo
    }
  }
}

// Función para manejar los mensajes MQTT entrantes
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Mensaje recibido [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

// Función que se ejecutará cada 10 segundos para publicar un mensaje
void publishMessage() {
  if (!client.connected()) {
    reconnect();
  }
  
  String message = "Holaaaaaaaaaaaaaaaaaa soy Refnando";
  Serial.print("Publicando mensaje: ");
  Serial.println(message);
  client.publish(mqtt_topic, message.c_str());
}

void setup() {
  Serial.begin(115200);
  delay(10);

  // Conexión a Wi-Fi
  Serial.println();
  Serial.print("Conectando a ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Configuración del cliente MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  // Configuración del Ticker para publicar el mensaje cada 10 segundos
  ticker.attach(10, publishMessage); // Llama a publishMessage cada 10 segundos
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
