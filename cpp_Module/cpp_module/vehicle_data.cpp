#include "vehicle_data.h"

Vehicle::Vehicle(uint16_t ID, VehiclePacketList* x) {
	vehicleID = ID;
    queueIndex = 0;
    gatewayList = x;
    Timestamp t();
    
    last_millis.bms = 0;
    last_millis.engine = 0;
    last_millis.shunt = 0;
    
    intervals.bms = 3000;
    intervals.engine = 1600;
    intervals.shunt = 2500;
	srand(time(NULL));
    genLogRoute(); //posizione per salvare i pacchetti in locale su un file csv
}

void Vehicle::genLogRoute() {
    //la cartella di destinazione dei log di tutti i veicoli sarà APL\cpp_module\cpp_module\logs
    //il nome del file è composto da: id_veicolo + timestamp di avvio thread + estensione file
    logRoute = "logs/" + std::to_string(vehicleID) + "_" + std::to_string(t.getTimestamp()) + ".csv";
}

//funzione che serve per far generare i pacchetti alla classe.
uint16_t Vehicle::genRoutine() {
    uint16_t newPacketCounter = 0;
    if (t.getTimestamp() - last_millis.bms > intervals.bms) {
        printf("%d: Generazione bms\n",Vehicle::vehicleID);
        newPacketCounter += genBMS();
        last_millis.bms = t.getTimestamp();
    }

    if (t.getTimestamp() - last_millis.engine > intervals.engine) {
        printf("%d: Generazione engine\n", Vehicle::vehicleID);
        newPacketCounter += genENGINE();
        last_millis.engine = t.getTimestamp();
    }

    if (t.getTimestamp() - last_millis.shunt > intervals.shunt) {
        printf("%d: Generazione shunt\n", Vehicle::vehicleID);
        newPacketCounter += genSHUNT();
        last_millis.shunt = t.getTimestamp();
    }
	
	return newPacketCounter;
}

void Vehicle::addToQueue(uint16_t id, uint8_t payload[], uint64_t timestamp) {
    //controlla se è possibile aggiungere un pacchetto nella queue locale prima di iniziare la procedura di inserimento
    if (Vehicle::queueIndex == 0) {

    }
    if (Vehicle::queueIndex + 1 < Vehicle::QUEUE_DIMENSION ) {
        //posso aggiungere il valore nella queue
        
        Vehicle::queue[Vehicle::queueIndex].can_id = id;
        for (uint8_t i = 0; i < 8; i++) {
            Vehicle::queue[Vehicle::queueIndex].payload[i] = payload[i];
        }
        Vehicle::queue[Vehicle::queueIndex].timestamp = timestamp;
        Vehicle::queueIndex++;
    }else {
        //non posso aggiungere il valore nella queue
        //queue piena
    }
}

/*
* La funzione lavora a perdita. Scrive sulla lista del gateway partendo
* dall'ultimo pacchetto che ha memorizzato nella coda interna (quella di veicolo).
* Se la lista del gateway si riempe, scartera' i messaggi piu' vecchi
*/
uint16_t Vehicle::writeGateway() {
    //prima di mandare i pacchetti al gateway, scrive tutto su disco
    writeToDisk();
    uint16_t counter = 0;
    if (queueIndex > 0) {
        int16_t i = queueIndex-1;
        gatewayList->listMutex.lock();

        //mi salvo l'index in locale per comodità di lavoro
        int16_t gatewayListIndex = gatewayList->index;
        for (; i >= 0; i--) {
            if (gatewayListIndex + 1 < PACKETLIST_DIMENSION) {
                gatewayList->packetList[gatewayListIndex] = queue[i];
                gatewayListIndex++;
                counter++;
            }
            else {
                //la lista del gateway è piena, esco dal ciclo
                i = -1;
            }
        }
        //aggiorno l'index e rilascio il mutex
        gatewayList->index = gatewayListIndex;
        gatewayList->listMutex.unlock();
        queueIndex = 0;
    }
    return counter;
}

void Vehicle::writeToDisk() {
    //tento di aprire il file per scrivere i dati
    std::ofstream logData;
    //il file viene aperto per scrivere (::out) e tutte le righe vengono inserite in fondo al file (::app)
    logData.open(logRoute, std::ofstream::out | std::ofstream::app);
    if (logData.is_open()) {
        //tentativo di apertura riuscito
        uint16_t i, j;
        for (i = 0; i < queueIndex; i++) {
            //i ; servono per indicare il separatore di colonna
            logData << std::to_string(queue[i].timestamp)
                << ";"
                << std::to_string(queue[i].can_id)
                << ";";
            for (j = 0; j < 8; j++) {
                logData << std::to_string(queue[i].payload[j]) << ";";
            }
            logData << std::endl;
        }
        logData.close();
    }
    else {
        //non sono riuscito a scrivere i dati nel log
    }

}

//area = dimensione dell'intervallo reale di numeri randomizzati
//center = punto centrale dell'intervallo reale di numeri randomizzati
int16_t Vehicle::RdeltaBMS(int16_t area, int16_t center) {
    return (rand() % area) - center;
}

uint16_t Vehicle::genBMS() {
    uint16_t voltage = 3000;
    uint32_t current_in  = 150000 + RdeltaBMS(4000,2000);
    uint32_t current_out = 600000 + RdeltaBMS(8000,4000);
    int16_t area = 200;
    int16_t center = area / 2;
    uint16_t counter = 0;
    uint64_t timestamp;
    uint16_t randomized;
    uint8_t payload[8];
    uint16_t ids[] = {
        COBID_BMS_DATA0,
        COBID_BMS_DATA1,
        COBID_BMS_DATA2,
        COBID_BMS_DATA3,
        COBID_BMS_DATA4,
        COBID_BMS_DATA5,
        COBID_BMS_DATA6,
        COBID_BMS_DATA7
    };
    
    //valore di tensione interno, necessario per la generazione degli altri pacchetti (serve per mantenere un minimo di correlazione dati tra i pacchetti)
    battVoltage = 0;

    //generazione dei pacchetti di tensione
    for (uint8_t j = 0; j < 8; j++) {
        for (uint8_t i = 0; i < 8; i += 2) {
            randomized = voltage + RdeltaBMS(area, center);
            battVoltage += randomized;
            payload[i] = randomized & 0xFF;
            payload[i + 1] = (uint8_t)((uint16_t)(randomized & 0xFF00) >> 8);
        }
        timestamp = t.getTimestamp();
        counter++;
        addToQueue(ids[j], payload, timestamp);
    }

    //quasi tutti i valori che vengono scritti sui pacchetti sono più grandi di 8 bit, quindi vengono fatte delle operazioni logiche per
    //"spezzare" i dati all'interno del payload

    //generazione del pacchetto di notifica
    payload[0] = (uint8_t)((current_out & 0xFF000000) >> 24);
    payload[1] = (uint8_t)((current_out & 0xFF0000) >> 16);
    payload[2] = (uint8_t)((current_out & 0xFF00) >> 8);
    payload[3] = (uint8_t)(current_out & 0xFF);
    payload[4] = (uint8_t)((current_in & 0xFF000000) >> 24);
    payload[5] = (uint8_t)((current_in & 0xFF0000) >> 16);
    payload[6] = (uint8_t)((current_in & 0xFF00) >> 8);
    payload[7] = (uint8_t)(current_in & 0xFF);
    timestamp = t.getTimestamp();
    counter++;
    addToQueue(COBID_BMS_NOTIFY, payload, timestamp);

    //generazione del pacchetto di operazione
    counter++;
    timestamp = t.getTimestamp();
    payload[0] = 0;
    payload[1] = 0;
    //under patent
    payload[2] = rand() % 0xFF;
    payload[3] = rand() % 0xFF;
    payload[4] = rand() % 0xFF;
    payload[5] = rand() % 0xFF;
    payload[6] = 0;
    payload[7] = 0;
    counter++;
    addToQueue(COBID_BMS_OPERATION, payload, timestamp);

    randomized = (38 + RdeltaBMS(10, 5)) * 128;
    payload[0] = (uint8_t)((current_in & 0xFF00) >> 8);
    payload[1] = (uint8_t)(current_in & 0xFF);
    randomized = (38 + RdeltaBMS(10, 5)) * 128;
    payload[2] = (uint8_t)((current_in & 0xFF00) >> 8);
    payload[3] = (uint8_t)(current_in & 0xFF);
    randomized = (38 + RdeltaBMS(10, 5)) * 128;
    payload[4] = (uint8_t)((current_in & 0xFF00) >> 8);
    payload[5] = (uint8_t)(current_in & 0xFF);
    payload[6] = 0;
    payload[7] = 0;
    timestamp = t.getTimestamp();
    counter++;
    addToQueue(COBID_BMS_DATA8, payload, timestamp);

    return counter;
}

uint16_t Vehicle::genENGINE() {
    uint64_t timestamp;
    uint16_t randomized;
    uint8_t payload[8];
    uint16_t counter = 0;

    //motor rpm
    randomized = 3200 + RdeltaBMS(700, 100);
    payload[0] = (uint8_t)((randomized & 0xFF00) >> 8);
    payload[1] = (uint8_t)(randomized & 0xFF);

    //motor current
    randomized = 1000 + RdeltaBMS(1000, 320);
    payload[2] = (uint8_t)((randomized & 0xFF00) >> 8);
    payload[3] = (uint8_t)(randomized & 0xFF);

    //controller voltage
    controllerVoltage = battVoltage + RdeltaBMS(400, 200);
    randomized = (uint16_t)(controllerVoltage/10);
    payload[4] = (uint8_t)((randomized & 0xFF00) >> 8);
    payload[5] = (uint8_t)(randomized & 0xFF);
    payload[6] = 0;
    payload[7] = 0;

    timestamp = t.getTimestamp();
    counter++;
    addToQueue(COBID_ENGINE0, payload, timestamp);
    
    timestamp = t.getTimestamp();
    //input throttle
    payload[0] = 160 + RdeltaBMS(70, 10);
    //controller temperature
    //valore nel payload - 40 = temperatura in Celsius, con 1 bit = 1 Celsius
    payload[1] = 70 + RdeltaBMS(20, 10);
    //controller motore
    //valore nel payload - 30 = temperatura in Celsius, con 1 bit = 1 Celsius
    payload[2] = 80 + RdeltaBMS(20, 10);
    payload[3] = 0;
    payload[4] = 0;
    payload[5] = 0;
    payload[6] = 0;
    payload[7] = 0;

    addToQueue(COBID_ENGINE1, payload, timestamp);
    counter++;

    return counter;
}

uint16_t Vehicle::genSHUNT() {
    uint16_t counter = 0;
    uint64_t timestamp;
    uint8_t payload[8];
    uint32_t service;
    int32_t current;
    int32_t power;
    int32_t temperature = 500 + RdeltaBMS(800, 200);

    current = 30000 + RdeltaBMS(30000, 15000) - RdeltaBMS(30000, 15000);
    power = current * battVoltage;

    payload[0] = 0;
    payload[1] = 0;
    payload[2] = (uint8_t)((current & 0xFF000000) >> 24);
    payload[3] = (uint8_t)((current & 0xFF0000) >> 16);
    payload[4] = (uint8_t)((current & 0xFF00) >> 8);
    payload[5] = (uint8_t)(current & 0xFF);
    payload[6] = 0;
    payload[7] = 0;
    timestamp = t.getTimestamp();
    counter++;
    addToQueue(COBID_IVT_CURRENT, payload, timestamp);

    payload[0] = 0;
    payload[1] = 0;
    payload[2] = (uint8_t)((battVoltage & 0xFF000000) >> 24);
    payload[3] = (uint8_t)((battVoltage & 0xFF0000) >> 16);
    payload[4] = (uint8_t)((battVoltage & 0xFF00) >> 8);
    payload[5] = (uint8_t)(battVoltage & 0xFF);
    payload[6] = 0;
    payload[7] = 0;
    timestamp = t.getTimestamp();
    counter++;
    addToQueue(COBID_IVT_VOLTAGE1, payload, timestamp);

    service = 12000 + RdeltaBMS(100, 50);
    payload[0] = 0;
    payload[1] = 0;
    payload[2] = (uint8_t)((service & 0xFF000000) >> 24);
    payload[3] = (uint8_t)((service & 0xFF0000) >> 16);
    payload[4] = (uint8_t)((service & 0xFF00) >> 8);
    payload[5] = (uint8_t)(service & 0xFF);
    payload[6] = 0;
    payload[7] = 0;
    timestamp = t.getTimestamp();
    counter++;
    addToQueue(COBID_IVT_VOLTAGE2, payload, timestamp);

    payload[0] = 0;
    payload[1] = 0;
    payload[2] = (uint8_t)((controllerVoltage & 0xFF000000) >> 24);
    payload[3] = (uint8_t)((controllerVoltage & 0xFF0000) >> 16);
    payload[4] = (uint8_t)((controllerVoltage & 0xFF00) >> 8);
    payload[5] = (uint8_t)(controllerVoltage & 0xFF);
    payload[6] = 0;
    payload[7] = 0;
    timestamp = t.getTimestamp();
    counter++;
    addToQueue(COBID_IVT_VOLTAGE3, payload, timestamp);

    payload[0] = 0;
    payload[1] = 0;
    payload[2] = (uint8_t)((temperature & 0xFF000000) >> 24);
    payload[3] = (uint8_t)((temperature & 0xFF0000) >> 16);
    payload[4] = (uint8_t)((temperature & 0xFF00) >> 8);
    payload[5] = (uint8_t)(temperature & 0xFF);
    payload[6] = 0;
    payload[7] = 0;
    timestamp = t.getTimestamp();
    counter++;
    addToQueue(COBID_IVT_TEMP, payload, timestamp);

    payload[0] = 0;
    payload[1] = 0;
    payload[2] = (uint8_t)((power & 0xFF000000) >> 24);
    payload[3] = (uint8_t)((power & 0xFF0000) >> 16);
    payload[4] = (uint8_t)((power & 0xFF00) >> 8);
    payload[5] = (uint8_t)(power & 0xFF);
    payload[6] = 0;
    payload[7] = 0;
    timestamp = t.getTimestamp();
    counter++;
    addToQueue(COBID_IVT_POWER, payload, timestamp);

    return counter;
}