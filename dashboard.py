import streamlit as st
import requests
import pandas as pd

# Adres API
API_URL = "http://localhost:5001"

st.set_page_config(page_title="Dashboard Pomiarowy", layout="wide")

# --- NAGŁÓWEK I STATUS ---
st.title("Rozproszone Systemy Pomiarowe - Dashboard")
st.markdown("Interfejs operatorski REST API dla danych z ESP32.")

# --- PANEL BOCZNY: UWIERZYTELNIANIE I FILTRY ---
st.sidebar.header("🔑 Uwierzytelnianie (Basic Auth)")
username = st.sidebar.text_input("Nazwa użytkownika", value="student")
password = st.sidebar.text_input("Hasło", value="student", type="password")
use_auth = st.sidebar.checkbox("Użyj Basic Auth", value=True)

def get_auth():
    if use_auth and username and password:
        return (username, password)
    return None

st.sidebar.markdown("---")
st.sidebar.header("Diagnostyka i Połączenie")

# 1. Endpoint: /health
if st.sidebar.button("Test API (/health)", type="secondary"):
    try:
        res = requests.get(f"{API_URL}/health", auth=get_auth(), timeout=3)
        if res.status_code == 200:
            st.sidebar.success("Status API: OK (Połączono)")
        elif res.status_code == 401:
            st.sidebar.error("BŁĄD 401: Brak autoryzacji!")
        else:
            st.sidebar.warning(f"Zła odpowiedź HTTP: {res.status_code}")
    except Exception as e:
        st.sidebar.error(f"Brak połączenia z backendem: {e}")

st.sidebar.markdown("---")
st.sidebar.header("Konfiguracja i filtry")

# 2. Endpoint: /devices (Pobranie listy urządzeń)
@st.cache_data(ttl=10) # Cache, aby nie spamować API przy każdym przeładowaniu
def get_devices():
    try:
        res = requests.get(f"{API_URL}/devices", auth=get_auth(), timeout=3)
        if res.status_code == 200:
            data = res.json()
            # Zależnie jak Twoje API zwraca dane (lista stringów czy lista słowników)
            if len(data) > 0 and isinstance(data[0], dict):
                return [d.get("device_id", "Nieznane") for d in data]
            return data
        return []
    except:
        return []

devices_list = get_devices()
if not devices_list:
    devices_list = ["Brak urządzeń / Wpisz ręcznie"]

selected_device = st.sidebar.selectbox("Wybierz urządzenie (device_id)", devices_list)
# Opcja ręcznego wpisania, gdyby baza chwilowo była pusta
manual_device = st.sidebar.text_input("Lub wpisz ID ręcznie", value="")
final_device = manual_device if manual_device else selected_device

selected_sensor = st.sidebar.text_input("Sensor", value="temperature")
selected_limit = st.sidebar.number_input("Limit rekordów", min_value=1, max_value=1000, value=50)


# --- GŁÓWNY PANEL: PREZENTACJA DANYCH ---
col1, col2 = st.columns([1, 1])

with col1:
    st.subheader("📌 Ostatni pomiar (Latest)")
    # 3. Endpoint: /measurements/latest
    if st.button("Pobierz latest"):
        try:
            res = requests.get(f"{API_URL}/measurements/latest", auth=get_auth(), timeout=3)
            if res.status_code == 200:
                data = res.json()
                if not data:
                    st.warning("Brak danych pomiarowych w bazie.")
                else:
                    st.success("Pomyślnie pobrano ostatni pomiar.")
                    # Rozbicie danych na estetyczne kafelki (metryki)
                    c1, c2, c3 = st.columns(3)
                    c1.metric("Urządzenie", data.get("device_id", "-"))
                    c2.metric("Sensor", data.get("sensor", "-"))
                    c3.metric("Wartość", f"{data.get('value', '-')} {data.get('unit', '')}")
                    st.caption(f"Timestamp (ms): {data.get('ts_ms', '-')}")
            elif res.status_code == 401:
                st.error("Brak dostępu (401). Sprawdź hasło.")
            else:
                st.error(f"Błąd HTTP: {res.status_code}")
        except Exception as e:
            st.error(f"Błąd połączenia: {e}")

with col2:
    st.subheader("📈 Wykres i Historia")
    # 4. Endpoint: /measurements/history z filtrowaniem
    if st.button("Pobierz historię i filtruj", type="primary"):
        try:
            params = {
                "device_id": final_device,
                "sensor": selected_sensor,
                "limit": selected_limit
            }
            res = requests.get(f"{API_URL}/measurements/history", params=params, auth=get_auth(), timeout=3)
            
            if res.status_code == 200:
                data = res.json()
                if len(data) == 0:
                    st.warning(f"Brak danych dla filtra: {final_device} | {selected_sensor}")
                else:
                    st.success(f"Pobrano {len(data)} rekordów.")
                    df = pd.DataFrame(data)
                    
                    # Parsowanie czasu, zamiana na strefę PL i formatowanie do czystego tekstu
                    if 'ts_ms' in df.columns:
                        df['Czas'] = pd.to_datetime(df['ts_ms'], unit='ms').dt.tz_localize('UTC').dt.tz_convert('Europe/Warsaw').dt.strftime('%Y-%m-%d %H:%M:%S')
                        df.set_index('Czas', inplace=True)
                        df.sort_index(inplace=True)
                    
                    # Rysowanie wykresu na głównej szerokości pod spodem
                    st.line_chart(df['value'] if 'value' in df.columns else df)
                    
                    # Tabela surowych danych historycznych
                    with st.expander("Tabela ostatnich rekordów"):
                        st.dataframe(df)
            
            elif res.status_code == 401:
                st.error("Brak dostępu (401). Sprawdź hasło.")
            else:
                st.error(f"Błąd HTTP: {res.status_code}")
        except Exception as e:
            st.error(f"Błąd połączenia: {e}")