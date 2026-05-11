#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

class mqtt_manager
{
  PubSubClient mqttClient;
  String deviceId;
  String mainTopic;
  uint seq_data_counter;
  uint seq_status_counter;

public:
  mqtt_manager(WiFiClient &espClient);
  void begin(const String &deviceID, const String &deviceTopic);
  void connectMQTT();
  void publishMeasurement(const String &sensor, float value, const String &unit, long long ts_ms);
  void publishStatus(const String &status, long long ts_ms);
  bool isConnected() { return mqttClient.connected(); }
  bool loop() { return mqttClient.loop(); }
};

#endif // MQTT_MANAGER_H