#include <WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Credenciales WiFi y configuración MQTT
const char* ssid = "Hugo";
const char* password = "Alicia24";
const char* mqtt_server = "172.20.10.2";

WiFiClient espClient;
PubSubClient client(espClient);

// Pines de sensores
#define ORP_PIN 33
#define PH_PIN 32
#define TDS_PIN 35
#define TURBIDITY_PIN 34
#define ONE_WIRE_BUS 4

// Configuración del sensor de temperatura
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensors(&oneWire);

// Variables globales para los sensores
float orpValue, phValue, tdsValue, turbidityValue, temperature;

// Resolución ADC y constantes
#define ADC_RESOLUTION 4095
#define VREF 3.3
#define OFFSET_ORP 150
const float TdsFactor = 0.5;
float voltage_at_ph7 = 2.5;
float voltage_at_ph4 = 3.0;
float ph_step = (voltage_at_ph7 - voltage_at_ph4) / (7.0 - 4.0);

// Estados
enum State { READ_ORP, READ_PH, READ_TDS, READ_TURBIDITY, READ_TEMPERATURE, SEND_DATA };
State currentState = READ_ORP;

void setup_wifi() {
    delay(10);
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("\nWiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP32Client")) {
            Serial.println("connected");
        } else {
            Serial.print("failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    setup_wifi();
    client.setServer(mqtt_server, 1883);
    tempSensors.begin();
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    switch (currentState) {
        case READ_ORP: {
            int adcValue = analogRead(ORP_PIN);
            float voltage = (adcValue / float(ADC_RESOLUTION)) * VREF;
            orpValue = (voltage * 1000) - OFFSET_ORP;
            currentState = READ_PH;
            break;
        }
        case READ_PH: {
            int adcValue = analogRead(PH_PIN);
            float voltage = adcValue * VREF / ADC_RESOLUTION;
            phValue = 7.0 - ((voltage_at_ph7 - voltage) / ph_step);
            currentState = READ_TDS;
            break;
        }
        case READ_TDS: {
            int adcValue = analogRead(TDS_PIN);
            float voltage = (adcValue * VREF) / ADC_RESOLUTION;
            tdsValue = (voltage / TdsFactor) * 1000;
            currentState = READ_TURBIDITY;
            break;
        }
        case READ_TURBIDITY: {
            int adcValue = analogRead(TURBIDITY_PIN);
            float voltage = adcValue * (VREF / ADC_RESOLUTION);
            turbidityValue = voltage;
            currentState = READ_TEMPERATURE;
            break;
        }
        case READ_TEMPERATURE: {
            tempSensors.requestTemperatures();
            temperature = tempSensors.getTempCByIndex(0);
            currentState = SEND_DATA;
            break;
        }
        case SEND_DATA: {
            // Crear el JSON con los datos de los sensores
            String payload = "{";
            payload += "\"orp\": " + String(orpValue, 1) + ", ";
            payload += "\"ph\": " + String(phValue, 2) + ", ";
            payload += "\"tds\": " + String(tdsValue, 1) + ", ";
            payload += "\"turbidity\": " + String(turbidityValue, 2) + ", ";
            payload += "\"temperature\": " + String(temperature, 2);
            payload += "}";

            // Publicar el JSON en el broker MQTT
            client.publish("sensors/data", payload.c_str());
            Serial.println("Data sent: " + payload);

            currentState = READ_ORP;
            break;
        }
    }

    delay(1000); // Intervalo entre lecturas
}
