#ifndef H_CHRONO
#include <chrono>
#define H_CHRONO
#endif

#ifndef H_STDLIB
#include <stdlib.h>
#define H_STDLIB
#endif

#ifndef H_TIME
#include <time.h>
#define H_TIME
#endif

#ifndef H_FSTREAM
#include <fstream>
#define H_FSTREAM
#endif

#ifndef H_CANBUS_STRUCTURES
#include "canbus_structures.h"
#define H_CANBUS_STRUCTURES
#endif

#ifndef H_CANBUS_CONSTANTS
#include "canbus_constants.h"
#define H_CANBUS_CONSTANTS
#endif

#ifndef H_TIMESTAMP
#include "timestamp_utils.h"
#define H_TIMESTAMP
#endif

class Vehicle {
public:
	Vehicle(uint16_t,VehiclePacketList*);

	//genera i pacchetti e li scrive nella queue interna
	//restituisce quanti ne ha generati
	uint16_t genRoutine();
	
	//scrive i pacchetti nella queue interna nella lista
	//che legge il gateway
	//restituisce quanti ne ha scritti
	uint16_t writeGateway();

	static const uint16_t QUEUE_DIMENSION = 200;

private:
	//struct costante che non cambia
	typedef struct {
		uint64_t bms;
		uint64_t engine;
		uint64_t shunt;
	}INTERVAlS;

	typedef struct {
		uint64_t bms;
		uint64_t engine;
		uint64_t shunt;
	}LAST_MILLIS;

	uint32_t battVoltage;
	uint32_t controllerVoltage;
	
	uint16_t vehicleID;
	
	INTERVAlS intervals;
	LAST_MILLIS last_millis;

	uint16_t queueIndex;
	Timestamp t;//oggetto per ottenere il timestamp attuale

	std::string logRoute = "";
	void genLogRoute();

	//coda di tipo LIFO = Last In First Out
	CanPacket queue[QUEUE_DIMENSION];
	VehiclePacketList* gatewayList;
	
	int16_t RdeltaBMS(int16_t, int16_t);
	uint16_t genBMS();
	uint16_t genENGINE();
	uint16_t genSHUNT();

	void addToQueue(uint16_t,uint8_t [],uint64_t);

	void writeToDisk();
};