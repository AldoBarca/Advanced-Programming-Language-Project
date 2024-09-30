from worker_sql import WorkerSql
from worker_csv import WorkerCsv
from lib import parametri_parallelismo

class ConvogliatoreRisultati():

    def convoglia_dati_in_storage(self,lock_result,lista_result):
        worker_csv=WorkerCsv()
        worker_sql=WorkerSql()

        y=0
        while True:
            while(not lock_result[y].acquire(False)):
                if(y>=parametri_parallelismo.numero_liste_result-1):
                    y=0
                else:
                    y=y+1
            if(lista_result[y] is not None):
                len_result = len(lista_result[y])
                if len_result>0:
                    dati=[]
                    for i in range(0,len_result):
                        dati.append(lista_result[y].pop())
                    lock_result[y].release()
                    worker_csv.aggiungi_dati(dati)
                    worker_sql.inserimento_db(dati)

                else:
                    lock_result[y].release()
                