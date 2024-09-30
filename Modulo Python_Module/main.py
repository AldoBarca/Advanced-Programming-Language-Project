import multiprocessing
import time
import pickle
from elaboratore import Elaboratore
from server_veicoli import ServerVeicoli
from convogliatore_results import ConvogliatoreRisultati
from lib import parametri_parallelismo

def run():
    elaboratori=[]
    processi=[]

    manager = multiprocessing.Manager()

    lock_grezzo = [manager.Lock()]*parametri_parallelismo.numero_liste_grezze
    lista_grezza = [manager.list()]*parametri_parallelismo.numero_liste_grezze

    lock_result = [manager.Lock()]*parametri_parallelismo.numero_liste_result
    lista_result = [manager.list()]*parametri_parallelismo.numero_liste_result

    for i in range(0,parametri_parallelismo.numero_elaboratori):
        elaboratore=Elaboratore()
        elaboratori.append(elaboratore)

    server_veicoli=ServerVeicoli()
    processo_server_veicoli = multiprocessing.Process(target=server_veicoli.distribuisci_dati, args=(lock_grezzo,lista_grezza))
    processo_server_veicoli.start()

    convogliatore_risultati=ConvogliatoreRisultati()
    processo_convogliatore_risultati=multiprocessing.Process(target=convogliatore_risultati.convoglia_dati_in_storage,args=(lock_result,lista_result))
    processo_convogliatore_risultati.start()
    
    for i in range(0,parametri_parallelismo.numero_elaboratori):
        processo = multiprocessing.Process(target=elaboratori[i].elabora, args=(lock_grezzo,lista_grezza,lock_result,lista_result))
        processi.append(processo) 
        processo.start()

    while True:
        if processo_server_veicoli.is_alive() == False:
            processo_server_veicoli = multiprocessing.Process(target=server_veicoli.distribuisci_dati, args=(lock_grezzo,lista_grezza))
        
        if processo_convogliatore_risultati== False:
            processo_convogliatore_risultati=multiprocessing.Process(target=convogliatore_risultati.convoglia_dati_in_storage,args=(lock_result,lista_result))

        for process in processi:
            if process.is_alive() == False:
                processi.remove(process)
                processo = multiprocessing.Process(target=elaboratore.elabora, args=(lock_grezzo,lista_grezza,lock_result,lista_result))
                processi.append(processo)

if __name__ == "__main__":
    run()
