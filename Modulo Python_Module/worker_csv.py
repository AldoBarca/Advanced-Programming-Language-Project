import os
import csv
class WorkerCsv:
    
    def aggiungi_a_nuovo_csv(self,percorso_file,dati):
        mode = 'w'
        with open(percorso_file, mode, newline='') as file_csv:
            writer = csv.writer(file_csv,delimiter=';')
            writer.writerow(["cod_legenda; ", "valore; ", "time; ", "cod_veicolo; "])
            print(dati)
            writer.writerow(dati)


    def aggiungi_a_csv_esistente(self,percorso_file,dati):
        mode = 'a'
        with open(percorso_file, mode, newline='') as file_csv:
            writer = csv.writer(file_csv,delimiter=';')
            print(dati)
            writer.writerow(dati)


    def aggiungi_dati(self, dati_da_aggiungere):
       
            directory_file_csv = os.path.join(os.path.dirname(__file__),"PermanentStorage")
            if isinstance(dati_da_aggiungere, (list, tuple)):
                for i in range(0,len(dati_da_aggiungere)):
                    file_path=directory_file_csv+"\\logVeicolo_"+str(dati_da_aggiungere[i][3])+str(".csv")
                    if os.path.exists(file_path):
                        self.aggiungi_a_csv_esistente(file_path,dati_da_aggiungere[i])
                    else:
                        self.aggiungi_a_nuovo_csv(file_path,dati_da_aggiungere[i])
        


        