/*
 * main.c
 */

#include "functions.h"
#include "timer.h"
#include "package.h"

char clientSample[CLIENT_TYPE_SIZE] = SERVER;
char controlSample[CONTROL_SIGNAL_SIZE] = SEND_DATA;
char sshSample[SENSORHUB_NAME_SIZE] = HUB01;
char ssidSample[SENSOR_ID_SIZE] = SENSOR_LIGHT_1;
char sstSample[SENSOR_TYPE_SIZE] = LIGHT_SENSOR;


char SSID[100] = "";
char PSSWD[64] = "";
char ESPRecvBuffer[MAX_SIZE];
char ESPSendBuffer[MAX_SIZE];
char sendPackBuffer[PACKAGE_MAX_SIZE];
char recvPackBuffer[PACKAGE_MAX_SIZE];
char IPList[100];
char *clientState = INIT;
char *sensorHubState = INIT;
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
int port = 12345;
bool isInitStage = true;
unsigned int packageList_count = 0;
unsigned int packageList_indexS = 0;
unsigned int packageList_indexR = 0;

struct Package packageList[100];
struct Package sendPack = {{0}, 0, 0, 0, 0, 0, 0, {0}, {0}, {0}, 0.0};

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
/*
 * Network data process related functions
 */
bool SendData(char *content, int size, int connectionID, char *flag);
void sensorHubHandler(char *buffer, int connectionID);
void clientHandler(char *buffer, int connectionID);
void ProcessData();
void createPackageForCmd(char *clientName, char *controlSignal, char *dstBuffer);
int str_cmp(char str1[], char str2[]);
//for testing
void test_phase1();
void initPackageList();
void packageTesting(struct Package *pack);
void createSamplePackage();
/*
 * Common functions
 */
int strtoint(char *buffer);

/*
 * For Debug purposes
 */
void Debug(char *mssg);

void main(void) {

	char temp;
	int i = 0;
	unsigned long start;
	
	/*
	 * Configure UART to PC and ESP moduleP
	 * Configure timer for timeout
	 */
	SysClockFreq = SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ | SYSCTL_OSC_MAIN | SYSCTL_USE_PLL | SYSCTL_CFG_VCO_480), 120000000);

	IntMasterEnable();

	TimerInit(SysClockFreq);

	CONSUARTConfig();
	ESPUARTConfig();

	/*
	 * Test ESP module setup
	 * Set module to mode 3 - Access point and Station
	 * Try to list available access points
	 * Connect to access point
	 */
//	createSamplePackage();
	UARTprintf("%sServer version 4.%s", space, space);
	Setup();
	while(true)
	{

		if (UARTCharsAvail(UART7_BASE))
		{
			if (SearchIndexOf(ESPRecvBuffer, "+IPD,") != -1)
			{
				i = 0;
				memset(ESPRecvBuffer, 0, sizeof(ESPRecvBuffer));
				start = getMilliseconds();
				while (getMilliseconds() - start < 5000)
				{
					while (UARTCharsAvail(UART7_BASE))
					{
						temp = UARTCharGet(UART7_BASE);
						if(temp == '\0') continue;
						ESPRecvBuffer[i] = temp;
						i++;
					}
					if (i > PACKAGE_MAX_SIZE + 4)
						break;
				}
				ProcessData();
				i = 0;
				Delayms(100);
				memset(ESPRecvBuffer, 0, sizeof(ESPRecvBuffer));
			}
			else
			{
				temp = UARTCharGet(UART7_BASE);
				if(temp == '\0') continue;
				ESPRecvBuffer[i]= temp;
				i++;
			}
		}
	}
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

	TaskStatus = SetMultiConnection();

	if (TaskStatus)
	{
		UARTprintf("Set multiconnection: %s\n%s\n", "OK", space);
		TaskStatus = false;
	}

	TaskStatus = SetAsServer(1, port);
	if (TaskStatus)
	{
		UARTprintf("Set ESP module as server: %s\nPort: %d\n%s\n", "OK", port, space);
		TaskStatus = false;
	}

	TaskStatus = SetSVTimeout();
	if (TaskStatus)
	{
		UARTprintf("Set server timeout in 7200s: %s\n%s\n", "OK", space);
		TaskStatus = false;
	}

	TaskStatus = disableEcho();
	if(TaskStatus == true)
	{
		UARTprintf("Disable ECHO.\n%s", space);
		UARTprintf("Waiting for client...%s", space);
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

bool SendData(char* content, int size, int connectionID, char *flag)
{
	memset(ESPRecvBuffer, 0, sizeof(ESPRecvBuffer));

	UARTprintf("Sent to SSH\n");
	UARTprintf("%s\n", content);
	return CIPSENDesp(ESPRecvBuffer, content, size, connectionID, flag);
}

void Debug(char *mssg)
{
	UARTprintf(mssg);
	UARTprintf(ESPRecvBuffer);
	UARTprintf("\n");
}

void test_phase1()
{
	//Construct sample package
	PackageConst(&sendPack, PC_CLIENT, "NOTIFY", 2016, 12, 30, 9, 40, 20, HUB01, SENSOR_TEMP_1, TEMP_SENSOR, (float)34.34);
	Serialize(&sendPack, sendPackBuffer);
	SendData(sendPackBuffer, PACKAGE_MAX_SIZE, 0, PACKAGE);
}

void ProcessData()
{
	/*
	 * filter ESPReceiver for information -- for
	 * connectionID of incoming connection
	 * size of content of data
	 * content of data
	 */

	int connectionID = ESPRecvBuffer[0] - 48;
	char incomingData[PACKAGE_MAX_SIZE + 1];
	char dataSize[7];
	int incomingDataSize = 0, index = 2, stick = 0;

	memset(dataSize, 0, sizeof(dataSize));
	memset(incomingData, 0, sizeof(incomingData));
	while(ESPRecvBuffer[index] != ':')
	{
		dataSize[index - 2] = ESPRecvBuffer[index];
		index++;
	}

	incomingDataSize = strtoint(dataSize);
	stick = index + 1;
	index++; //index is at ":" character, so ignore it

	int i = 0;
	for (index = index; index < incomingDataSize + stick; index++)
	{
		incomingData[i] = ESPRecvBuffer[index];
		i++;
	}
	UARTprintf(space);
	UARTprintf("Incoming data: %s", incomingData);
	UARTprintf(space);
	if (strstr(incomingData, PC_CLIENT) != NULL)
		clientHandler(incomingData, connectionID);
	else if (strstr(incomingData, HUB_SENSOR) != NULL)
		sensorHubHandler(incomingData, connectionID);
}

void clientHandler(char *buffer, int connectionID)
{
	memset(ESPSendBuffer, 0, sizeof(ESPSendBuffer));
	char mssg[CONTROL_SIGNAL_SIZE];
	arraycopy(buffer, CLIENT_TYPE_SIZE, mssg, 0, CONTROL_SIGNAL_SIZE);
	if (strstr(buffer, UPDATE_DATA) != NULL)
		clientState = INIT;

	else if (strstr(buffer, GET_DATA) != NULL)
		clientState = IN_PROGRESS;

	if (strstr(clientState, INIT) != NULL)
	{
		createPackageForCmd(SERVER, READY, ESPSendBuffer);
		SendData(ESPSendBuffer, PACKAGE_MAX_SIZE, connectionID, PACKAGE);
	}

	else if (strstr(clientState, IN_PROGRESS) != NULL)
	{
		if (packageList_indexS >= packageList_count)
		{
			createPackageForCmd(SERVER, END_OF_DATA, ESPSendBuffer);
			SendData(ESPSendBuffer, PACKAGE_MAX_SIZE, connectionID, PACKAGE);
		}
		else
		{
			Serialize(&(packageList[packageList_indexS % 100]), ESPSendBuffer);
			SendData(ESPSendBuffer, PACKAGE_MAX_SIZE, connectionID, PACKAGE);
			packageList_indexS++;
		}
	}
	clientState = "";
}

void sensorHubHandler(char *buffer, int connectionID)
{
	Delayms(250);
	memset(ESPSendBuffer, 0, sizeof(ESPSendBuffer));
	memset(ESPRecvBuffer, 0, sizeof(ESPRecvBuffer));
	struct Package recvPack = {{0}, 0, 0, 0, 0, 0, 0, {0}, {0}, {0}, 0.0};
	char mssg[CONTROL_SIGNAL_SIZE];
	memset(mssg, 0, sizeof(mssg));
	arraycopy(buffer, CLIENT_TYPE_SIZE, mssg, 0, CONTROL_SIGNAL_SIZE);
	if (strstr(buffer, UPDATE_DATA) != NULL)
		sensorHubState = INIT;
	else if (strstr(buffer, SEND_DATA) != NULL)
		sensorHubState = IN_PROGRESS;

	if (strstr(sensorHubState, INIT) != NULL)
	{
		createPackageForCmd(SERVER, READY, ESPSendBuffer);
		SendData(ESPSendBuffer, PACKAGE_MAX_SIZE, connectionID, PACKAGE);
	}

	else if (strstr(sensorHubState, IN_PROGRESS) != NULL)
	{
		if (packageList_count >= 100)
		{
			packageList_count = 100;
			packageList_indexR++;
		}
		else
		{
			packageList_count++;
			packageList_indexR++;
		}

		Deserialize(&recvPack, buffer);
		PackageConst(&(packageList[(packageList_indexR - 1) % 100]), SERVER, SEND_DATA,
				recvPack.yy, recvPack.mm, recvPack.dd, recvPack.hour, recvPack.minute, recvPack.second,
				recvPack.SensorHubName, recvPack.SensorID, recvPack.SensorType, recvPack.value);

		UARTprintf(space);
		UARTprintf("Package received from SensorHub:\n");
		printPackage(&(packageList[(packageList_indexR - 1) % 100]));
		UARTprintf(space);
		memset(ESPSendBuffer, 0, sizeof(ESPSendBuffer));
		createPackageForCmd(SERVER, GET_DATA, ESPSendBuffer);
		SendData(ESPSendBuffer, PACKAGE_MAX_SIZE, connectionID, PACKAGE);
	}
	sensorHubState = "";
}

void createPackageForCmd(char *clientName, char *controlSignal, char *dstBuffer)
{
	arraycopy(clientName, 0, dstBuffer, 0, CLIENT_TYPE_SIZE);
	arraycopy(controlSignal, 0, dstBuffer, CLIENT_TYPE_SIZE, CONTROL_SIGNAL_SIZE);
	arraycopy(redundant, 0, dstBuffer, CLIENT_TYPE_SIZE + CONTROL_SIGNAL_SIZE, sizeof(redundant));
}

void createSamplePackage()
{
	int i = 0;
	int yy = 2016, mm = 12, dd = 12, hour = 12, min = 12, sec = 12;
	int value = 10;
	for (i = 0; i < 20; i ++)
		PackageConst(&(packageList[i]), clientSample, controlSample, yy, mm, dd, hour, min, sec, sshSample, ssidSample, sstSample, value + i);
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

int str_cmp(char str1[], char str2[])
{
	int index = 0;
	if (getStrSize(str1) != getStrSize(str2))
		return 1;
	else
	{
		while(str1[index] != '\0')
		{
			if (str1[index] != str2[index])
				return 1;
			index++;
		}
		return 0;
	}
}
