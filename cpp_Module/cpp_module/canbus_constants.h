/*
* Ogni pacchetto canbus ha un identificativo univoco.
* Questi sono riportati in gruppi a seconda dell'area di appartenenza
* 
* "COBID" è un prefisso per indicare che la costante è un identificativo di pacchetto canbus,
* successivamente è riportato il nome del pacchetto
* Nota: tutti gli id sono riportati in base esadecimale
*/

#define COBID_BMS_OPERATION 0x150
#define COBID_BMS_DATA0		0x151
#define COBID_BMS_DATA1		0x152
#define COBID_BMS_DATA2		0x153
#define COBID_BMS_DATA3		0x154
#define COBID_BMS_DATA4		0x155
#define COBID_BMS_DATA5		0x156
#define COBID_BMS_DATA6		0x157
#define COBID_BMS_DATA7		0x158
#define COBID_BMS_DATA8		0x159
#define COBID_BMS_NOTIFY	0x15A

#define COBID_ENGINE0 0x100
#define COBID_ENGINE1 0x102

#define COBID_IVT_CURRENT		 0x521
#define COBID_IVT_VOLTAGE1		 0x522
#define COBID_IVT_VOLTAGE2		 0x523
#define COBID_IVT_VOLTAGE3		 0x524
#define COBID_IVT_TEMP			 0x525
#define COBID_IVT_POWER			 0x526