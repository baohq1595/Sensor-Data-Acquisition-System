/*
 * package.c
 *
 *  Created on: Oct 23, 2016
 *      Author: BAOBAO
 */

#include "package.h"
#include <string.h>
#include <stdbool.h>

#include "utils/uartstdio.h"
#include <string.h>
#include <stdbool.h>
#include <stdint.h>


//void itoa(int i, char b[]);
//int atoi(char array[]);

void PackageConst(struct Package *pack, char *clntName, char *ctrl,
						int yy, int mm, int dd, int hour, int minute, int second,
						char *ssName, char *ssID, char *ssType,
						int value)
{
	memset(pack->clientName, 0, CLIENT_TYPE_SIZE + 1);
	memset(pack->control, 0, CONTROL_SIGNAL_SIZE + 1);
	memset(pack->SensorHubName, 0, SENSORHUB_NAME_SIZE + 1);
	memset(pack->SensorID, 0, SENSOR_ID_SIZE + 1);
	memset(pack->SensorType, 0, SENSOR_TYPE_SIZE + 1);
	arraycopy(clntName, 0, pack->clientName, 0, CLIENT_TYPE_SIZE);
	arraycopy(ctrl, 0, pack->control, 0, CONTROL_SIGNAL_SIZE);
	pack->yy = yy;
	pack->mm = mm;
	pack->dd = dd;
	pack->hour = hour;
	pack->minute = minute;
	pack->second = second;
	arraycopy(ssName, 0, pack->SensorHubName, 0, SENSORHUB_NAME_SIZE);
	arraycopy(ssID, 0, pack->SensorID, 0, SENSOR_ID_SIZE);
	arraycopy(ssType, 0, pack->SensorType, 0, SENSOR_TYPE_SIZE);
	pack->value = value;
}

void printPackage(struct Package *pack)
{
	UARTprintf("%s%d/%d/%d %d:%d:%d\n%s%s%s%s%s%s%s%s%s%s%s%s%d%s",
			"Time:\n\t", pack->yy, pack->mm, pack->dd, pack->hour, pack->minute, pack->second,
			"CLient Name:\n\t", pack->clientName, "\nControl:\n\t", pack->control,
			"\nSensorInfo:\n\t", "SensorHubName:", pack->SensorHubName, "\n\tSensorID: ", pack->SensorID, "\n\tSensorType: ", pack->SensorType,
			"\n\tSensor Value: ", pack->value, "\n");
}

void Serialize(struct Package *pack, char buffer[])
{
	char numBuffer[10];
	int i = 0;// strSize = 0;

	memset(numBuffer, 0, sizeof(numBuffer));

	//put CLIENT_TYPE_SIZE bytes of clientName field into buffer
	arraycopy(pack->clientName, 0, buffer, i, CLIENT_TYPE_SIZE);
	i = i + CLIENT_TYPE_SIZE;

	//put CONTROL_SIGNAL_SIZE bytes of control field into buffer
	arraycopy(pack->control, 0, buffer, i, CONTROL_SIGNAL_SIZE);
	i = i + CONTROL_SIGNAL_SIZE;

	//put TIME_TYPE_SIZE bytes of year field into buffer
	itoa_sss(pack->yy, numBuffer);
	arraycopy(numBuffer, 0, buffer, i, 4);
	i = i + 4;

	//put TIME_TYPE_SIZE bytes of month field into buffer
	memset(numBuffer, 0, sizeof(numBuffer));
	itoa_sss(pack->mm, numBuffer);
	arraycopy(numBuffer, 0, buffer, i, 2);
	i = i + 2;

	//put TIME_TYPE_SIZE bytes of day field into buffer
	memset(numBuffer, 0, sizeof(numBuffer));
	itoa_sss(pack->dd, numBuffer);
	arraycopy(numBuffer, 0, buffer, i, 2);
	i = i + 2;

	//put TIME_TYPE_SIZE bytes of hour field into buffer
	memset(numBuffer, 0, sizeof(numBuffer));
	itoa_sss(pack->hour, numBuffer);
	arraycopy(numBuffer, 0, buffer, i, 2);
	i = i + 2;

	//put TIME_TYPE_SIZE bytes of minute field into buffer
	memset(numBuffer, 0, sizeof(numBuffer));
	itoa_sss(pack->minute, numBuffer);
	arraycopy(numBuffer, 0, buffer, i, 2);
	i = i + 2;

	//put TIME_TYPE_SIZE bytes of second field into buffer
	memset(numBuffer, 0, sizeof(numBuffer));
	itoa_sss(pack->second, numBuffer);
	arraycopy(numBuffer, 0, buffer, i, 2);
	i = i + 2;

	//put SENSORHUB_NAME_SIZE bytes of SensorName field into buffer
	arraycopy(pack->SensorHubName, 0, buffer, i, SENSORHUB_NAME_SIZE);
	i = i + SENSORHUB_NAME_SIZE;

	//put SENSORID_SIZE bytes of SensorID field into buffer
	arraycopy(pack->SensorID, 0, buffer, i, SENSOR_ID_SIZE);
	i = i + SENSOR_ID_SIZE;

	//put SENSOR_TYPE_SIZE bytes of SensorType field into buffer
	arraycopy(pack->SensorType, 0, buffer, i, SENSOR_TYPE_SIZE);
	i = i + SENSOR_TYPE_SIZE;

	//put 8 bytes of float value to buffer
	memset(numBuffer, 0, sizeof(numBuffer));

	if (pack->value >= 10000)
	{
		itoa_sss(pack->value, numBuffer);
	}
	else if (pack->value < 10000 && pack->value >= 1000)
	{
		numBuffer[0] = '0';
		itoa_sss(pack->value, &(numBuffer[1]));
	}
	else if (pack->value < 1000 && pack->value >= 100)
	{
		numBuffer[0] = '0';
		numBuffer[1] = '0';
		itoa_sss(pack->value, &(numBuffer[2]));
	}
	else
	{
		numBuffer[0] = '0';
		numBuffer[1] = '0';
		numBuffer[2] = '0';
		itoa_sss(pack->value, &(numBuffer[3]));
	}
	arraycopy(numBuffer, 0, buffer, i, 5);
}

void Deserialize(struct Package *pack, char buffer[])
{
	int i = 0;
	char numBuffer[8];
//	char strBuffer[10];

	arraycopy(buffer, i, pack->clientName, 0, CLIENT_TYPE_SIZE);
	i = i + CLIENT_TYPE_SIZE;

	arraycopy(buffer, i, pack->control, 0, CONTROL_SIGNAL_SIZE);
	i = i + CONTROL_SIGNAL_SIZE;

	memset(numBuffer, 0, sizeof(numBuffer));
	arraycopy(buffer, i, numBuffer, 0, 4);
	pack->yy = atoi_sss(numBuffer);
	i = i + 4;

	memset(numBuffer, 0, sizeof(numBuffer));
	arraycopy(buffer, i, numBuffer, 0, 2);
	pack->mm = atoi_sss(numBuffer);
	i = i + 2;

	memset(numBuffer, 0, sizeof(numBuffer));
	arraycopy(buffer, i, numBuffer, 0, 2);
	pack->dd = atoi_sss(numBuffer);
	i = i + 2;

	memset(numBuffer, 0, sizeof(numBuffer));
	arraycopy(buffer, i, numBuffer, 0, 2);
	pack->hour = atoi_sss(numBuffer);
	i = i + 2;

	memset(numBuffer, 0, sizeof(numBuffer));
	arraycopy(buffer, i, numBuffer, 0, 2);
	pack->minute = atoi_sss(numBuffer);
	i = i + 2;

	memset(numBuffer, 0, sizeof(numBuffer));
	arraycopy(buffer, i, numBuffer, 0, 2);
	pack->second = atoi_sss(numBuffer);
	i = i + 2;

	arraycopy(buffer, i, pack->SensorHubName, 0, SENSORHUB_NAME_SIZE);
	i = i + SENSORHUB_NAME_SIZE;

	arraycopy(buffer, i, pack->SensorID, 0, SENSOR_ID_SIZE);
	i = i + SENSOR_ID_SIZE;

	arraycopy(buffer, i, pack->SensorType, 0, SENSOR_TYPE_SIZE);
	i = i + SENSOR_TYPE_SIZE;

	memset(numBuffer, 0, sizeof(numBuffer));
	arraycopy(buffer, i, numBuffer, 0, 5);
	pack->value = atoi_sss(numBuffer);
}

void arraycopy(char *src, int srcPos, char *dst, int dstPos, int length)
{
	int i = 0;
	for (i = srcPos; i < length + srcPos; i++)
	{
		dst[dstPos] = src[i];
		dstPos++;
	}
}


float pow__(int base, int ex)
{
	int i = 0;
	long result = base;
	bool isGr0 = true;

	if (ex == 0)
		return 1;

	if (ex < 0)
	{
		isGr0 = false;
		ex = -ex;
	}

	for(i = 0; i < ex - 1; i++)
	{
		result = result * base;
	}

	if (isGr0 == true)
		return result;

	else
	{
		float x = 1/result;
		return x;
	}
}

int getStrSize(char *str)
{
	int i = 0;
	if (str[0] == '\0')
		return 0;
	else
		while(str[i++] != '\0');

	return i + 1;
}

void ftobyte(float val, char *buffer)
{
	char temp[4];
	float fPart;
	int iPart, fint, index = 0;

	iPart = (int)val;
	fPart = val - iPart;

	//.2f
	fint = (int)(fPart * 100);

	int2Char(iPart, temp);
	arraycopy(temp, 0, buffer, index, 4);
	index = index + 4;

	int2Char(fint, temp);
	arraycopy(temp, 0, buffer, index, 4);
}

void int2Char(int val, char *buffer)
{
	memset(buffer, 0, 4);
	buffer[0] = (val >> 24) & 0xFF;
	buffer[1] = (val >> 16) & 0xFF;
	buffer[2] = (val >> 8) & 0xFF;
	buffer[3] = val & 0xFF;
}

int chartoint(unsigned char *buffer)
{
	int result = 0, i = 0;
	for (i = 0; i < 4; i++)
	{
		result = (result << 8) | buffer[i];
	}
	return result;
}

void itoa_sss(int i, char b[])
{
    char const digit[] = "0123456789";
    char* p = b;
    if(i < 10){
        *p++ = 0x30;
    //    i *= -1;
    }
    int shifter = i;
    do{
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{
        *--p = digit[i%10];
        i = i/10;
    }while(i);
   // return b;
}

int atoi_sss(char array[])
{
	int value = 0, index = 0;
	while(array[index] != '\0')
	{
		value = value * 10 + (array[index] - 48);
		index++;
	}
	return value;
}

