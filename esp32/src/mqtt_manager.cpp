#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "secrets.h"

WiFiClient espClient;
PubSubClient mqttClient(espClient);

unsigned long lastMqttAttemptMs = 0;
const unsigned long MQTT_RETRY_MS = 3000;
String deviceId = "esp32-test";

String statusTopic() {
  return "lab/" + String(MQTT_GROUP) + "/" + deviceId + "/status";
}

bool connectMqttIfNeeded() {
  // 1. Sprawdzenie Wi-Fi
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  // 2. Sprawdzenie czy już połączono
  if (mqttClient.connected()) {
    return true;
  }
  // 3. Stoper prób połączenia
  if (millis() - lastMqttAttemptMs < MQTT_RETRY_MS) {
    return false;
  }
  lastMqttAttemptMs = millis();

  // 4. Konfiguracja Last Will
  String willPayload = "{\"device_id\":\"" + deviceId + "\",\"status\":\"offline\"}";
  
  // 5. Próba połączenia
  bool ok = mqttClient.connect(
    deviceId.c_str(),
    statusTopic().c_str(),
    0,
    true,
    willPayload.c_str()
  );

  // 6. Raportowanie stanu
  if (ok) {
    Serial.println("MQTT connected");
    // DODATEK Z INSTRUKCJI: Status online po udanym połączeniu
    String onlinePayload = "{\"device_id\":\"" + deviceId + "\",\"status\":\"online\"}";
    mqttClient.publish(statusTopic().c_str(), onlinePayload.c_str(), true);
  } else {
    Serial.print("MQTT connect failed, rc=");
    Serial.println(mqttClient.state());
  }
  
  return ok;
}