import queue
import multiprocessing
from datetime import datetime
from lib import parametri_canbus as pc
from lib import parametri_parallelismo


def bmsEq(pacchetto,start,vehicle_id):
        returnValues=[]
        for i in range(0,8):
            returnValues.append([start,pacchetto.payload[i] & 0b00000001,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
            start = start + 1
            returnValues.append([start,(pacchetto.payload[i] & 0b00000010)>>1,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
            start = start + 1
            returnValues.append([start,(pacchetto.payload[i] & 0b00000100)>>2,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
            start = start + 1
            returnValues.append([start,(pacchetto.payload[i] & 0b00001000)>>3,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
            start = start + 1
            returnValues.append([start,(pacchetto.payload[i] & 0b00010000)>>4,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
            start = start + 1
            returnValues.append([start,(pacchetto.payload[i] & 0b00100000)>>5,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
            start = start + 1
            returnValues.append([start,(pacchetto.payload[i] & 0b01000000)>>6,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
            start = start + 1
            returnValues.append([start,(pacchetto.payload[i] & 0b10000000)>>7,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
            start = start + 1
        return returnValues

def bmsVoltage(pacchetto,start,vehicle_id):
        returnValues = []
        returnValues.append([start,pacchetto.payload[0]+(pacchetto.payload[1]<<8),datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
        returnValues.append([start+1,pacchetto.payload[2]+(pacchetto.payload[3]<<8),datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
        returnValues.append([start+2,pacchetto.payload[4]+(pacchetto.payload[5]<<8),datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
        returnValues.append([start+3,pacchetto.payload[6]+(pacchetto.payload[7]<<8),datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
        return returnValues

class Elaboratore:
    class CanPack(object):
        __slots__ = ('can_id', 'payload','timestamp','vehicle_id')

    def __init__(self):
        pass

    def elabora(self,lock_grezzo,lista_grezza,lock_result,lista_result):
        j=0
        y=0
        while True:
            decoded = self.CanPack()
            
            while(not lock_grezzo[j].acquire(False)):
                if(j>=parametri_parallelismo.numero_liste_grezze-1):
                    j=0
                else:
                    j=j+1
                
            if(lista_grezza[j] is not None):
                if(len(lista_grezza[j])!= 0):
                    elemento = lista_grezza[j].pop()
                    lock_grezzo[j].release()

                    decoded.can_id=int(elemento["can_id"],16)
                    decoded.payload = []
                    decoded.payload.append(int(elemento["payload"][0],16))
                    decoded.payload.append(int(elemento["payload"][1],16))
                    decoded.payload.append(int(elemento["payload"][2],16))
                    decoded.payload.append(int(elemento["payload"][3],16))
                    decoded.payload.append(int(elemento["payload"][4],16))
                    decoded.payload.append(int(elemento["payload"][5],16))
                    decoded.payload.append(int(elemento["payload"][6],16))
                    decoded.payload.append(int(elemento["payload"][7],16))
                    decoded.timestamp = int(elemento["timestamp"],16)
                    decoded.vehicle_id = int(elemento["vehicle_id"],16)
                    dato_elaborato=self.legenda(decoded)
                    if(dato_elaborato != None):
                        while(not lock_result[y].acquire(False)):
                            if(y>=parametri_parallelismo.numero_liste_result-1):
                                y=0
                            else:
                                y=y+1
                        
                        for i in range(0,len(dato_elaborato)):
                            lista_result[y].append(dato_elaborato[i])
                        lock_result[y].release()
                else:
                    lock_grezzo[j].release()
            else:
                lock_grezzo[j].release()

            #le voci di sotto servono solamente a ruotare tra le liste disponibili
            #in questo modo il thread non va a scrivere o leggere sempre dalla stessa lista
            if(j>=parametri_parallelismo.numero_liste_grezze-1):
                j=0
            else:
                j=j+1

            if(y>=parametri_parallelismo.numero_liste_result-1):
                y=0
            else:
                y=y+1


    def legenda(self,pacchetto):
        returnValues = []
        vehicle_id = pacchetto.vehicle_id
        match pacchetto.can_id:
            case pc.COBID_BMS_OPERATION:
                returnValues.extend(bmsEq(pacchetto,1,vehicle_id))

            case pc.COBID_BMS_DATA0:
                returnValues.extend(bmsVoltage(pacchetto,33,vehicle_id))

            case pc.COBID_BMS_DATA1:
                returnValues.extend(bmsVoltage(pacchetto,37,vehicle_id))
            
            case pc.COBID_BMS_DATA2:
                returnValues.extend(bmsVoltage(pacchetto,41,vehicle_id))

            case pc.COBID_BMS_DATA3:
                returnValues.extend(bmsVoltage(pacchetto,45,vehicle_id))

            case pc.COBID_BMS_DATA4:
                returnValues.extend(bmsVoltage(pacchetto,49,vehicle_id))

            case pc.COBID_BMS_DATA5:
                returnValues.extend(bmsVoltage(pacchetto,53,vehicle_id))

            case pc.COBID_BMS_DATA6:
                returnValues.extend(bmsVoltage(pacchetto,57,vehicle_id))

            case pc.COBID_BMS_DATA7:
                returnValues.extend(bmsVoltage(pacchetto,61,vehicle_id))

            case pc.COBID_BMS_DATA8:
                returnValues.append([65,(pacchetto.payload[0]+(pacchetto.payload[1]<<8))/1280.0,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
                returnValues.append([66,(pacchetto.payload[2]+(pacchetto.payload[3]<<8))/1280.0,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
                returnValues.append([67,(pacchetto.payload[4]+(pacchetto.payload[5]<<8))/1280.0,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])

            case pc.COBID_BMS_NOTIFY:
                value = (pacchetto.payload[0]<<24) + (pacchetto.payload[1]<<16) + (pacchetto.payload[2]<<8) + pacchetto.payload[3]
                returnValues.append([68,value,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
                value = (pacchetto.payload[4]<<24) + (pacchetto.payload[5]<<16) + (pacchetto.payload[6]<<8) + pacchetto.payload[7]
                returnValues.append([69,value,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])

            case pc.COBID_ENGINE0:
                returnValues.append([70,pacchetto.payload[0]+(pacchetto.payload[1]<<8),datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
                returnValues.append([71,(pacchetto.payload[0]+(pacchetto.payload[1]<<8))/10.0,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
                returnValues.append([72,(pacchetto.payload[2]+(pacchetto.payload[3]<<8))/10.0,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])

            case pc.COBID_ENGINE1:
                returnValues.append([73,pacchetto.payload[0],datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
                returnValues.append([74,pacchetto.payload[1]-40,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
                returnValues.append([75,pacchetto.payload[2]-30,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])

            case pc.COBID_IVT_CURRENT:
                value = (pacchetto.payload[2]<<24) + (pacchetto.payload[3]<<16) + (pacchetto.payload[4]<<8) + pacchetto.payload[5]
                returnValues.append([76,value,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])

            case pc.COBID_IVT_VOLTAGE1:
                value = (pacchetto.payload[2]<<24) + (pacchetto.payload[3]<<16) + (pacchetto.payload[4]<<8) + pacchetto.payload[5]
                returnValues.append([77,value,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])

            case pc.COBID_IVT_VOLTAGE2:
                value = (pacchetto.payload[2]<<24) + (pacchetto.payload[3]<<16) + (pacchetto.payload[4]<<8) + pacchetto.payload[5]
                returnValues.append([78,value,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])

            case pc.COBID_IVT_VOLTAGE3:
                value = (pacchetto.payload[2]<<24) + (pacchetto.payload[3]<<16) + (pacchetto.payload[4]<<8) + pacchetto.payload[5]
                returnValues.append([79,value,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])

            case pc.COBID_IVT_TEMP:
                value = (pacchetto.payload[2]<<24) + (pacchetto.payload[3]<<16) + (pacchetto.payload[4]<<8) + pacchetto.payload[5]
                returnValues.append([80,value,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])

            case pc.COBID_IVT_POWER:
                value = (pacchetto.payload[2]<<24) + (pacchetto.payload[3]<<16) + (pacchetto.payload[4]<<8) + pacchetto.payload[5]
                returnValues.append([81,value,datetime.fromtimestamp(pacchetto.timestamp/1000.0),vehicle_id])
            case _:
                returnValues = None

        return returnValues