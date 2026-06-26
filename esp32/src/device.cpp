#include "device.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_random.h"

String generateDeviceIdFromEfuse()
{
  uint64_t chipId = ESP.getEfuseMac();
  char id[32];
  snprintf(id, sizeof(id), "esp32-%04X%08X",
           (uint16_t)(chipId >> 32),
           (uint32_t)chipId);
  return String(id);
}

String generateDeviceIdFromNvs()
{
  nvs_handle_t nvs;
  esp_err_t err;

  err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND)
  {
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);
  ESP_ERROR_CHECK(nvs_open("storage", NVS_READWRITE, &nvs));

  uint64_t high, low;
  esp_err_t err_high = nvs_get_u64(nvs, "device_id_high", &high);
  esp_err_t err_low  = nvs_get_u64(nvs, "device_id_low",  &low);

  if (err_high == ESP_ERR_NVS_NOT_FOUND || err_low == ESP_ERR_NVS_NOT_FOUND)
  {
    Serial.println("ID nie znaleziono, generuję nowe...");

    high = ((uint64_t)esp_random() << 32) | esp_random();
    low  = ((uint64_t)esp_random() << 32) | esp_random();

    ESP_ERROR_CHECK(nvs_set_u64(nvs, "device_id_high", high));
    ESP_ERROR_CHECK(nvs_set_u64(nvs, "device_id_low",  low));
    ESP_ERROR_CHECK(nvs_commit(nvs));

    Serial.println("Nowe ID zapisane.");
  }
  else
  {
    ESP_ERROR_CHECK(err_high);
    ESP_ERROR_CHECK(err_low);
    Serial.println("Odczytano ID.");
  }

  nvs_close(nvs);

  char id_str[40];
  snprintf(id_str, sizeof(id_str), "esp32-%016llX%016llX",
           (unsigned long long)high,
           (unsigned long long)low);
  return String(id_str);
}