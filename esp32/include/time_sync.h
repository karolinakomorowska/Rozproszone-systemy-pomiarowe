#ifndef TIME_SYNC_H
#define TIME_SYNC_H

#include <Arduino.h>

bool synchronizeTime();
long long getTimestampMs();
bool isTimeSynchronized();

#endif // TIME_SYNC_H