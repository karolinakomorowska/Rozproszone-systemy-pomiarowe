from flask import Flask, jsonify, request
from psycopg2.extras import RealDictCursor
from db import get_db_connection

app = Flask(__name__)

# 1. Endpoint /health (wymagany)
@app.route("/health")
def health_check():
    return jsonify({"status": "healthy"})

# 2. Endpoint /measurements (zwraca ogólną listę)
@app.route("/measurements")
def get_measurements():
    conn = get_db_connection()
    cur = conn.cursor(cursor_factory=RealDictCursor)
    cur.execute("SELECT * FROM measurements ORDER BY ts_ms DESC LIMIT 20;")
    pomiary = cur.fetchall()
    cur.close()
    conn.close()
    return jsonify(pomiary)

# 3. Endpoint /measurements/latest (wymagany - zwraca tylko najnowszy wynik)
@app.route("/measurements/latest")
def get_latest():
    conn = get_db_connection()
    cur = conn.cursor(cursor_factory=RealDictCursor)
    # LIMIT 1 oznacza, że bierzemy tylko pierwszy od góry wynik
    cur.execute("SELECT * FROM measurements ORDER BY ts_ms DESC LIMIT 1;")
    pomiar = cur.fetchone() # fetchone() zamiast fetchall() bo to tylko jeden rekord
    cur.close()
    conn.close()
    return jsonify(pomiar)

# 4. Endpoint /measurements/history (wymagany - filtrowanie i dodawanie pól)
@app.route("/measurements/history")
def get_history():
    # To są te "dodawane pola"! Odbieramy parametry z adresu URL.
    # Np. uzytkownik wpisze: /measurements/history?sensor=temperature&limit=5
    sensor_type = request.args.get('sensor')
    limit = request.args.get('limit', 10, type=int) # domyślnie 10, jeśli ktoś nie poda
    
    conn = get_db_connection()
    cur = conn.cursor(cursor_factory=RealDictCursor)
    
    if sensor_type:
        # Jeśli ktoś podał sensor, filtrujemy wyniki (WHERE sensor = ...)
        cur.execute("SELECT * FROM measurements WHERE sensor = %s ORDER BY ts_ms DESC LIMIT %s;", (sensor_type, limit))
    else:
        # Jeśli nie, dajemy wszystko
        cur.execute("SELECT * FROM measurements ORDER BY ts_ms DESC LIMIT %s;", (limit,))
        
    pomiary = cur.fetchall()
    cur.close()
    conn.close()
    return jsonify(pomiary)

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5001)
