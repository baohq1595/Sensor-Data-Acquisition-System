/*
 * package.h
 *
 *  Created on: Oct 23, 2016
 *      Author: BAOBAO
 */

#ifndef PACKAGE_H_
#define PACKAGE_H_

//Package max size
#define PACKAGE_MAX_SIZE  45
						  //13

#define SERVER 				"SERVER"

//Client types tags
#define HUB_SENSOR			"HUBSSR"
#define PC_CLIENT			"PCCLNT"

//Sensor types tags
#define LIGHT_SENSOR		"LIGHT"
#define TEMP_SENSOR			"TEMPA"
#define HUMIDITY			"HUMID"

//Sensor hub name tags
#define HUB01				"HUB01"

//Sensor ID tags
#define SENSOR_TEMP_1		"T001"
#define SENSOR_LIGHT_1		"L001"
#define SENSOR_HUMIDITY_1	"H001"

//Client types size
#define CLIENT_TYPE_SIZE  6

//Action types size
#define CONTROL_SIGNAL_SIZE  6

//Sensor types size
#define SENSOR_TYPE_SIZE  5

//Sensor hub name size
#define SENSORHUB_NAME_SIZE  5

//Sensor ID size
#define SENSOR_ID_SIZE  4

//Time type size
#define TIME_TYPE_SIZE  4

//Value type size
#define VALUE_TYPE_SIZE  8

//Action types tags
#define NOTIFY				"NOTIFY"
#define GET_DATA			"GETDAT"
#define HAND_SHAKING		"HANDSH"
#define UPDATE_DATA			"UPDATA"
#define SEND_DATA			"SENDAT"
#define READY				"READY_"
#define END_OF_DATA			"ENDDAT"

//Initiation connection - update data requirement
#define INIT				"START"
#define IN_PROGRESS			"PROGRESS"

struct Package {

	//Client name
	char clientName[CLIENT_TYPE_SIZE + 1];

	//control field
	char control[CONTROL_SIGNAL_SIZE + 1];

	//time field;
	int yy;			//4
	int mm;			//2
	int dd;			//2
	int hour;		//2
	int minute;		//2
	int second;		//2
					//10

	//Sensor info
	char SensorHubName[SENSORHUB_NAME_SIZE + 1];
	char SensorID[SENSOR_ID_SIZE + 1];
	char SensorType[SENSOR_TYPE_SIZE + 1];

	//sensor value
	int value;	//5
					//3
};

extern void PackageConst(struct Package *pack, char *clntName, char *ctrl,
						int yy, int mm, int dd, int hour, int minute, int second,
						char *ssName, char *ssID, char *ssType,
						int value);

extern void Serialize(struct Package *pack, char buffer[]);
extern void Deserialize(struct Package *pack, char buffer[]);
extern void printPackage(struct Package *pack);

void arraycopy(char *src, int srcPos, char *dst, int dstPos, int length);
int intToStr(int x, char str[], int d);
void reverse(char *str, int len);
float pow__(int base, int ex);
extern int getStrSize(char *str);
void ftobyte(float val, char *buffer);
void int2Char(int val, char *buffer);
int chartoint(unsigned char *buffer);

void itoa_sss(int i, char b[]);
int atoi_sss(char array[]);


#endif /* PACKAGE_H_ */
