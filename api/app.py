from flask import Flask, jsonify, request 
from models import measurement_to_dict
from db import get_connection

app = Flask(__name__)

@app.route("/")
def hello_world():
    return "<p>Hello World!</p>"

# 1. Sprawdzenie stanu aplikacji
@app.route("/health", methods=["GET"])
def health():
    return jsonify({"status": "ok"})

# 2. Pobranie 20 ostatnich pomiarów
@app.route("/measurements", methods=["GET"])
def get_measurements():
    try:
        conn = get_connection()
        cur = conn.cursor()
        cur.execute("""
            SELECT id, group_id, device_id, sensor, value, unit, ts_ms, seq, topic
            FROM measurements
            ORDER BY id DESC
            LIMIT 20
        """)
        rows = cur.fetchall()
        cur.close()
        conn.close()

        result = [measurement_to_dict(row) for row in rows]
        return jsonify(result)
    except Exception as e:
        return jsonify({"error": str(e)}), 500

# 3. Pobranie najnowszego pomiaru
@app.route("/measurements/latest", methods=["GET"])
def get_latest_measurement():
    try:
        conn = get_connection()
        cur = conn.cursor()
        cur.execute("""
            SELECT id, group_id, device_id, sensor, value, unit, ts_ms, seq, topic
            FROM measurements
            ORDER BY id DESC
            LIMIT 1
        """)
        row = cur.fetchone()
        cur.close()
        conn.close()

        if row is None:
            return jsonify({"message": "Brak danych"}), 404

        return jsonify(measurement_to_dict(row))
    except Exception as e:
        return jsonify({"error": str(e)}), 500

# 4. Historia z filtrowaniem
@app.route("/measurements/history", methods=["GET"])
def get_measurement_history():
    try:
        device_id = request.args.get("device_id")
        group_id = request.args.get("group_id")
        sensor = request.args.get("sensor")
        limit = request.args.get("limit", default=20, type=int)

        conn = get_connection()
        cur = conn.cursor()
        
        # SQL z "WHERE 1=1" pozwala na łatwe dodawanie kolejnych filtrów
        query = """
            SELECT id, group_id, device_id, sensor, value, unit, ts_ms, seq, topic
            FROM measurements
            WHERE 1=1
        """
        params = []

        if device_id:
            query += " AND device_id = %s"
            params.append(device_id)

        if group_id:
            query += " AND group_id = %s"
            params.append(group_id)

        if sensor:
            query += " AND sensor = %s"
            params.append(sensor)

        query += " ORDER BY id DESC LIMIT %s"
        params.append(limit)

        cur.execute(query, params)
        rows = cur.fetchall()
        cur.close()
        conn.close()

        result = [measurement_to_dict(row) for row in rows]
        return jsonify(result)
    except Exception as e:
        return jsonify({"error": str(e)}), 500
    

# 5. Dostępne urządzenia
@app.route("/devices", methods=["GET"])
def get_devices_list():
    try:
        conn = get_connection()
        cur = conn.cursor()
        cur.execute("""
            SELECT DISTINCT device_id
            FROM measurements
            WHERE device_id IS NOT NULL
            ORDER BY device_id
        """)
        rows = cur.fetchall()
        cur.close()
        conn.close()

        result = [row[0] for row in rows]
        return jsonify(result)
    except Exception as e:
        return jsonify({"error": str(e)}), 500
    
# 6. Dostępne czujniki
@app.route("/sensors", methods=["GET"])
def get_sensors_list():
    try:
        device_id = request.args.get("device_id")
        conn = get_connection()
        cur = conn.cursor()

        query = """
            SELECT DISTINCT device_id, sensor
            FROM measurements
            WHERE 1=1
        """

        params = []

        if device_id:
            query += " AND device_id = %s"
            params.append(device_id)

        query += " ORDER BY device_id"

        cur.execute(query, params)
        rows = cur.fetchall()
        cur.close()
        conn.close()

        result = [{"device_id": row[0], "sensor": row[1]} for row in rows]
        return jsonify(result)
    except Exception as e:
        return jsonify({"error": str(e)}), 500
    

if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5002)
