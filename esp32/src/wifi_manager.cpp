#include "wifi_manager.h"
#include "secrets.h"

// --- Zmienne do stoperów ---
unsigned long lastWifiAttemptMs = 0;
const unsigned long WIFI_RETRY_MS = 5000;

// --- 1. FUNKCJA STARTOWA (Blokująca, używana w setup) ---
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

// --- 2. FUNKCJA DO TŁA (Nieblokująca, używana w loop) ---
void connectWiFiIfNeeded() 
{
  if (WiFi.status() == WL_CONNECTED) {
    return;
  }

  if (millis() - lastWifiAttemptMs < WIFI_RETRY_MS) {
    return; 
  }

  lastWifiAttemptMs = millis();
  Serial.println("WiFi disconnected. Trying reconnect...");

  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

// --- 3. FUNKCJA SPRAWDZAJĄCA ---
bool isWiFiConnected()
{
  return WiFi.status() == WL_CONNECTED;
}