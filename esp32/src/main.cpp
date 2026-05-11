#include <Arduino.h>
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "time_sync.h"
#include "secrets.h"
#include "device.h"

WiFiClient espClient;
mqtt_manager mqttClient(espClient);

String deviceId;
String topic;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  deviceId = generateDeviceIdFromNvs();
  topic = "lab/" + String(MQTT_GROUP) + "/" + deviceId;

  Serial.print("Device ID: ");
  Serial.println(deviceId);

  mqttClient.begin(deviceId, topic);

  connectWiFi();
  mqttClient.connectMQTT();
  synchronizeTime();
  mqttClient.publishStatus("online", getTimestampMs());
}

void loop()
{
  if (!isWiFiConnected())
  {
    connectWiFi();
  }

  if (!mqttClient.isConnected())
  {
    mqttClient.connectMQTT();
  }

  mqttClient.loop();

  long long time = 0;
  if (isTimeSynchronized())
  {
    time = getTimestampMs();
  }

  float tempC = temperatureRead();

  mqttClient.publishMeasurement("temperature", tempC, "C", time);
  
  delay(5000);
}