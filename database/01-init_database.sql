-- EXAMPLE 
-- TODO
CREATE TABLE sensor (
    uuid UUID PRIMARY KEY,
    name TEXT NOT NULL,
    type TEXT,
    sensor TEXT,
    apidetails TEXT,
    is_online BOOLEAN DEFAULT FALSE
);

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

INSERT INTO measurements
	(group_id,device_id,sensor,value,unit,ts_ms,seq,topic)
	VALUES ('g03', 'esp32-abcd01','temperature', 21.5, 'C', 1742030400000, 15, 'lab/g03/esp32-abcd01/temperature');

INSERT INTO measurements
	(group_id,device_id,sensor,value,unit,ts_ms,seq,topic)
	VALUES ('g03', 'esp32-abcd02','humidity', 55, '%', 1742030440000, 16, 'lab/g03/esp32-abcd02/humidity');    

INSERT INTO measurements
	(group_id,device_id,sensor,value,unit,ts_ms,seq,topic)
	VALUES ('g03', 'esp32-abcd03','pressure', 991.5, 'hPa', 1742030460000, 17, 'lab/g03/esp32-abcd01/pressure');  
