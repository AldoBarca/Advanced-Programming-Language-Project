#include "timestamp_utils.h"

//la funzione utilizza il clock interno di sistema per generare un timestamp con risoluzione in ms
uint64_t Timestamp::getTimestamp() {
    //acquisizione del clock = ritorna un time point corrispondente all'istante di tempo sulla macchina host quando è stata
    //chiamata la funzione
    auto now = std::chrono::system_clock::now();

    //converte un time point in un altro time point con lo stesso clock, ma esprimendolo in milli secondi
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto epoch = now_ms.time_since_epoch(); //porta il time point in formato "quanti millisecondi sono passati da epoch"
    auto value = std::chrono::duration_cast<std::chrono::milliseconds>(epoch); //conversione del clock in ms con offset "epoch"
    uint64_t returnValue = value.count(); //conversione del clock in ms in uint64_t
    return returnValue;
}
//auto serve per definire automaticamente il tipo della variabile in funzione del tipo che serve per inizializzare
//la variabile che viene utilizzata per inizializzarla
//ES: auto now = std::chrono::system_clock::now() ==> la variabile now sarà del tipo che restituisce "now()"