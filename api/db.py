import psycopg2
from psycopg2.extras import RealDictCursor
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker

# To są dane logowania do Twojej bazy (te same co w Ingestorze)
DB_HOST = "localhost"
DB_NAME = "abcd_db"
DB_USER = "admin"
DB_PASSWORD = "admin_pass1234"

DATABASEURL = f"postgresql+psycopg2://{DB_USER}:{DB_PASSWORD}@{DB_HOST}:5432/{DB_NAME}"

engine = create_engine(DATABASEURL)
SessionLocal = sessionmaker(bind=engine, autoflush=False, autocommit=False)

def get_db_connection():
    # Ta funkcja puka do drzwi bazy danych i otwiera połączenie
    conn = psycopg2.connect(
        host=DB_HOST,
        database=DB_NAME,
        user=DB_USER,
        password=DB_PASSWORD
    )
    return conn