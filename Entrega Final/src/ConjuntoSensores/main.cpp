#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h> // Biblioteca para el protocolo OneWire
#include <DallasTemperature.h> // Biblioteca para sensores de temperatura Dallas
#include <Ticker.h>

// Credenciales de red Wi-Fi
const char* ssid = "Hugo";
const char* password = "Alicia24";
// Dirección del servidor MQTT
const char* mqtt_server = "172.20.10.2";

// Los clientes Wi-Fi y MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Definimos los pines para cada sensor
#define ORP_PIN 33
#define PH_PIN 32
#define TDS_PIN 35
#define TURBIDITY_PIN 34
#define ONE_WIRE_BUS 4

// Configuramos el sensor de temperatura
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensors(&oneWire);

// Variables para almacenar los datos de los sensores
float orpValue, phValue, tdsValue, turbidityValue, temperature;

// Estas son unas constantes necesarias para calcular los valores del agua
#define ADC_RESOLUTION 4095 // Resolución ADC del ESP32
#define VREF 3.3 // Voltaje de referencia
#define OFFSET_ORP 150 // Desplazamiento para el cálculo de ORP
const float TdsFactor = 0.5; // Factor de conversión para TDS

// Calibramos el sensor de pH
float voltage_at_ph7 = 2.5;
float voltage_at_ph4 = 3.0;
float ph_step = (voltage_at_ph7 - voltage_at_ph4) / (7.0 - 4.0);

// Definimos los estados del código
enum State { READ_ORP, READ_PH, READ_TDS, READ_TURBIDITY, READ_TEMPERATURE, SEND_DATA };
State currentState = READ_ORP; // Iniciamos con el primer estado del programa

Ticker ticker;

// Esta función se encarga del salto de estados y de la lectura de los sensores, asi como de el envio de datos al servidor MQTT 
void handleState() {
    switch (currentState) {
        case READ_ORP: { // Lectura de ORP
            int adcValue = analogRead(ORP_PIN);
            float voltage = (adcValue / float(ADC_RESOLUTION)) * VREF;
            orpValue = (voltage * 1000) - OFFSET_ORP;
            currentState = READ_PH;
            break;
        }
        case READ_PH: { // Lectura de pH
            int adcValue = analogRead(PH_PIN);
            float voltage = adcValue * VREF / ADC_RESOLUTION;
            phValue = 7.0 - ((voltage_at_ph7 - voltage) / ph_step);
            currentState = READ_TDS;
            break;
        }
        case READ_TDS: { // Lectura de TDS
            int adcValue = analogRead(TDS_PIN);
            float voltage = (adcValue * VREF) / ADC_RESOLUTION;
            tdsValue = (voltage / TdsFactor) * 1000;
            currentState = READ_TURBIDITY;
            break;
        }
        case READ_TURBIDITY: { // Lectura de turbidez
            int adcValue = analogRead(TURBIDITY_PIN);
            float voltage = adcValue * (VREF / ADC_RESOLUTION);
            turbidityValue = voltage;
            currentState = READ_TEMPERATURE;
            break;
        }
        case READ_TEMPERATURE: { // Lectura de Temperatura
            tempSensors.requestTemperatures();
            temperature = tempSensors.getTempCByIndex(0) - 0.4; // Ajuste por calibración
            currentState = SEND_DATA;
            break;
        }
        case SEND_DATA: { // Envío de datos al servidor MQTT
            String payload = "{";
            payload += "\"orp\": " + String(orpValue, 1) + ", ";
            payload += "\"ph\": " + String(phValue, 2) + ", ";
            payload += "\"tds\": " + String(tdsValue, 1) + ", ";
            payload += "\"turbidity\": " + String(turbidityValue, 2) + ", ";
            payload += "\"temperature\": " + String(temperature, 2);
            payload += "}";

            client.publish("sensors/data", payload.c_str()); // Publicamos el resultado en el servidor MQTT
            Serial.println("Data sent: " + payload); // Mostramos el resultado en el terminal

            currentState = READ_ORP; // Reiniciamos el ciclo de lectura
            break;
        }
    }
}

// Función para iniciar la conexión wifi
void setup_wifi() {
    Serial.print("Intentando conectar a ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi conectado con éxito.");
    Serial.println("Dirección IP: ");
    Serial.println(WiFi.localIP());
}

// Esta función se usa al perder conexión con el servidor para reconectar
void reconnect() {
    while (!client.connected()) {
        Serial.print("Intentando conectar a MQTT...");
        if (client.connect("ESP32Client")) {
            Serial.println("Conexión establecida");
        } else {
            Serial.print("Falló, código de error=");
            Serial.print(client.state());
            Serial.println(". Intentando de nuevo en 5 segundos");
        }
    }
}

void setup() {
    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqtt_server, 1883); // Configuramos el servidor MQTT
    tempSensors.begin(); // Iniciamos el sensor de temperatura

    ticker.attach(1.0, handleState); // Lectura periódica de los sensores
}

void loop() {
    if (!client.connected()) {
        reconnect(); // Si la conexión MQTT falla se reconecta automáticamente
    }
    client.loop(); // Mantenemos la conexión MQTT activa
}
