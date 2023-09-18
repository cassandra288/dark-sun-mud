import psycopg2
from psycopg2.extensions import ISOLATION_LEVEL_AUTOCOMMIT
from dotenv import load_dotenv
import os
import importlib
from inspect import isfunction

load_dotenv()
db_default = os.getenv("DB_DEFAULT")
db_username = os.getenv("DB_USERNAME")
db_password = os.getenv("DB_PASSWORD")
db_host = os.getenv("DB_HOST")
db_port = os.getenv("DB_PORT")

def ensure_database_exists():
  # TODO: Securely pass password
  conn = psycopg2.connect("dbname={} user={} password={} host={} port={}".format(db_default, db_username, db_password, db_host, db_port))
  conn.set_isolation_level(ISOLATION_LEVEL_AUTOCOMMIT)

  cur = conn.cursor()
  cur.execute("SELECT 1 AS result FROM pg_database WHERE datname = 'darksun_server'")

  setup_db = False
  if not cur.fetchone():
      print("Creating database...")
      cur.execute("CREATE DATABASE darksun_server")
      conn.commit()
      setup_db = True

  cur.close()
  conn.close()

  if setup_db:
    conn = psycopg2.connect("dbname=darksun_server user={} password={} host={} port={}".format(db_username, db_password, db_host, db_port))
    cur = conn.cursor()
    cur.execute("CREATE TABLE migrations (id varchar(255) NOT NULL, date timestamp NOT NULL, PRIMARY KEY (id))")
    conn.commit()
    cur.close()
    conn.close()

def run_migrations():
  conn = psycopg2.connect("dbname=darksun_server user={} password={} host={} port={}".format(db_username, db_password, db_host, db_port))
  cur = conn.cursor()
  res = cur.execute("SELECT id FROM migrations")
  migrations = cur.fetchall()
  cur.close()

  for migration_file in os.listdir("migrations"):
    if migration_file.endswith(".py"):
      migration_id = migration_file.replace(".py", "")
      if (migration_id,) not in migrations:
        print("Running migration {}".format(migration_file))
        cur = conn.cursor()

        module = importlib.import_module("migrations" + "." + migration_file.replace(".py", ""))
        for attr_name in dir(module):
          attr = getattr(module, attr_name)
          if isfunction(attr) and attr_name == "up":
            attr(cur)
        
        timestamp = migration_id.split("_")[0]
        cur.execute("INSERT INTO migrations (id, date) VALUES (%s, to_timestamp(%s))", (migration_id, timestamp))
        conn.commit()
        cur.close()


if __name__ == "__main__":
  ensure_database_exists()
  run_migrations()
