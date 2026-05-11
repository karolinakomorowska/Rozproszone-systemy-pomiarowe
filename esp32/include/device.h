#ifndef DEVICE_H
#define DEVICE_H

#include <Arduino.h>

String generateDeviceIdFromEfuse();
String generateDeviceIdFromNvs();

#endif // DEVICE_H