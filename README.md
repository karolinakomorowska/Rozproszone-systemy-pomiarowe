# Rozproszone systemy pomiarowe

Repozytorium startowe do projektu z systemów rozproszonych.  
Projekt dotyczy budowy rozproszonego systemu pomiarowego, w którym urządzenia ESP32 zbierają dane z czujników, publikują je do brokera MQTT, a następnie dane są odbierane przez serwisy backendowe, zapisywane do bazy danych i udostępniane przez REST API.

Aktualnie projekt zawiera przygotowane serwisy backendowe uruchamiane przez Docker Compose oraz katalogi na kolejne elementy systemu, takie jak:
- `esp32`
- `ingestor`
- `ui`
- `docs`

---

## Quick Start

Poniższa instrukcja pozwala uruchomić podstawową wersję środowiska projektowego dla systemu rozproszonego.

### Wymagania

Przed uruchomieniem upewnij się, że masz zainstalowane:
- Docker
- Docker Compose

### Klonowanie repozytorium

```bash
git clone https://github.com/mateuszbartczak-pwr/Rozproszone-systemy-pomiarowe.git
cd Rozproszone-systemy-pomiarowe
```

### Uruchomienie środowiska
Aby zbudować i uruchomić wszystkie dostępne serwisy:
```bash
docker compose up --build
```
lub aby uruchomić środowisko w tle:
```bash
docker compose up -d --build
```
### Zatrzymanie środowiska
```bash
docker compose down
```

### Aktualnie dostępne serwisy

Po uruchomieniu Docker Compose powinny być dostępne następujące usługi:

- REST API (Flask) — http://localhost:5001

- Broker MQTT — localhost:1883

- PostgreSQL — localhost:5432

### Podgląd logów

Aby sprawdzić logi wszystkich serwisów:
```bash
docker compose logs
```

Aby śledzić logi na żywo:
```bash
docker compose logs -f
```

Aby wyświetlić logi tylko jednego serwisu:
```bash
docker compose logs -f flask
docker compose logs -f broker
docker compose logs -f database
```
Sprawdzenie statusu kontenerów
```bash
docker compose ps
```

### Struktura projektu

Repozytorium zawiera między innymi następujące katalogi:

- `api/` — backend REST API

- `broker/` — broker MQTT

- `database/` — baza danych PostgreSQL

- `esp32/` — kod dla urządzeń ESP32

- `ingestor/` — serwis odbierający dane z MQTT i zapisujący je do bazy

- `ui/` — warstwa interfejsu użytkownika

- `docs/` — dokumentacja projektu

- `utils/` — narzędzia pomocnicze

### Uwagi

Projekt będzie rozwijany etapami w trakcie semestru.
W kolejnych zajęciach repozytorium będzie rozszerzane o dodatkowe serwisy, integracje i mechanizmy bezpieczeństwa.

## Lab03
Utworzenie kodu esp32 publikującego dane json'owe do brokera

### Wykonane zadania

### 1. Konfiguracja środowiska (PlatformIO)
W pliku `platformio.ini` zdefiniowano środowisko dla płytki `esp32dev` oraz dodano niezbędne biblioteki do obsługi MQTT i formatu JSON:
* `knolleary/PubSubClient` (do komunikacji MQTT)
* `bblanchon/ArduinoJson` (do budowania paczek z danymi)

### 2. Konfiguracja poświadczeń (`secrets.h`)
Utworzono plik `include/secrets.h`, który zawiera:
* SSID i hasło do sieci Wi-Fi.
* Adres brokera MQTT 
* Port (1883) oraz identyfikator grupy roboczej.

### 3. Główny kod programu (`main.cpp`)
Napisano program realizujący następujące funkcje:
* **Generowanie unikalnego ID:** Wykorzystano funkcję `ESP.getEfuseMac()`, aby wygenerować unikalny `device_id` na podstawie sprzętowego adresu MAC płytki.
* **Połączenie z Wi-Fi:** Funkcja `connectWiFi()` łącząca ESP32 z lokalną siecią.
* **Połączenie z MQTT:** Funkcja `connectMQTT()` nawiązująca sesję z brokerem.
* **Publikowanie danych (JSON):** Użyto biblioteki `ArduinoJson` (klasa `JsonDocument`) do zbudowania ramki danych zawierającej m.in. ID urządzenia, typ czujnika ("temperature"), wartość (24.5 °C) i znacznik czasu (`millis()`). Następnie zmodyfikowane kod, aby zczytywał realną wartość temperatury rdzenia mikrokontrolera.
```bash
void publishMeasurement() {
StaticJsonDocument<256> doc;
doc["device_id"] = deviceId;
doc["sensor"] = "temperature";
doc["value"] = temperatureRead();
doc["unit"] = "C";
doc["ts_ms"] = millis()
```
* Dane są wysyłane co 5 sekund na dedykowany topic: `lab/<grupa>/<device_id>/temperature`.


### 4. Weryfikacja działania (MQTT Explorer)
Działanie kodu zweryfikowano za pomocą programu **MQTT Explorer**. Po podłączeniu się do odpowiedniego brokera i zasubskrybowaniu tematu grupy, potwierdzono poprawne, cykliczne odbieranie wiadomości JSON ze zdefiniowaną temperaturą.

### 5. Kontrola wersji (Git)
Kod został zsynchronizowany ze zdalnym repozytorium na GitHubie. 


## Lab04

## 1. Struktura topiców MQTT
Wiadomości pomiarowe publikowane są w następującej strukturze:
`lab/<group_id>/<device_id>/<sensor>`

* **Zasady nazewnictwa:** topic musi być pisany wyłącznie małymi literami, bez spacji i bez polskich znaków.
* **Przykład:** `lab/g03/esp32-ec0ead004f8c/azimuth`


![Struktura topiców w MQTT Explorer](img\topic.png)

## 2. Opis wiadomości JSON (v1)
Payload każdej wiadomości jest płaskim obiektem JSON. Każda wiadomość reprezentuje pojedynczą próbkę pomiarową z jednego czujnika w danym momencie czasu, wzbogaconą o metadane urządzenia.

## 3. Pola wymagane
Każda wiadomość pomiarowa musi bezwzględnie zawierać poniższe pola:
* `device_id` (string) – unikalny identyfikator urządzenia.
* `sensor` (string) – nazwa rodzaju sensora lub typu danych.
* `value` (number) – faktyczna wartość pomiaru.
* `ts_ms` (integer) – czas pomiaru zapisany jako liczba milisekund od epoki Unix.

## 4. Pola opcjonalne (zalecane)
* `schema_version` (integer) – wersja kontraktu danych. -na razie brak implementacji
* `group_id` (string) – identyfikator grupy laboratoryjnej. -na razie brak implementacji
* `unit` (string) – jednostka fizyczna wartości. - zaimplementowano tę wartość
* `seq` (integer) – rosnący numer sekwencyjny wiadomości. -  na razie brak implementacji

## 5. Podstawowe reguły walidacji
Ingestor odrzuca wiadomości, które nie spełniają następujących warunków:
* `device_id` musi być niepustym napisem.
* `sensor` musi być napisem.
* `value` musi być poprawną liczbą (niedopuszczalny jest zapis liczby jako string).
* `ts_ms` musi być dodatnią liczbą całkowitą.
* `unit` (jeśli występuje) musi odpowiadać typowi sensora[cite: 212].
* [cite_start]`seq` (jeśli występuje) musi być liczbą całkowitą nieujemną[cite: 213].

## 6. Przykład wiadomości poprawnej
```json
{
  "schema_version": 1,
  "group_id": "g03",
  "device_id": "esp32-ec0ead004f8c",
  "sensor": "azimuth",
  "value": 184.25,
  "unit": "deg",
  "ts_ms": 1742030400000,
  "seq": 12
}
```
## 7. Przykłady wiadomości błednej
```
{
  "device_id": "esp32-ec0ead004f8c",
  "sensor": "temperature",
  "value": "24.5",
  "unit": "C"
}

{
  "schema_version": 1,
  "group_id": "g03",
  "sensor": "oxygen",
  "value": 7.8,
  "unit": "mg/L",
  "ts_ms": 1742030405000,
  "seq": 13
}
```
## 8. Uwagi dotyczące środowiska testowego (Źródła danych)
W aktualnej fazie laboratoryjnej urządzenie ESP32 wysyła mieszankę danych rzeczywistych oraz symulowanych w celu testowania obciążenia i wykresów:

* **Temperatura (`temperature`)** – rzeczywisty odczyt temperatury rdzenia ESP32 (otrzymane wartości są rzędu 40-50°C). Jednostka: `C`.
* **Azymut (`azimuth`)** – dane symulowane matematycznie (funkcja sinus). Wartości płynnie falują w zakresie od 170.0 do 190.0 ze stałym okresem wynoszącym 60 sekund. Jednostka: `deg`.
![Wykres falującego azymutu w MQTT Explorerze](img\screen.png)


## Lab05

# Dokumentacja usługi Ingestor

## 1. Cel i opis usługi
Usługa **Ingestor** jest kluczowym elementem systemu zbierania danych pomiarowych. Działa jako pośrednik (subscriber) w architekturze MQTT, którego zadaniem jest:
* Nasłuchiwanie na wiadomości przesyłane do brokera na temacie `lab/+/+/+`.
* Parsowanie danych z formatu JSON.
* Weryfikacja poprawności otrzymanych danych (walidacja pól `device_id`, `sensor`, `value`, `ts_ms`).
* Trwałe zapisywanie pomiarów w relacyjnej bazie danych **PostgreSQL**.



## 2. Model Danych (PostgreSQL)
Dane przechowywane są w tabeli `measurements` w bazie `abcd_db`. Tabela została zaprojektowana tak, aby przechowywać zarówno surowe dane z czujników, jak i metadane dotyczące czasu odebrania pakietu.

### Struktura tabeli SQL:
```sql
CREATE TABLE IF NOT EXISTS measurements (
    id SERIAL PRIMARY KEY,
    group_id TEXT,
    device_id TEXT NOT NULL,
    sensor TEXT NOT NULL,
    value DOUBLE PRECISION NOT NULL,
    unit TEXT,
    ts_ms BIGINT NOT NULL,
    seq INTEGER,
    topic TEXT,
    received_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

### 3. Konfiguracja środowiska 
Usługa została skonteneryzowana przy użyciu Docker. Kluczowe parametry połączenia zdefiniowane w usłudze:
| Zmienna | Wartość |
| :--- | :--- |
| **MQTT_HOST** | `broker` |
| **DB_HOST** | `database` |
| **DB_NAME** | `abcd_db` |
| **DB_USER** | `admin` |
| **DB_PASSWORD** | `admin_pass1234` |


