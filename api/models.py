from sqlalchemy.orm import declarative_base
from sqlalchemy import Column, String, Integer, Double, BigInteger, TIMESTAMP

Base = declarative_base()

class Sample(Base):
    __tablename__ = "measurements"

    id = Column(Integer, primary_key=True)
    group_id = Column(String)
    device_id = Column(String)
    sensor = Column(String)
    value = Column(Double)
    unit = Column(String)
    ts_ms = Column(BigInteger)
    seq = Column(Integer)
    topic = Column(String)
    received_at = Column(TIMESTAMP)

    def to_dict(self):
        return {
            "id": self.id,
            "group_id": self.group_id,
            "device_id": self.device_id,
            "sensor": self.sensor,
            "value": self.value,
            "unit": self.unit,
            "ts_ms": self.ts_ms,
            "seq": self.seq,
            "topic": self.topic,
            "received_at": self.received_at.isoformat() if self.received_at else None
        }
    

def make_response(list_of_Samples):
        return [x.to_dict() for x in list_of_Samples]