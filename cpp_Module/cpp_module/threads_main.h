/*
* Le due funzioni implementate qui sotto descrivono il comportamento ciclico dei thread
* dei veicoli e del gateway
* Le print vengono usate come una sorta di "buongiorno" da parte dei thread
*/
#include "gateway_data.h"
#include "vehicle_data.h"

//il thread riceve un intero corrispondente all'identificativo del veicolo e
//l'indirizzo di memoria del buffer da usare
void vehicle_run(uint16_t vehicleID, VehiclePacketList* gatewayList) {
	//setting parametri
	printf("veicolo %d: hello!\n", vehicleID);

	//&(*gatewayList) => al costruttore viene passato l'indirizzo di memoria del buffer
	//l'operatore puntatore "*" punta all'array nella parte di memoria del gateway, mentre l'operatore reference "&"
	//ottiene l'indirizzo di memoria del buffer che l'operatore "*" ha aperto
	//In questo modo il costruttore sta puntando alla zona di memoria del gateway, corrispondente alla posizione dell'array
	Vehicle vehicle(vehicleID, &(*gatewayList));
	while (true) {
		//generazione dei pacchetti canbus
		vehicle.genRoutine();
		//scrittura dei pacchetti generati sul buffer del gateway
		vehicle.writeGateway();
	}
}

void gateway_run() {
	printf("gateway: hello!\n");
	GatewayLists gatewayLists;
	printf("gateway: creazione veicoli\n");
	std::thread threads[VEHICLE_NUMBER];
	for (uint8_t i = 0; i < VEHICLE_NUMBER; i++) {
		//al thread viene passato l'identificativo del veicolo e il buffer che deve utilizzare per
		//mandare i pacchetti al gateway
		//gli viene passato l'indirizzo di memoria dell'array che deve usare
		threads[i] = std::thread(vehicle_run, 
			gatewayLists.vehiclepacketlist[i].vehicleID,
			&(gatewayLists.vehiclepacketlist[i]));
	}

	while (true) {
		gatewayLists.readRoutine();
		gatewayLists.writeToSocket();
	}
}