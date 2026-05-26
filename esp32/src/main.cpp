#include <Arduino.h>
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "time_sync.h"
#include "secrets.h"
#include "device.h" // Dodane dla generateDeviceIdFromNvs()

unsigned long lastMeasurementMs = 0;
const unsigned long MEASUREMENT_INTERVAL_MS = 5000;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  // Pobranie unikalnego ID urządzenia i przypisanie do zmiennej globalnej
  deviceId = generateDeviceIdFromNvs();
  Serial.print("Device ID: ");
  Serial.println(deviceId);

  // KONFIGURACJA BROKERA MQTT (Czysta biblioteka PubSubClient)
  // UWAGA: Sprawdź w swoim pliku secrets.h, jak nazywa się zmienna z adresem IP!
  // Jeśli to nie MQTT_SERVER, podmień tę nazwę poniżej:
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);

  connectWiFi();
  synchronizeTime(); // (Jeśli znowu zablokuje program, dodaj tu //)
}

void loop()
{
  // 1. Zabezpieczenie połączeń (nieblokujące)
  connectWiFiIfNeeded();
  connectMqttIfNeeded(); // <-- Tu wykonuje się nasz nowy kod z instrukcji (z Last Will!)
  mqttClient.loop();

  // 2. Cykliczne wysyłanie pomiarów
  if (millis() - lastMeasurementMs >= MEASUREMENT_INTERVAL_MS)
  {
    lastMeasurementMs = millis();

    if (isWiFiConnected() && mqttClient.connected())
    {
      long long time = 0;
      if (isTimeSynchronized())
      {
        time = getTimestampMs();
      }

      float tempC = temperatureRead();
      
      // Ręczne budowanie topicu i JSONa z pomiarem
      String topic = "lab/" + String(MQTT_GROUP) + "/" + deviceId + "/temperature";
      String payload = "{\"device_id\":\"" + deviceId + "\",\"value\":" + String(tempC) + ",\"ts_ms\":" + String(time) + "}";
      
      // Wysyłamy przez standardową funkcję biblioteki
      mqttClient.publish(topic.c_str(), payload.c_str());
      
      Serial.print("Wyslano pomiar: ");
      Serial.println(tempC);
    }
  }
}