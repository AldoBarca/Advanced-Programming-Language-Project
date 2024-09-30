#define _CRT_SECURE_NO_WARNINGS

#include "canbus_structures.h"
#include <cstdint>

/*
* La funzione trasforma un pacchetto canbus in una stringa di caratteri json
* I parametri della funzione sono:
*   array di caratteri per la stringa generata,
*   struttura contenente pacchetto canbus + identificativo veicolo
*   dimensione dell'array di caratteri
* La funzione restituisce la quantità di caratteri scritti sull'array
*/
uint16_t json_element(char* buffer, PacketsJsonList item,static const uint16_t DIM) {
    uint16_t written = 0;
    written = snprintf(buffer, DIM, "{\"vehicle_id\":\"%X\",\"can_id\":\"%X\",\"payload\":[\"%X\",\"%X\",\"%X\",\"%X\",\"%X\",\"%X\",\"%X\",\"%X\"],\"timestamp\":\"%llX\"}",
        item.vehicleID,
        item.canPacket.can_id,
        item.canPacket.payload[0],
        item.canPacket.payload[1],
        item.canPacket.payload[2],
        item.canPacket.payload[3],
        item.canPacket.payload[4],
        item.canPacket.payload[5],
        item.canPacket.payload[6],
        item.canPacket.payload[7],
        item.canPacket.timestamp);
    return written;
}