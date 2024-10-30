#include <Arduino.h>
#include <ESP8266WiFiMulti.h>
#include <WiFi.h>

// WiFi connect timeout per AP. Increase when connecting takes longer.
const uint32_t connectTimeoutMs = 5000;
WiFiClient  client; 

void setup()
{
  Serial.begin(115200);
  Serial.println();

  WiFi.begin("RUT_A836_2G", "iot_wifi_2g");

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.print("Connected, IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()
{

}
