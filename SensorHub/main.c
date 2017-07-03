/*
 * main.c
 */

#include "include.h"

int yy=2016;
int mm=12;
int dd=12;
int hh=0;
int pp=0;
int ss=0;
uint32_t SysClockFreq;

char SSID[100] = "";
char PSSWD[64] = "";
char serverIP[16] = "";
char serverPort[6] = "";
char ESPRecvBuffer[MAX_SIZE];
char ESPSendBuffer[MAX_SIZE];
char IPList[100];
char space[] = "\n------------------***-------------------\n";
char redundant[PACKAGE_MAX_SIZE - CLIENT_TYPE_SIZE - CONTROL_SIGNAL_SIZE] = {
		0x23, 0x23, 0x23, 0x23, 0x23,
		0x23, 0x23, 0x23, 0x23, 0x23,
		0x23, 0x23, 0x23, 0x23, 0x23,
		0x23, 0x23, 0x23, 0x23, 0x23,
		0x23, 0x23, 0x23, 0x23, 0x23,
		0x23, 0x23, 0x23, 0x23, 0x23,
		0x23, 0x23, 0x23};
//46 - 13 characters

uint32_t SysClockFreq;
bool readDone = false;
bool isInitStage = true;
int port = 12345;

unsigned int packageSent = 0;
unsigned int packageIdx = 0;

struct Package packageList[100];

/*
 * ESP module related functions
 */
bool disableEcho();
bool enableEcho();
bool SetupTesing();
bool ResetESP();
bool SetAPAndStation();
bool JoinAP();
bool QuitAP();
bool SetMultiConnection();
bool GetLocalIP(char *IPList);
bool SetAsServer(int mode, int port);
bool SetSVTimeout();
void Setup();
bool setUpConnection();
/*
* Network data process related functions
*/
bool SendData(char *content, int size, char *flag);
void ReadData();
void ProcessData();
void createPackageForCmd(char *clientName, char *controlSignal, char *dstBuffer);
void createSamplePackage();
//for testing
/*
* Common functions
*/
int strtoint(char *buffer);
void time(int minute, int hour);
void packData(int second, int minute, int hour, char *ssh, char *ssid, char *sst, int val);

void main(void) {
	int minute = 45;
	int hour   = 10;

	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);//40 MHz
	IntMasterEnable();
	TimerInit(40000);//1ms / 25ns(40MHz each clock =25ns)=40000
	ConfigureUART();
	ESPUARTConfig();
	//BH1750
	initI2C();
	bh1750_init();

	//DHT11
	DHT_init();
	Delayms(2000);
	th.celsius_temp= 0;
	th.humidity = 0;
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	UARTprintf("%s\nSensorHub version 3.%s", space, space);
	Setup();
	Delayms(1000);
	while(1)
	{
		time(minute,hour);
		dht_readTH(&th);

		packData(ss, pp, hh, HUB01, SENSOR_TEMP_1, TEMP_SENSOR, (int)th.celsius_temp);
		packData(ss, pp, hh, HUB01, SENSOR_HUMIDITY_1, HUMIDITY, (int)th.humidity);

		i2cReadData(BH1750_ADDR,&data);
		packData(ss, pp, hh, HUB01, SENSOR_LIGHT_1, LIGHT_SENSOR, data);

		if (isSendData2Server() == true)
		{
			createPackageForCmd(HUB_SENSOR, UPDATE_DATA, ESPSendBuffer);
			SendData(ESPSendBuffer, PACKAGE_MAX_SIZE, PACKAGE);
			donotSendData2Server();
		}
		ReadData();
	}
}

void packData(int second, int minute, int hour, char *ssh, char *ssid, char *sst, int val)
{
	PackageConst(&(packageList[packageIdx % 100]), HUB_SENSOR, SEND_DATA, yy, dd, mm, hh, pp, ss, ssh, ssid, sst, val);
	packageIdx++;
}

void time(int minute,int hour) {
	ss=0;
	pp=minute;
	hh=hour;
	minute = 0;
	hour =0;
	int second=getMilliseconds()/1000;
	if (second > 59 ){
		pp= pp + second / 60;
		if (pp>59) hh=hh+ pp / 60;
		pp = pp % 60 ;
	}
	ss= second % 60;
}

void ProcessData()
{
	/*
	 * filter ESPReceiver for information -- for
	 * connectionID of incoming connection
	 * size of content of data
	 * content of data
	 */
	Delayms(250);
	char incomingData[PACKAGE_MAX_SIZE + 1];
	char dataSize[7];
	int incomingDataSize = 0, index = 0, stick = 0;


	memset(dataSize, 0, sizeof(dataSize));
	memset(incomingData, 0, sizeof(incomingData));
	while(ESPRecvBuffer[index] != ':')
	{
		dataSize[index] = ESPRecvBuffer[index];
		index++;
	}

	incomingDataSize = strtoint(dataSize);
	stick = index + 1;
	index++;			//index is at ":" character, so ignore it

	int i = 0;
	for (index = index; index < incomingDataSize + stick; index++)
	{
		incomingData[i] = ESPRecvBuffer[index];
		i++;
	}
	UARTprintf("Data received: %s\n", incomingData);
	if (strstr(incomingData, SERVER) != 0)
	{
		if (strstr(incomingData, READY) != 0)
		{
			Serialize(&(packageList[0]), ESPSendBuffer);
			SendData(ESPSendBuffer, PACKAGE_MAX_SIZE, PACKAGE);
			packageSent++;
			printPackage(&(packageList[0]));
			UARTprintf("\nSent first package.\n");
		}
		else if (strstr(incomingData, GET_DATA) != 0)
		{
			if (packageSent == packageIdx)
			{
				createPackageForCmd(HUB_SENSOR, END_OF_DATA, ESPSendBuffer);
				SendData(ESPSendBuffer, PACKAGE_MAX_SIZE, PACKAGE);
			}
			else
			{
				Serialize(&(packageList[packageSent % 100]), ESPSendBuffer);
				SendData(ESPSendBuffer, PACKAGE_MAX_SIZE, PACKAGE);
				packageSent++;
			}
		}
	}
}

void ReadData()
{
	int i = 0, start = 0, begin = 0;
	char temp;
	memset(ESPRecvBuffer, 0, sizeof(ESPRecvBuffer));
	readDone = false;
	begin = getMilliseconds();
	while(getMilliseconds() - begin < 3000)
	//while(true)
	{
		if (UARTCharsAvail(UART3_BASE))
		{
			if (SearchIndexOf(ESPRecvBuffer, "+IPD,") != -1)
			{
				i = 0;
				memset(ESPRecvBuffer, 0, sizeof(ESPRecvBuffer));
				start =	 getMilliseconds();
				while (getMilliseconds() - start < 3000)
				{
					while (UARTCharsAvail(UART3_BASE))
					{
						temp = UARTCharGet(UART3_BASE);
						if(temp == '\0') continue;
						ESPRecvBuffer[i] = temp;
						i++;
					}
					if (i > PACKAGE_MAX_SIZE + 2)
					{
						readDone = true;
						break;
					}
				}
				ProcessData();
				i = 0;
				Delayms(100);
				memset(ESPRecvBuffer, 0, sizeof(ESPRecvBuffer));
			}
			else
			{
				temp = UARTCharGet(UART3_BASE);
				if(temp == '\0') continue;
				ESPRecvBuffer[i]= temp;
				i++;
			}
		}
		if (readDone) break;
	}
}

void createPackageForCmd(char *clientName, char *controlSignal, char *dstBuffer)
{
	arraycopy(clientName, 0, dstBuffer, 0, CLIENT_TYPE_SIZE);
	arraycopy(controlSignal, 0, dstBuffer, CLIENT_TYPE_SIZE, CONTROL_SIGNAL_SIZE);
	arraycopy(redundant, 0, dstBuffer, CLIENT_TYPE_SIZE + CONTROL_SIGNAL_SIZE, sizeof(redundant));
}

int strtoint(char *buffer)
{
	int i = 0, result = 0;
	while (buffer[i] != '\0')
	{
		result = (result * 10) + (buffer[i] - 48);
		i++;
	}

	return result;
}

void Setup()
{
	bool TaskStatus = false;
	if(isInitStage)
		TaskStatus = QuitAP();
	if (TaskStatus)
	{
		UARTprintf("Quit AP: %s\n%s\n", "OK", space);
		TaskStatus = false;
		isInitStage = false;
	}

	TaskStatus = SetupTesing();
	if (TaskStatus == true)
	{
		UARTprintf("Setup testing: %s\n%s\n", "OK", space);
		TaskStatus = false;
	}


	TaskStatus = SetAPAndStation();

	if (TaskStatus == true)
	{
		UARTprintf("Set ESP module as AP and Station: %s\n%s\n", "OK", space);
		TaskStatus = false;
	}

	while(true)
	{
		TaskStatus = JoinAP();
		if (TaskStatus)
		{
			UARTprintf("Joined to AP.\nGet local IP address:\n");
			TaskStatus = GetLocalIP(IPList);
			UARTprintf(IPList);
			UARTprintf("\n%s", space);
			TaskStatus = false;
			break;
		}
	}

	TaskStatus = setUpConnection();
	if(TaskStatus == true)
	{
		UARTprintf("Connection established.\n");
		UARTprintf(space);
	}

	TaskStatus = disableEcho();
	if(TaskStatus == true)
	{
		UARTprintf("Disable ECHO.\n%s", space);
		UARTprintf("Waiting for client...\n");
	}
	memset(ESPRecvBuffer, 0, sizeof(ESPRecvBuffer));
}

bool disableEcho()
{
	memset(ESPRecvBuffer, 0, sizeof(ESPRecvBuffer));
	return ATE0esp(ESPRecvBuffer);
}

bool enableEcho()
{
	memset(ESPRecvBuffer, 0, sizeof(ESPRecvBuffer));
	return ATE1esp(ESPRecvBuffer);
}

bool SetupTesing()
{
	memset(ESPRecvBuffer, 0, sizeof(ESPRecvBuffer));
	return ATesp(ESPRecvBuffer);
}

bool ResetESP()
{
	memset(ESPRecvBuffer, 0 , sizeof(ESPRecvBuffer));
	return RSTesp(ESPRecvBuffer);
}

bool SetAPAndStation()
{
	memset(ESPRecvBuffer, 0 , sizeof(ESPRecvBuffer));
	return CWMODEesp(ESPRecvBuffer);
}

bool JoinAP()
{
	char getBuffer[100];
	memset(ESPRecvBuffer, 0 , sizeof(ESPRecvBuffer));
	UARTprintf("Input SSID: ");
	UARTgets(getBuffer, 100);
	strcpy(SSID, getBuffer);

	memset(getBuffer, 0, sizeof(getBuffer));

	UARTprintf("Password: ");
	UARTgets(getBuffer, 100);
	strcpy(PSSWD, getBuffer);

	UARTprintf("Connecting to AP...\n%s\n", space);
	return CWJAPesp(ESPRecvBuffer, SSID, PSSWD);
}

bool QuitAP()
{
	memset(ESPRecvBuffer, 0 , sizeof(ESPRecvBuffer));
	return CWQAPesp(ESPRecvBuffer);
}

bool SetMultiConnection()
{
	memset(ESPRecvBuffer, 0 , sizeof(ESPRecvBuffer));
	return CIPMUXesp(ESPRecvBuffer);
}

bool GetLocalIP(char *IPList)
{
	memset(ESPRecvBuffer, 0 , sizeof(ESPRecvBuffer));
	return CIFSResp(ESPRecvBuffer, IPList);
}

bool SetAsServer(int mode, int port)
{
	memset(ESPRecvBuffer, 0, sizeof(ESPRecvBuffer));
	return CIPSERVEResp(ESPRecvBuffer, mode, port);
}

bool SetSVTimeout()
{
	memset(ESPRecvBuffer, 0, sizeof(ESPRecvBuffer));
	return CIPSTOesp(ESPRecvBuffer);
}

bool SendData(char* content, int size, char *flag)
{
	memset(ESPRecvBuffer, 0, sizeof(ESPRecvBuffer));
	return CIPSENDesp(ESPRecvBuffer, content, size, flag);
}

bool setUpConnection()
{
	char getBuffer[100];
	memset(ESPRecvBuffer, 0 , sizeof(ESPRecvBuffer));
	UARTprintf("Input Server IP: ");
	UARTgets(getBuffer, 100);
	strcpy(serverIP, getBuffer);

	memset(getBuffer, 0, sizeof(getBuffer));

	UARTprintf("Input Server port: ");
	UARTgets(getBuffer, 100);
	strcpy(serverPort, getBuffer);

	UARTprintf("Connecting to %s, port %s...\n%s\n", serverIP, serverPort, space);
	return CIPSTARTesp(ESPRecvBuffer, serverIP, serverPort);
}

void createSamplePackage()
{
	int i = 0;
	int yy = 2016, mm = 12, dd = 12, hour = 12, min = 12, sec = 12;
	int value = 10;
	for (i = 0; i < 20; i ++)
		PackageConst(&(packageList[i]), HUB_SENSOR, SEND_DATA, yy, mm, dd, hour, min, sec, HUB01, SENSOR_TEMP_1, TEMP_SENSOR, value + i);
	packageIdx = i;
}
