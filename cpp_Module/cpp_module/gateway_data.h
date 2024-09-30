#ifndef H_WINSOCK2
#include <winsock2.h>
#define H_WINSOCK2
#endif

#ifndef H_CANBUS_STRUCTURES
#include "canbus_structures.h"
#define H_CANBUS_STRUCTURES
#endif

#ifndef H_JSON_UTILS
#include "json_utils.h"
#define H_JSON_UTILS
#endif

#define PY_SERVER "127.0.0.1"
#define PY_PORT 12400

class GatewayLists {
public:
	GatewayLists();

	VehiclePacketList vehiclepacketlist[VEHICLE_NUMBER];
	static const uint16_t QUEUE_DIMENSION = 512;

	//funzione che inizializza le letture dalle liste condivise
	void readRoutine();

	void writeToSocket();

	bool sockStatus() const;

private:
	//legge dalla lista i-esima di un veicolo
	void readFromList(uint8_t);

	bool addToQueueG(PacketsJsonList);
	
	bool socketStatus;
	void sockGen();
	
	//queue per json

	//corrisponde alla posizione dove scrivere
	//ES: se non ci sono elementi nel vettore, queueIndex=0, indicando che si deve iniziare a salvare
	//i dati dalla posizione 0 in poi. Se queueIndex=5, indica che ci sono 5 elementi, rispettivamente nelle posizioni 0,1,2,3,4,
	//quindi si deve iniziare a scrivere dalla posizione 5
	int16_t queueIndex;
	PacketsJsonList queueToJson[QUEUE_DIMENSION];

	//socket data
	WSADATA ws;
	SOCKET client_socket;
	sockaddr_in server;

	void socketSend(char*, uint16_t);
};