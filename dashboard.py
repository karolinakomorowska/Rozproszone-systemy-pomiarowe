import streamlit as st
import requests
import pandas as pd

# Adres Twojego API
API_URL = "http://localhost:5001"

st.set_page_config(page_title="Historia Pomiarów", layout="wide")
st.title("📈 Historia Pomiarów (ESP32)")

# --- PANEL BOCZNY: UWIERZYTELNIANIE ---
st.sidebar.header("Uwierzytelnianie (Basic Auth)")
username = st.sidebar.text_input("Nazwa użytkownika", value="student")
password = st.sidebar.text_input("Hasło", value="student", type="password")
use_auth = st.sidebar.checkbox("Użyj Basic Auth", value=True)

def get_auth():
    if use_auth and username and password:
        return (username, password)
    return None

# --- GŁÓWNY PANEL: WYKRES ---
if st.button("Pobierz dane i narysuj wykres", type="primary"):
    try:
        # Odpytujemy API Z AUTORYZACJĄ:
        res = requests.get(f"{API_URL}/measurements/history", auth=get_auth())
        
        if res.status_code == 200:
            st.success("Pomyślnie pobrano dane!")
            data = res.json()
            
            if len(data) == 0:
                st.warning("Baza danych jest pusta. Upewnij się, że ESP32 wysyła pomiary.")
            else:
                # Zamieniamy JSON na tabelę danych (DataFrame)
                df = pd.DataFrame(data)
                
                # Ustawiamy oś czasu na wykresie
                if 'ts_ms' in df.columns:
                    df['Czas'] = pd.to_datetime(df['ts_ms'], unit='ms')
                    df.set_index('Czas', inplace=True)
                elif 'timestamp' in df.columns:
                    df.set_index('timestamp', inplace=True)
                
                # Rysujemy wykres
                if 'value' in df.columns:
                    st.line_chart(df['value'])
                else:
                    st.line_chart(df)
                
                # Pokazujemy surowe dane
                with st.expander("Zobacz surowe dane w tabeli"):
                    st.dataframe(df)
                    
        elif res.status_code == 401:
            st.error("BŁĄD 401: Brak dostępu! Zaznacz 'Użyj Basic Auth' z lewej strony i sprawdź hasło.")
        else:
            st.error(f"Nieznany błąd HTTP: {res.status_code}")
            
    except Exception as e:
        st.error(f"Nie udało się połączyć z API: {e}.")