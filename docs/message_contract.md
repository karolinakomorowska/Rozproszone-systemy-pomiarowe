# Kontrakt danych - Projekt Rozproszone Systemy Pomiarowe

## 1. Opis struktury topiców MQTT
Wiadomości są publikowane według schematu pozwalającego na rozróżnienie wielu czujników na jednym urządzeniu:
`lab/<group_id>/<device_id>/<sensor_type>`

**Przykłady:**
* `lab/g03/esp32-01/temperature`
* `lab/g03/esp32-01/humidity`

## 2. Opis wiadomości JSON

Urządzenia wysyłają dwa rodzaje wiadomości: pomiarowe (`type: "meas"`) oraz statusowe (`type: "status"`).

### 2.1 Wiadomość pomiarowa

Publikowana na topic: `lab/<group_id>/<device_id>/<sensor_type>`

```json
{
  "schema_version": 2,
  "group_id": "g02",
  "device_id": "esp32-01",
  "sensor": "temperature",
  "value": 24.5,
  "unit": "C",
  "ts_ms": 1742030400000,
  "seq": 1,
  "type": "meas"
}
```

### 2.2 Wiadomość statusowa

Publikowana na topic: `lab/<group_id>/<device_id>/status`

```json
{
  "schema_version": 2,
  "group_id": "g02",
  "device_id": "esp32-01",
  "status": "online",
  "ts_ms": 1742030400000,
  "seq": 0,
  "type": "status"
}
```

## 3. Lista pól wymaganych

### Wiadomość pomiarowa (`type: "meas"`)
* type, typ: string, wartość: `"meas"`
* device_id, typ: string
* sensor, typ: string
* value, typ: number
* ts_ms, typ: integer, >0

### Wiadomość statusowa (`type: "status"`)
* type, typ: string, wartość: `"status"`
* device_id, typ: string
* status, typ: string
* ts_ms, typ: integer, >0


## 4. Lista pól opcjonalnych 
* schema_version, typ: integer
* group_id, typ: string
* unit, typ: string (tylko wiadomość pomiarowa)
* seq, typ: integer, $\ge 0$


## 5. Przykłady wiadomości błędnych
Przedstawiono przykłady błędnej wiadomości pomiarowej.

1.  W tej wiadomości brakuje pola ts_ms, które jest kluczowe w określeniu dokładnego czasu pomiaru. 

```json

{
  "device_id": "esp32-01",
  "sensor": "temperature",
  "value": 24.5
}
```
2. W tej wiadomości wartość 24.5 zapisana jest jako string, a nie jako number, co uniemoliwia poprawny odczyt.
```json

{
  "device_id": "esp32-01",
  "sensor": "temperature",
  "value": "24.5",
  "ts_ms": 1742030400000
}
```
3.  W tej wiadomości brak pola device_id uniemożliwia przypisanie pomiaru do źródła.
```json
{
  "sensor": "temperature",
  "value": 24.5,
  "ts_ms": 1742030400000
}
```


