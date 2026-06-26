Jasne, oto kompletny, ostateczny plik `README.md` zebrany w jedną, spójną całość. Obejmuje on wszystko od konfiguracji środowiska, przez kolejne laboratoria, aż po dzisiejszą finalizację i interfejs w Streamlicie.

Wystarczy, że skopiujesz poniższy blok i wkleisz do swojego pliku:

```markdown
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
git clone [https://github.com/mateuszbartczak-pwr/Rozproszone-systemy-pomiarowe.git](https://github.com/mateuszbartczak-pwr/Rozproszone-systemy-pomiarowe.git)
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

* REST API (Flask) — http://localhost:5001
* Broker MQTT — localhost:1883
* PostgreSQL — localhost:5432

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
docker compose logs -f api
docker compose logs -f broker
docker compose logs -f database

```

Sprawdzenie statusu kontenerów:

```bash
docker compose ps

```

### Struktura projektu

Repozytorium zawiera między innymi następujące katalogi:

* `api/` — backend REST API
* `broker/` — broker MQTT
* `database/` — baza danych PostgreSQL
* `esp32/` — kod dla urządzeń ESP32
* `ingestor/` — serwis odbierający dane z MQTT i zapisujący je do bazy
* `ui/` — warstwa interfejsu użytkownika
* `docs/` — dokumentacja projektu
* `utils/` — narzędzia pomocnicze

### Uwagi

Projekt będzie rozwijany etapami w trakcie semestru.
W kolejnych zajęciach repozytorium będzie rozszerzane o dodatkowe serwisy, integracje i mechanizmy bezpieczeństwa.

---

## Lab03

Utworzenie kodu esp32 publikującego dane json'owe do brokera

### Wykonane zadania

**1. Konfiguracja środowiska (PlatformIO)**
W pliku `platformio.ini` zdefiniowano środowisko dla płytki `esp32dev` oraz dodano niezbędne biblioteki do obsługi MQTT i formatu JSON:

* `knolleary/PubSubClient` (do komunikacji MQTT)
* `bblanchon/ArduinoJson` (do budowania paczek z danymi)

**2. Konfiguracja poświadczeń (`secrets.h`)**
Utworzono plik `include/secrets.h`, który zawiera:

* SSID i hasło do sieci Wi-Fi.
* Adres brokera MQTT
* Port (1883) oraz identyfikator grupy roboczej.

**3. Główny kod programu (`main.cpp`)**
Napisano program realizujący następujące funkcje:

* **Generowanie unikalnego ID:** Wykorzystano funkcję `ESP.getEfuseMac()`, aby wygenerować unikalny `device_id` na podstawie sprzętowego adresu MAC płytki.
* **Połączenie z Wi-Fi:** Funkcja `connectWiFi()` łącząca ESP32 z lokalną siecią.
* **Połączenie z MQTT:** Funkcja `connectMQTT()` nawiązująca sesję z brokerem.
* **Publikowanie danych (JSON):** Użyto biblioteki `ArduinoJson` (klasa `JsonDocument`) do zbudowania ramki danych zawierającej m.in. ID urządzenia, typ czujnika ("temperature"), wartość i znacznik czasu. Następnie zmodyfikowano kod, aby sczytywał realną wartość temperatury rdzenia mikrokontrolera.

**4. Weryfikacja działania (MQTT Explorer)**
Działanie kodu zweryfikowano za pomocą programu **MQTT Explorer**. Po podłączeniu się do odpowiedniego brokera i zasubskrybowaniu tematu grupy, potwierdzono poprawne, cykliczne odbieranie wiadomości JSON.

---

## Lab04

**1. Struktura topiców MQTT**
Wiadomości pomiarowe publikowane są w następującej strukturze:
`lab/<group_id>/<device_id>/<sensor>`

* **Zasady nazewnictwa:** topic musi być pisany wyłącznie małymi literami, bez spacji i bez polskich znaków.
* **Przykład:** `lab/g03/esp32-ec0ead004f8c/azimuth`

**2. Opis wiadomości JSON (v1)**
Payload każdej wiadomości jest płaskim obiektem JSON. Każda wiadomość reprezentuje pojedynczą próbkę pomiarową z jednego czujnika w danym momencie czasu, wzbogaconą o metadane urządzenia. Zaktualizowano kod ESP32, aby wysyłał pełny, wymagany przez `Ingestor` obiekt JSON (dodano pola autoryzujące typ wiadomości, numer sekwencyjny oraz jednostkę).

**3. Pola wymagane**
Każda wiadomość pomiarowa musi bezwzględnie zawierać poniższe pola:

* `device_id` (string) – unikalny identyfikator urządzenia.
* `type` (string) – autoryzacja wiadomości (wartość `meas`).
* `sensor` (string) – nazwa rodzaju sensora lub typu danych.
* `value` (number) – faktyczna wartość pomiaru.
* `ts_ms` (integer) – czas pomiaru zapisany jako liczba milisekund od epoki Unix.

**4. Pola opcjonalne (zalecane)**

* `schema_version` (integer) – wersja kontraktu danych.
* `group_id` (string) – identyfikator grupy laboratoryjnej (zaimplementowano).
* `unit` (string) – jednostka fizyczna wartości (zaimplementowano).
* `seq` (integer) – rosnący numer sekwencyjny wiadomości (zaimplementowano).

**5. Przykład wiadomości poprawnej z ESP32**

```json
{
  "device_id": "esp32-ec0ead004f8c",
  "type": "meas",
  "sensor": "temperature",
  "value": 45.2,
  "unit": "C",
  "group_id": "g03",
  "seq": 15,
  "ts_ms": 1742030400000
}

```

**6. Uwagi dotyczące środowiska testowego (Źródła danych)**
W aktualnej fazie laboratoryjnej urządzenie ESP32 wysyła:

* **Temperatura (`temperature`)** – rzeczywisty odczyt temperatury rdzenia ESP32 (otrzymane wartości są rzędu 40-50°C). Jednostka: `C`.

---

## Lab05

### Dokumentacja usługi Ingestor

**1. Cel i opis usługi**
Usługa **Ingestor** jest kluczowym elementem systemu zbierania danych pomiarowych. Działa jako pośrednik (subscriber) w architekturze MQTT, którego zadaniem jest:

* Nasłuchiwanie na wiadomości przesyłane do brokera na temacie `lab/+/+/+`.
* Parsowanie danych z formatu JSON.
* Weryfikacja poprawności otrzymanych danych (walidacja pól `type`, `device_id`, `sensor`, `value`, `ts_ms`).
* Trwałe zapisywanie pomiarów w relacyjnej bazie danych **PostgreSQL**.

**2. Model Danych (PostgreSQL)**
Dane przechowywane są w tabeli `measurements` w bazie `abcd_db`. Zaktualizowano zapytania SQL, aby w pełni obsługiwały również opcjonalne kolumny, tworząc kompletną strukturę pod API.

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

---

## Lab10

Laboratorium 10 - Zabezpieczenie komunikacji w systemie IoT (TLS / MQTT)

**Cel laboratorium:**
Głównym celem ćwiczenia było wdrożenie mechanizmów bezpieczeństwa. Obejmowało to implementację szyfrowania TLS dla komunikacji z brokerem MQTT, zabezpieczenie infrastruktury Docker oraz konfigurację mikrokontrolera ESP32.

**1. Generowanie certyfikatów kryptograficznych**

* Utworzono własny urząd certyfikacji (CA) oraz wygenerowano klucze dla serwera (brokera MQTT).
* Wygenerowane pliki umieszczono w kontekście budowania usługi brokera (folder `/broker/certs`).

**2. Konfiguracja środowiska (Docker Compose)**

* **Izolacja bazy danych:** Usunięto publiczne mapowanie portów dla bazy PostgreSQL (`5432:5432`), co uniemożliwia bezpośredni dostęp z zewnątrz.
* **Bezpieczna sieć:** Skonfigurowano wewnętrzną sieć typu `bridge` i przypisano do niej wszystkie kontenery.
* **Broker MQTT:** Zmieniono konfigurację Mosquitto, zmuszając usługę do używania certyfikatów i nasłuchiwania na bezpiecznym porcie **8883**.

**3. Przygotowanie konfiguracji klienta (ESP32)**

* **Synchronizacja czasu (NTP):** Upewniono się, że mikrokontroler przed próbą połączenia MQTT wykonuje `synchronizeTime()`, aby poprawnie weryfikować daty certyfikatów.
* Zastąpiono `WiFiClient` bezpiecznym odpowiednikiem `WiFiClientSecure` i zaimplementowano certyfikat CA na poziomie kodu sprzętowego.

> Pomimo pomyślnego skonfigurowania i przetestowania szyfrowania TLS w izolowanym środowisku, ze względu na techniczne problemy z lokalnym rutingiem i zaporą sieciową (Firewall blokujący handshake TLS na mostku sieciowym WSL2-Windows), podjęto decyzję o wycofaniu się z szyfrowania dla połączeń zewnętrznych z ESP32.
> **Finalna, działająca wersja projektu używa standardowego, nieszyfrowanego połączenia na porcie 1883 oraz obiektu `WiFiClient` na urządzeniu ESP32, zapewniając stabilny i ciągły przepływ danych ze świata fizycznego do środowiska Dockerowego.**

---

## Finalizacja Projektu (REST API & Dashboard)

Ostatni etap projektu polegał na udostępnieniu zgromadzonych danych oraz zbudowaniu interaktywnego panelu operatorskiego zastępującego środowisko LabVIEW.

**1. Budowa REST API (Flask)**
Napisano w pełni funkcjonalne API w języku Python (framework Flask), pełniące rolę bramy dostępowej do bazy PostgreSQL:

* **Autoryzacja:** Całe API zabezpieczono mechanizmem *Basic Auth* (login/hasło).
* **Endpoint `/health`:** Diagnostyczna weryfikacja statusu i dostępności serwisu.
* **Endpoint `/measurements/latest`:** Pobieranie najświeższego rekordu pomiarowego z bazy.
* **Endpoint `/measurements/history`:** Pobieranie historii pomiarów z pełną obsługą parametrów filtrowania (Query Params: `limit`, `sensor`, `device_id`).
* **Translacja Danych:** Zaimplementowano funkcjonalność (`measurement_to_dict`), mapującą wyniki zapytań SQL na kompletne struktury JSON, które zawierają wszystkie uzupełnione metadane (w tym `seq`, `group_id`, `unit`).

**2. Interfejs Użytkownika (Streamlit)**
Stworzono nowoczesny, w 100% webowy i w pełni zintegrowany Dashboard z użyciem biblioteki Streamlit, pełniący funkcję pełnoprawnego klienta REST:

* **Panel diagnostyczny i konfiguracyjny:** Umożliwia uwierzytelnianie (Basic Auth), testowanie połączenia z endpointami API oraz intuicyjne filtrowanie danych wejściowych (wybór urządzenia z rozwijanej listy z użyciem funkcji `@st.cache_data`, wybór sensora, limit rekordów).
* **Prezentacja Ostatniego Pomiaru:** Wykorzystano dedykowane komponenty UI (metryki) do czytelnej prezentacji najświeższych danych nadchodzących z ESP32.
* **Wizualizacja i Trend (Pandas):** Aplikacja pobiera surowy JSON z API i przetwarza go w "locie" do struktury `DataFrame` biblioteki Pandas, budując dynamiczne wykresy liniowe.
* **Obsługa stref czasowych i formatowanie:** Rozwiązano problem stref czasowych (Unix Timestamp UTC), konwertując czas bezpośrednio w bibliotece Pandas na polską strefę czasową (`Europe/Warsaw`) wraz z formatowaniem stringów (odrzucenie offsetu `+02:00`), zapewniając najwyższą czytelność tabelaryczną dla operatora.

