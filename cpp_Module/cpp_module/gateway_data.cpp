//queste due definizioni di "....NO_WARNINGS" stanno qui perché microsoft
//vuole che vengano usate le loro definizioni di funzioni come "printf", "scanf" ecc
//in questo modo si disabilitano gli errori quando si utilizzano e la compilazione va a buon fine
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable:4996) 

#include "gateway_data.h"

//costruttore della classe di lavoro del gateway
GatewayLists::GatewayLists() {

	//inizializzazione dei parametri dei veicoli
	for (uint8_t i = 0; i < VEHICLE_NUMBER; i++) {
		vehiclepacketlist[i].index = 0;
		vehiclepacketlist[i].vehicleID = VEHICLES_ID[i];
	}
	queueIndex = 0;

	//setup address structure
	memset((char*)&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_port = htons(PY_PORT);
	server.sin_addr.S_un.S_addr = inet_addr(PY_SERVER);

	//creazione della socket
	sockGen();
	//scrittura stato della socket
	socketStatus = sockStatus();
}

//questa funzione viene chiamata per provare a leggere dei dati dalle code dei veicoli
void GatewayLists::readRoutine() {
	for (uint8_t i = 0; i < VEHICLE_NUMBER; i++) {
		//viene usato il try_lock e non il lock perché il gateway deve leggere i buffer
		//di tutti i veicoli presenti. Se non riesce a fare il lock di un buffer va avanti, alla
		//prossima volta che la funzione viene chiamata magari riuscirà a fare il lock
		if (vehiclepacketlist[i].listMutex.try_lock()) {
			readFromList(i);
			vehiclepacketlist[i].listMutex.unlock();
		}
	}
}

//questa viene chiamata per creare (o ri creare) la socket per inviare i dati
//al modulo python
void GatewayLists::sockGen() {
	if (WSAStartup(MAKEWORD(2, 2), &ws) != 0) {
		//fallimento nell'avvio del servizio di creazione socket
		printf("Failed. Error Code: %d\n", WSAGetLastError());
		socketStatus = false;
		return;
	}else if ((client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == SOCKET_ERROR) {
		//fallimento di creazione della socket
		WSACleanup();
		printf("socket() failed with error code: %d\n", WSAGetLastError());
		socketStatus = false;
		return;
	}
	//socket creata con successo
	socketStatus = true;
}

//ritorna lo stato della socket
//true = la socket è operativa
//false = la socket è down
bool GatewayLists::sockStatus() const {
	return socketStatus;
}

//creazione della stringa json da mandare al server python
void GatewayLists::writeToSocket() {

	//la lettura dei pacchetti avviene in modo FIFO
	if (queueIndex > 0) {
		static const uint16_t SERIALIZED_SIZE = 4096; //dimensione della stringa che il gateway invia al modulo python
		static const uint16_t BUFFER_SIZE = 130; //dimensione della stringa per la conversione di un singolo pacchetto canbus
		char buffer[BUFFER_SIZE], serialized[SERIALIZED_SIZE];
		uint16_t start; //utilizzato per tenere traccia della posizione per scrivere su serialized[]

		//qui viene fatta la conversione in stringa di un pacchetto canbus. Il valore di ritorno è il numero di
		//caratteri scritti
		queueIndex--;
		uint16_t written = json_element(buffer, queueToJson[queueIndex], BUFFER_SIZE);

		snprintf(serialized, SERIALIZED_SIZE, "{\"data\":[");//questo testo viene chiamato una sola volta. Serve per creare correttamente una stringa json
		snprintf(serialized+9, SERIALIZED_SIZE, buffer);//scrittura dei dati convertiti
		start = written + 9; //nuovo inizio della stringa. "9" è la dimensione del testo  ->  {\"data\":[

		for (int16_t i = queueIndex-1; i > 0; i--) {
			/*
			* il codice scritto sotto serve per evitare di mandare più di X oggetti 
			* json alla volta. Una volta raggiunto quel numero, viene verificato che 
			* all'iterazione successiva non possa concludere il ciclo
			* Qualora il ciclo non si concluda, trasforma in json un altro campo
			* in modo che all'iterazione successiva la funzione possa mettere tranquillamente la virgola
			* (altrimenti il json non è formattatto correttamente
			*/
			/* Prende la posizione di partenza da cui scrivere. Se sommando 125 è ancora dentro l'array
			* inserisce un nuovo elemento altrimenti abortisce
			* 120 è la dimensione massima che può avere una stringa serializzata json
			* 1 è una virgola da aggiungere per indicare un altro elemento dell'array
			* 2 sono i caratteri di terminazione del json
			* 125 per stare sicuri
			*/
			if (start + 125 < 4096) {
				queueIndex--;
				written = json_element(buffer, queueToJson[i], BUFFER_SIZE);
				snprintf(serialized + start, SERIALIZED_SIZE, ",%s", buffer);
				start += 1 + written;
			}
			else {
				snprintf(serialized + start, SERIALIZED_SIZE, "]}");
				socketSend(serialized,start+2);
				return;
			}
		}
		snprintf(serialized + start, SERIALIZED_SIZE, "]}");
		socketSend(serialized, start + 2);
		return;

	}
}

//procedura per l'invio dei dati tramite socket
void GatewayLists::socketSend(char* message, uint16_t len) {
	
	//verifica status socket
	if (!sockStatus()) {
		//socket non utilizzabile, tentativo ripristino
		sockGen();
		if (!sockStatus()) {
			//tentativo ripristino fallito, annulla invio
			return;
		}
	}
	
	//invio di dati con la socket
	if (sendto(client_socket, &(*message), len, 0, (sockaddr*)&server, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		//tentativo di invio fallito
		printf("sendto() failed with error code: %d", WSAGetLastError());
	}
	//invio riuscito
}

/*
* Funzione utilizzata dal gateway per leggere i pacchetti bufferizzati nelle code dei veicoli
* 
* Non sono presenti tentativi di mutex lock perché questa funzione viene chiamata in "readRoutine()"
* qualora si riesce a eseguire il lock su uno dei buffer per i pacchetti dei veicoli
*/
void GatewayLists::readFromList(uint8_t vehicleIndex) {
	PacketsJsonList item;
	item.vehicleID = vehiclepacketlist[vehicleIndex].vehicleID;

	//la lettura dei pacchetti avviene secondo una logica LIFO
	//questo perché si preferisce leggere sempre i pacchetti più recenti e scartare gli eventuali pacchetti più vecchi
	if (vehiclepacketlist[vehicleIndex].index>0) {
		vehiclepacketlist[vehicleIndex].index--;
		for (int16_t i = vehiclepacketlist[vehicleIndex].index; i > 0; i--) {
			item.canPacket = vehiclepacketlist[vehicleIndex].packetList[i];

			//In questo punto può accadere che i pacchetti vengano persi
			//se il buffer interno di gateway (queueToJson[]) è pieno,
			//il gateway svuota la coda dei pacchetti dei veicoli.
			//Non c'è perdita di dati dal lato veicolo perché ogni thread ha
			//già memorizzato tutti i dati necessari su un file csv
			if (addToQueueG(item)) {
				//c'era posto nel buffer, quindi il pacchetto è stato memorizzato
				vehiclepacketlist[vehicleIndex].index--;
			}
			else {
				//non c'era più posto nel buffer, il pacchetto non è stato memorizzato e
				//i successivi non possono essere memorizzati
				vehiclepacketlist[vehicleIndex].index = 0;
				i = vehiclepacketlist[vehicleIndex].index;
			}

		}
	}
	
}

//scrittura di un item nel buffer interno per trasformare i pacchetti in json
bool GatewayLists::addToQueueG(PacketsJsonList item) {
	if (queueIndex + 1 < QUEUE_DIMENSION) {
		queueToJson[queueIndex] = item;
		queueIndex++;
		return true;
	}
	else {
		return false;
	}
}