#include "mqtt_manager.h"
#include "secrets.h"

#define SCHEMA_VERSION 2

mqtt_manager::mqtt_manager(WiFiClient& espClient) : mqttClient(espClient) {}

void mqtt_manager::begin(const String& deviceID, const String& deviceTopic)
{
  mainTopic = deviceTopic;
  deviceId = deviceID;
  seq_data_counter = 0;
  seq_status_counter = 0;
}

void mqtt_manager::connectMQTT()
{
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  while (!mqttClient.connected())
  {
    Serial.print("Laczenie z MQTT...");
    if (mqttClient.connect(deviceId.c_str()))
    {
      Serial.println("OK");
    }
    else
    {
      Serial.print("blad, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" - ponowna proba za 2 s");
      delay(2000);
    }
  }
}

void mqtt_manager::publishMeasurement(const String &sensor, float value, const String &unit, long long ts_ms)
{
  JsonDocument doc;
  doc["schema_version"] = SCHEMA_VERSION;
  doc["group_id"] = MQTT_GROUP;
  doc["device_id"] = deviceId;
  doc["sensor"] = sensor;
  doc["value"] = value;
  doc["unit"] = unit;
  doc["ts_ms"] = ts_ms;
  doc["seq"] = seq_data_counter++;
  doc["type"] = "meas";

  char payload[256];
  serializeJson(doc, payload);

  String topic;
  topic = mainTopic + "/" + sensor;

  mqttClient.publish(topic.c_str(), payload);
  Serial.print("Publikacja na topic: ");
  Serial.println(topic);
  Serial.println(payload);
}

void mqtt_manager::publishStatus(const String &status, long long ts_ms)
{
  JsonDocument doc;
  doc["schema_version"] = SCHEMA_VERSION;
  doc["group_id"] = MQTT_GROUP;
  doc["device_id"] = deviceId;
  doc["status"] = status;
  doc["ts_ms"] = ts_ms;
  doc["seq"] = seq_status_counter++;
  doc["type"] = "status";

  char payload[256];
  serializeJson(doc, payload);

  String topic;
  topic = mainTopic + "/" + "status";

  mqttClient.publish(topic.c_str(), payload);
  Serial.print("Publikacja na topic: ");
  Serial.println(topic);
  Serial.println(payload);
}