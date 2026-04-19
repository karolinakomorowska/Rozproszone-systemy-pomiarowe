import psycopg2
from psycopg2.extras import RealDictCursor

# To są dane logowania do Twojej bazy (te same co w Ingestorze)
DB_HOST = "database"
DB_NAME = "abcd_db"
DB_USER = "admin"
DB_PASSWORD = "admin_pass1234"

def get_db_connection():
    # Ta funkcja puka do drzwi bazy danych i otwiera połączenie
    conn = psycopg2.connect(
        host=DB_HOST,
        database=DB_NAME,
        user=DB_USER,
        password=DB_PASSWORD
    )
    return conn