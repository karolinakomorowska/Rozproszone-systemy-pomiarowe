#include "wifi_manager.h"
#include "secrets.h"

void connectWiFi()
{
  Serial.print("Laczenie z Wi-Fi: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.println("Polaczono z Wi-Fi");
  Serial.print("Adres IP: ");
  Serial.println(WiFi.localIP());
}


bool isWiFiConnected()
{
  return WiFi.status() == WL_CONNECTED;
}