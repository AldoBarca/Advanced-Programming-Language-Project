import mysql.connector
from lib import sql_data

class WorkerSql:

    def __init__(self):
        self.db_connection=self.database_connection()
        self.cursore_db=self.database_cursor(self.db_connection)
     
    def database_connection(self):
        db_conn = mysql.connector.connect(
            host=sql_data.MYSQLDB_HOST,
            user=sql_data.MYSQLDB_USER,
            password=sql_data.MYSQLDB_PASSWORD,
            database=sql_data.MYSQLDB_DATABASE
        )
        return db_conn

    def close(self):
        self.cursore_db.close()
        self.db_connection.close()

    def database_cursor(self,db_conn):
        return db_conn.cursor()

    def inserimento_db(self,dato):
        try:
            self.cursore_db.clear_attributes()

            for i in range(0,len(dato)):
                query_sql="INSERT INTO pacchetto (cod_veicolo,cod_legenda,valore,timestamp) VALUES (%s,%s,%s,%s)"
                dati_sql=(dato[i][3],dato[i][0],dato[i][1],dato[i][2])
                
                self.cursore_db.execute(query_sql,dati_sql)
                
                self.db_connection.commit()
        except Exception as e:
            print(f"Si è verificato un errore generico: {e}")