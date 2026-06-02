#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h> // <-- DODANE: Biblioteka do szyfrowania
#include <PubSubClient.h>
#include "secrets.h"

// <-- DODANE: Tutaj wklejasz certyfikat CA
const char* ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIEATCCAumgAwIBAgIUJbhV7ic3pNMXvZpVTEvhK+JXFScwDQYJKoZIhvcNAQEL
BQAwgY8xCzAJBgNVBAYTAlBMMRQwEgYDVQQIDAtEb2xueS1TbGFzazEQMA4GA1UE
BwwHV3JvY2xhdzEOMAwGA1UECgwFS01FaUYxDjAMBgNVBAsMBUtNRWlGMRYwFAYD
VQQDDA0xNTYuMTcuNDUuMTUwMSAwHgYJKoZIhvcNAQkBFhFsYWIxMDdAcHdyLmVk
dS5wbDAeFw0yNjA2MDIwOTI5MjZaFw0zNjA1MzAwOTI5MjZaMIGPMQswCQYDVQQG
EwJQTDEUMBIGA1UECAwLRG9sbnktU2xhc2sxEDAOBgNVBAcMB1dyb2NsYXcxDjAM
BgNVBAoMBUtNRWlGMQ4wDAYDVQQLDAVLTUVpRjEWMBQGA1UEAwwNMTU2LjE3LjQ1
LjE1MDEgMB4GCSqGSIb3DQEJARYRbGFiMTA3QHB3ci5lZHUucGwwggEiMA0GCSqG
SIb3DQEBAQUAA4IBDwAwggEKAoIBAQCzJvm0zI6B/i0DpES7duIGgSBpC5Axmy/4
ycTRxA/UkFNBIHNHnytzE/opBk4to1XwhS+Q2cFRINjtiU49bw/aFyoKNp3JMaVx
ie2m1+K7Ou5iz+hErk0YYPCzPgywxytHzRW/bWZza8ts/soQXnTHDbnC1XLzahZK
IHkc2rzwayfXTf0e20WwAcXBy2dHDlkyvmJPdkwSasAm2+3PoWagqXVXSPQHLVPv
6IXmdjA1ObEvMYWn+uJgILOZTWlKL/BQUKsWwaRjmvkJpSsAc2n8xLTOPkMWGW5E
fSMxJzYElGCN6vM20Jp3vHsAopkRdMd0DD9MTTe0n4bUMtGfbinDAgMBAAGjUzBR
MB0GA1UdDgQWBBSyMOGYj+2Ploat5HN2d6tF2ybyUzAfBgNVHSMEGDAWgBSyMOGY
j+2Ploat5HN2d6tF2ybyUzAPBgNVHRMBAf8EBTADAQH/MA0GCSqGSIb3DQEBCwUA
A4IBAQA8eOOZlghQnh+v8ifr9ro/qSDVslBfpuigNxUIfHeTidgAxm1qFxSOGyzp
KMlYvuoesNX7+muspD+6zzdzJTlySQxEf2729yv/4bSsoZhyx4tGo3gs318csn4j
EbXNHQfz56dGxZFa+NavcqwWvY27DtJXhr+XWH250xqg+zZOynq4DlErOJsLbRE+
CucY/dbWdFI9SiAgN9Dsih8HosWZfsiFAdefKHZDLGkCZH/I6kvnHMiU3mvaLTCx
8bZVDaZCiK9xfTc4OK4Jt0APYuT2wKYSWlveaP+h879mkjSszFbdPkhhJ/aqi9hI
FqLsyidSaW13XUnqizHbtSbG29/G
-----END CERTIFICATE-----
)EOF";

WiFiClientSecure espClient; // <-- ZMIENIONE: Teraz używamy bezpiecznego klienta WiFi
PubSubClient mqttClient(espClient);

unsigned long lastMqttAttemptMs = 0;
const unsigned long MQTT_RETRY_MS = 3000;
String deviceId = "esp32-test";

String statusTopic() {
  return "lab/" + String(MQTT_GROUP) + "/" + deviceId + "/status";
}

bool connectMqttIfNeeded() {
  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }
  if (mqttClient.connected()) {
    return true;
  }
  if (millis() - lastMqttAttemptMs < MQTT_RETRY_MS) {
    return false;
  }
  lastMqttAttemptMs = millis();

  // <-- DODANE: Mówimy klientowi WiFi, jakiego certyfikatu ma używać
  espClient.setCACert(ca_cert); 

  String willPayload = "{\"device_id\":\"" + deviceId + "\",\"status\":\"offline\"}";
  
  bool ok = mqttClient.connect(
    deviceId.c_str(),
    statusTopic().c_str(),
    0,
    true,
    willPayload.c_str()
  );

  if (ok) {
    Serial.println("MQTT connected (TLS)"); // Zmieniony napis, żeby było widać w konsoli!
    String onlinePayload = "{\"device_id\":\"" + deviceId + "\",\"status\":\"online\"}";
    mqttClient.publish(statusTopic().c_str(), onlinePayload.c_str(), true);
  } else {
    Serial.print("MQTT connect failed, rc=");
    Serial.println(mqttClient.state());
  }
  
  return ok;
}