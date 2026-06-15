#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

// Używamy zwykłego klienta WiFi do połączenia lokalnego (bez TLS)
WiFiClient espClient; 
PubSubClient mqttClient(espClient);

unsigned long lastMqttAttemptMs = 0;
const unsigned long MQTT_RETRY_MS = 3000;
String deviceId = "esp32-test";

String statusTopic() {
  return "lab/" + String(MQTT_GROUP) + "/" + deviceId + "/status";
}

bool connectMqttIfNeeded() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  if (mqttClient.connected()) {
    return true;
  }
  if (millis() - lastMqttAttemptMs < MQTT_RETRY_MS) {
    return false;
  }
  lastMqttAttemptMs = millis();

  String willPayload = "{\"device_id\":\"" + deviceId + "\",\"status\":\"offline\"}";
  
  bool ok = mqttClient.connect(
    deviceId.c_str(),
    statusTopic().c_str(),
    0,
    true,
    willPayload.c_str()
  );

  if (ok) {
    Serial.println("MQTT connected"); // Poprawiony komunikat
    String onlinePayload = "{\"device_id\":\"" + deviceId + "\",\"status\":\"online\"}";
    mqttClient.publish(statusTopic().c_str(), onlinePayload.c_str(), true);
  } else {
    Serial.print("MQTT connect failed, rc=");
    Serial.println(mqttClient.state());
  }
  
  return ok;
}