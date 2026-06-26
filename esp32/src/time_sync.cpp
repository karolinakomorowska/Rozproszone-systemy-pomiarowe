#include "time_sync.h"
#include <time.h>
#include <sys/time.h>
#include "wifi_manager.h"

bool synchronizeTime()
{
  if (!isWiFiConnected())
  {
    Serial.println("Brak polaczenia sieciowego podczas synchronizacji");
    return false;
  }

  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  struct tm timeinfo;
  while (!getLocalTime(&timeinfo))
  {
    Serial.println("Oczekiwanie na synchronizacje czasu...");
    delay(500);
  }

  Serial.println("Czas zsynchronizowany.");
  return true;
}

long long getTimestampMs()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return ((long long)tv.tv_sec * 1000LL) + (tv.tv_usec / 1000);
}

bool isTimeSynchronized()
{
  struct tm timeinfo;
  return getLocalTime(&timeinfo);
}