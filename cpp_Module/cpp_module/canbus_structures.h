#include <mutex>

//questa è la dimensione del buffer interno che utilizza ogni singolo veicolo
#define PACKETLIST_DIMENSION 300

//qui viene definito il numero e l'identificativo dei veicoli che genereranno pacchetti
#define VEHICLE_NUMBER 6
static const uint16_t VEHICLES_ID[] = { 46,16,14,44,7,64 };

//struttura che identifica la forma di un pacchetto canbus
typedef struct {
	uint16_t can_id;    //identificativo del pacchetto canbus
	uint8_t payload[8]; //payload dati del pacchetto (può contenere tensioni, correnti, rpm.....)
	uint64_t timestamp; //quando è stato generato il pacchetto
}CanPacket;

//struttura che identifica la forma della coda pacchetti interna di un veicolo
typedef struct {
	CanPacket packetList [PACKETLIST_DIMENSION]; //buffer utilizzato per la generazione dei pacchetti canbus
	uint16_t index;  //serve per tenere traccia dell'ultimo pacchetto che è stato memorizzato nel buffer
	uint16_t vehicleID; //contiene l'identificativo del veicolo che sta producendo i pacchetti
	std::mutex listMutex; //mutex del buffer dei pacchetti canbus
}VehiclePacketList;

/* 
* Struttura che identifica la forma del buffer per json del il gateway.
* Viene creato un array di tipo PacketsJsonList, in modo che la procedura di
* creazione del pacchetto per socket possa lavorare con più dati alla volta
* in una singola chiamata
*/
typedef struct {
	CanPacket canPacket;
	uint16_t vehicleID;
}PacketsJsonList;
