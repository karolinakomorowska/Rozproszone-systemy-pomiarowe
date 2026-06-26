# Distributed IoT Measurement System

A comprehensive distributed measurement system (IoT) project that implements a complete data flow: from edge devices, through a message broker and persistence layer, to data exposure via a REST API and an interactive analytical dashboard.

This project demonstrates the ability to build asynchronous, microservices-based systems and integrate hardware with a modern server-side technology stack.

---

##  Tech Stack & Architecture

The system is designed with modularity and containerization in mind. It consists of the following components:

* **Edge / Hardware:** ESP32, C++ (PlatformIO), `PubSubClient`, `ArduinoJson`
* **Message Broker:** Eclipse Mosquitto (MQTT)
* **Data Ingestion:** Python (MQTT Subscriber -> DB)
* **Database:** PostgreSQL
* **Backend REST API:** Python, Flask (Basic Auth)
* **Frontend / Dashboard:** Streamlit, Pandas
* **Infrastructure:** Docker, Docker Compose

---

## Core System Components

### 1. Edge Layer (ESP32)
Edge devices (built around the ESP32 microcontroller) are responsible for data acquisition (currently reading the core temperature) and publishing it to the MQTT broker.
* **Identification:** Each device generates a unique `device_id` based on its hardware MAC address (`ESP.getEfuseMac()`).
* **Data Format:** Measurements are transmitted in a structured JSON format according to a defined Data Contract.
* **MQTT Topics:** Data is published using a standardized topic pattern: `lab/<group_id>/<device_id>/<sensor>`.

**Example Payload (Data Contract v1):**
```json
{
  "device_id": "esp32-ec0ead004f8c",
  "type": "meas",
  "sensor": "temperature",
  "value": 45.2,
  "unit": "C",
  "seq": 15,
  "ts_ms": 1742030400000
}
```
### 2. Ingestion & Storage Layer (Ingestor + PostgreSQL)
The Ingestor microservice acts as a dedicated subscriber (listening on the lab/+/+/+ wildcard). Its primary task is to validate incoming JSON payloads (verifying message types, authorization, and data integrity) and persistently store them in a PostgreSQL relational database.

The measurements table structure fully reflects the data contract, retaining the received_at timestamp for audit and troubleshooting purposes.

### 3. Backend (REST API)
A data access gateway built with the Flask framework.

* Secured with Basic Auth.

* Exposes optimized endpoints:

** /health – Service health check and diagnostics.

** /measurements/latest – Retrieves the most recent sensor reading.

** /measurements/history – Retrieves historical data with full Query Params support (filtering by limits, specific devices, and sensors).

* Handles the mapping of SQL query results to normalized JSON objects.
4. User Interface (Streamlit Dashboard)
* A 100% web-based, interactive operator dashboard integrated with the REST API.

* Fetches raw JSON data and processes it on the fly using the Pandas library.

* converts UNIX timestamps (UTC) to the local time zone (Europe/Warsaw).

* Provides dynamic trend visualization (line charts) and clear current metrics.

* Optimizes API queries using built-in caching (@st.cache_data)
* 
## Security & Network Isolation
* Docker Isolation: The PostgreSQL database does not expose port 5432 to the host machine. All communication occurs exclusively over an internal Docker bridge network.

* API Authorization: Accessing data from the frontend or third-party clients requires proper authentication.

* TLS vs. Dev Environment: The system architecture anticipates TLS encryption for the MQTT broker (port 8883) and the use of WiFiClientSecure on the ESP32 (CA key infrastructure is included in the repository). However, due to local environment constraints (WSL2 network bridge firewall rules blocking TLS handshakes on Windows), the project defaults to a stable development mode utilizing standard port 1883.

