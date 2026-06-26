#ifndef MQTT_MANAGER_H
#define MQTT_MANAGER_H

#include <PubSubClient.h>

extern PubSubClient mqttClient;
extern String deviceId;
bool connectMqttIfNeeded();

#endif