#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>


String generateDeviceIdFromEfuse();
String generateDeviceIdFromNvs();
void connectWiFi();
void connectWiFiIfNeeded(); // <-- DODAJ TĘ LINIJKĘ
bool isWiFiConnected();
#endif // DEVICE_H