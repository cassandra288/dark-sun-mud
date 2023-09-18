import sys
import os
import datetime
from datetime import timezone

if __name__ == "__main__":
  if len(sys.argv) < 2:
    print("Usage: create_migration.py <migration_name>")
    exit(1)
  
  migration_name = sys.argv[1]
  migration_name = migration_name.replace(" ", "_")
  migration_name = migration_name.lower()

  utc_timestamp = datetime.datetime.now(timezone.utc).replace(tzinfo=timezone.utc).timestamp()

  migration_file_name = "{}_{}.py".format(int(utc_timestamp), migration_name)

  with open(os.path.join("migrations", migration_file_name), "w") as f:
    f.write("""import psycopg2

def up(cur):
  pass

def down(cur):
  pass
""")
