import psycopg2
import sys
import os
from dotenv import load_dotenv
import importlib
from inspect import isfunction

load_dotenv()
db_username = os.getenv("DB_USERNAME")
db_password = os.getenv("DB_PASSWORD")
db_host = os.getenv("DB_HOST")
db_port = os.getenv("DB_PORT")

def rollback(count):
  conn = psycopg2.connect("dbname=darksun_server user={} password={} host={} port={}".format(db_username, db_password, db_host, db_port))
  cur = conn.cursor()
  if count < 0:
    res = cur.execute("SELECT id FROM migrations ORDER BY date DESC")
  else:
    res = cur.execute("SELECT id FROM migrations ORDER BY date DESC LIMIT {}".format(count))
  migrations = cur.fetchall()
  cur.close()

  rollbacks_run = 0
  for migration in migrations:
    migration_id = migration[0]
    print("Rolling back migration {}".format(migration_id))
    cur = conn.cursor()

    module = importlib.import_module("migrations" + "." + migration_id)
    for attr_name in dir(module):
      attr = getattr(module, attr_name)
      if isfunction(attr) and attr_name == "down":
        attr(cur)
    
    cur.execute("DELETE FROM migrations WHERE id = '{}'".format(migration_id))
    conn.commit()
    cur.close()

    rollbacks_run += 1
    if count > 0 and rollbacks_run >= count:
      break

if __name__ == "__main__":
  count = 1
  if len(sys.argv) >= 2:
    if sys.argv[1] == "all":
      count = -1
    else:
      count = int(sys.argv[1])
  
  rollback(count)
