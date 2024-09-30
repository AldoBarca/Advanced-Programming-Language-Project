import socket
import json
from lib import parametri_socket
from lib import parametri_parallelismo

class ServerVeicoli:
    def __init__(self):
        print("Server veicoli init")

    def distribuisci_dati(self,lock_grezzo,lista_grezza):
        serverSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        serverSocket.bind((parametri_socket.host, parametri_socket.port))
        print("Listening on %s:%s..." % (str(socket.gethostname()), str(parametri_socket.port)))

        j=0
        while True:
            message, address = serverSocket.recvfrom(4096)
            if(j>=parametri_parallelismo.numero_liste_grezze-1):
                j=0
            dati = json.loads(message)["data"]
            dati_len = len(dati)
            if dati_len>0:
                while(not lock_grezzo[j].acquire(False)):
                    j=j+1
                    if(j>=parametri_parallelismo.numero_liste_grezze-1):
                        j=0

                for i in range(0,dati_len):
                    lista_grezza[j].append(dati[i])
                lock_grezzo[j].release()