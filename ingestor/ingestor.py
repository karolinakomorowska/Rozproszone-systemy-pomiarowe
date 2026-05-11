import json
import paho.mqtt.client as mqtt
from db import get_connection

MQTT_HOST = "broker"
MQTT_PORT = 1883
MQTT_TOPIC = "lab/+/+/+"

MES_TYPE = "type"

def save_measurement(topic, data):
  conn = get_connection()
  cur = conn.cursor()
  cur.execute("""
    INSERT INTO measurements
    (group_id, device_id, sensor, value, unit, ts_ms, seq, topic)
    VALUES (%s, %s, %s, %s, %s, %s, %s, %s)
  """, (
    data.get("group_id"),
    data["device_id"],
    data["sensor"],
    data["value"],
    data.get("unit"),
    data["ts_ms"],
    data.get("seq"),
    topic
  ))
  conn.commit()
  cur.close()
  conn.close()

def save_status(topic, data):
  conn = get_connection()
  cur = conn.cursor()
  cur.execute("""
    INSERT INTO statuses
    (group_id, device_id, status, ts_ms, seq, topic)
    VALUES (%s, %s, %s, %s, %s, %s)
  """, (
    data.get("group_id"),
    data["device_id"],
    data["status"],
    data["ts_ms"],
    data.get("seq"),
    topic
  ))
  conn.commit()
  cur.close()
  conn.close()

def is_measurment_valid(data):
  required = ["device_id", "sensor", "value", "ts_ms"]
  return all(field in data for field in required)

def is_status_valid(data):
  required = ["device_id", "status", "ts_ms"]
  return all(field in data for field in required)

def on_connect(client, userdata, flags, rc):
  print("Connected with result code", rc)
  client.subscribe(MQTT_TOPIC)

def on_message(client, userdata, msg):
  try:
    payload = msg.payload.decode("utf-8")
    data = json.loads(payload)

    if MES_TYPE not in data:
      print("Invalid payload:", data)
      return

    if data[MES_TYPE] == "meas" and is_measurment_valid(data):
      save_measurement(msg.topic, data)
    elif data[MES_TYPE] == "status" and is_status_valid(data):
      save_status(msg.topic, data)
    else:
      print("Invalid payload:", data)
      return
    
    print("Saved message from topic:", msg.topic)

  except Exception as e:
    print("Error:", e)


client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect(MQTT_HOST, MQTT_PORT, 60)
client.loop_forever()