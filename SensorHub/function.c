/*
 * functions.c
 *
 *  Created on: Sep 6, 2016
 *      Author: BAOBAO
 */

#include "include.h"
#include "function.h"
#include "package.h"
#include "timer.h"

char cmdBuffer[128];

char *Substring(char *src, char *dst, int start, int stop);
int SearchIndexOf(char *src, char *str);
int ESPRecv(char *data, char *target, unsigned int timeout, bool check);

/*
 * UART0 for debug purpose
 * Connects with PC via usb port
 */
void ConfigureUART()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTStdioConfig(0, 115200, SysCtlClockGet());
}

/*
 * UART3 to connect with ESP8266
 */
void ESPUARTConfig()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlDelay(3);

	GPIOPinConfigure(GPIO_PC6_U3RX);
	GPIOPinConfigure(GPIO_PC7_U3TX);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
	SysCtlDelay(3);

	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);

	UARTClockSourceSet(UART3_BASE, UART_CLOCK_SYSTEM);

	UARTConfigSetExpClk(UART3_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	UARTEnable(UART3_BASE);
}

/*
 * Write command to UART
 */
void SendATCmd(const char *cmd)
{
	unsigned int index = 0;
	while(UARTBusy(UART3_BASE));
	while(cmd[index] != '\0')
	{
		UARTCharPut(UART3_BASE, cmd[index++]);
	}
	UARTCharPut(UART3_BASE, '\r');
	UARTCharPut(UART3_BASE, '\n');
}

/*
 * Write data to UART
 */
void SendStr(const char *buffer)
{
	unsigned int index = 0;
	while(UARTBusy(UART3_BASE));
	while(buffer[index] != '\0')
	{
		UARTCharPut(UART3_BASE, buffer[index++]);
	}
}

/*
 * Write data to UART
 */
void SendPackage(const char *buffer)
{
	unsigned int index = 0;
	char temp_forDebug;
	while(UARTBusy(UART3_BASE));
	for (index = 0; index < PACKAGE_MAX_SIZE; index++)
	{
		temp_forDebug = buffer[index];
		UARTCharPut(UART3_BASE, temp_forDebug);
	}
}


/*
 * Read data from UART with a specific timeout
 */
int ESPRecv(char *data, char *target, unsigned int timeout, bool check)
{
	unsigned long start = getMilliseconds();
	unsigned int index = 0;
	char temp;

	memset(data, 0, sizeof(data));

	while (getMilliseconds() - start < timeout)
	{
		while (UARTCharsAvail(UART3_BASE))
		{
			temp = UARTCharGet(UART3_BASE);
			if (temp == '\0')
				continue;
			data[index] = temp;
			index++;
		}
		if (check)
			if (SearchIndexOf(data, target) != -1)
			{
				break;
			}
	}
	return 0;
}

/*
 * Read data from UART with a delimeter text (e.g OK)
 * If the content match with syntax expectation, the result is true
 */
bool ESPRecv_Find(char *data, char *target, unsigned int timeout, bool check)
{
	ESPRecv(data, target, timeout, check);

	if (SearchIndexOf(data, target) != -1)
	{
		//Receive OK
		return true;
	}
	return false;
}

/*
 * Read data from UART with more complex syntax
 */
bool ESPRecv_FindAndFilter(char *buffer, char *target, char *begin, char *end, unsigned int timeout, bool check, char *data)
{
	memset(buffer, 0, sizeof(buffer));
	memset(data, 0, sizeof(data));

	ESPRecv(buffer, target, timeout, check);

    if (SearchIndexOf(buffer, target) != -1)
    {
         int index1 = SearchIndexOf(buffer, begin);
         int index2 = SearchIndexOf(buffer, end);

         if (index1 != -1 && index2 != -1)
         {
             index1 += strlen(begin);
             Substring(buffer, data, index1, index2);
             return true;
         }
     }
     data = "";
     return false;
}

void TimerTesing()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
	GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0|GPIO_PIN_1, 0x00);

	while(1)
	{
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_INT_PIN_0 | GPIO_PIN_1, 2);
		Delayms(1000);
		GPIOPinWrite(GPIO_PORTN_BASE, GPIO_PIN_0 | GPIO_PIN_1, 0);
		Delayms(1000);
	}
}

bool ATE0esp(char *buffer)
{
	SendATCmd("ATE0");
	return ESPRecv_Find(buffer, "OK", 5000, true);
}

bool ATE1esp(char *buffer)
{
	SendATCmd("ATE1");
	return ESPRecv_Find(buffer, "OK", 5000, true);
}

bool ATesp(char *buffer)
{
	SendATCmd("AT");
	return ESPRecv_Find(buffer, "OK", 5000, true);
}

bool RSTesp(char *buffer)
{
	SendATCmd("AT+RST");
	return ESPRecv_Find(buffer, "OK", 5000, false);
}

bool CWMODEesp(char *buffer)
{
	SendATCmd("AT+CWMODE=3");
	return ESPRecv_Find(buffer, "OK", 1500, true);
}

bool CWJAPesp(char *buffer, const char* ssid, const char *psswd)
{
	memset(cmdBuffer, 0, sizeof(cmdBuffer));
	char *comma = ",";
	char *ATCmd = "AT+CWJAP=";
	memset(cmdBuffer, 0, sizeof(cmdBuffer));
	strcpy(cmdBuffer, ATCmd);
	strcat(cmdBuffer, ssid);
	strcat(cmdBuffer, comma);
	strcat(cmdBuffer, psswd);

	SendATCmd(cmdBuffer);
	return ESPRecv_Find(buffer, "OK", 20000, true);
}

bool CWQAPesp(char *buffer)
{
	SendATCmd("AT+CWQAP");
	return ESPRecv_Find(buffer, "OK", 10000, true);
}

bool CIPMUXesp(char *buffer)
{
	SendATCmd("AT+CIPMUX=1");
	return ESPRecv_Find(buffer, "OK", 5000, true);
}

bool CIFSResp(char *buffer, char *list)
{
	SendATCmd("AT+CIFSR");
	return ESPRecv_FindAndFilter(buffer, "OK", "\r\r\n", "\r\n\r\nOK", 2500, true, list);
}

bool CIPCLOSEesp(char *buffer)
{
	SendATCmd("AT+CIPCLOSE");
	return ESPRecv_Find(buffer, "OK", 5000, true);
}

/*
 * Setup server mode with specific waiting port
 */
bool CIPSERVEResp(char* buffer, int mode, int port)
{
	memset(cmdBuffer, 0, sizeof(cmdBuffer));
	char *toSend = "AT+CIPSERVER=";
	char c_mode[2], c_port[5], comma[1] = {','};
	itoa(mode, c_mode);
	itoa(port, c_port);
	strcpy(cmdBuffer, toSend);
	strcat(cmdBuffer, c_mode);
	strcat(cmdBuffer, comma);
	strcat(cmdBuffer, c_port);
	SendATCmd(cmdBuffer);

	return ESPRecv_Find(buffer, "OK", 5000, true);
}

/*
 * Send data to connection via CIPSEND command
 * @buffer is receiving buffer from UART
 * @content is the content to send to connection
 */
bool CIPSENDesp(char *buffer, char *content, int len, char *flag)
{
	memset(cmdBuffer, 0, sizeof(cmdBuffer));
	char len_str[5];
	bool isSendOK = false;
	itoa(len, len_str);

	char* AT_CMD_SEND = "AT+CIPSEND=";
	strcpy(cmdBuffer, AT_CMD_SEND);
	strcat(cmdBuffer, len_str);

	while(!isSendOK)
	{
		SendATCmd(cmdBuffer);
		isSendOK = ESPRecv_Find(buffer, ">", 1500, true);
	}
	memset(buffer, 0, sizeof(buffer));
	if (strcmp(flag, STRING) == 0)
		SendStr(content);
	else if (strcmp(flag, PACKAGE) == 0)
		SendPackage(content);
	isSendOK = false;
	Delayms(100);
	isSendOK = ESPRecv_Find(buffer, "Recv", 1500, true);

	UARTprintf("Debug from CIPSEND.\n");
	if(isSendOK) UARTprintf("Sending data is OK.\n");
	else UARTprintf("Sending data fails.\n");

	return isSendOK;
}


/*
 * AT+CIPSTO command to set timeout for server
 */

bool CIPSTOesp(char *buffer)
{
	SendATCmd("AT+CIPSTO=7200");
	return ESPRecv_Find(buffer, "OK", 2000, true);
}

/*
 * AT+CIPSTART command using TCP connection
 */
bool CIPSTARTesp(char *buffer, const char *ip, const char *port)
{
	memset(buffer, 0, sizeof(buffer));
	memset(cmdBuffer, 0, sizeof(cmdBuffer));
	char *comma = ",";
	char *ATCmd = "AT+CIPSTART=";
	char *tcp = "\"TCP\"";
	strcpy(cmdBuffer, ATCmd);
	strcat(cmdBuffer, tcp);
	strcat(cmdBuffer, comma);
	strcat(cmdBuffer, ip);
	strcat(cmdBuffer, comma);
	strcat(cmdBuffer, port);

	SendATCmd(cmdBuffer);
	return ESPRecv_Find(buffer, "OK", 2000, true);
}


/*
 * Get a sub-char buffer from another buffer with specific length
 */
char *Substring(char *src, char *dst, int start, int stop)
{
	int len = stop - start;
	strncpy(dst, src + start, len);

	return dst;
}

/*
 * Finds position of a sub-string in a string
 */
int SearchIndexOf(char *src, char *str)
{
   int i, j, firstOcc;
   i = 0, j = 0;

   while (src[i] != '\0')
   {

      while (src[i] != str[0] && src[i] != '\0')
         i++;

      if (src[i] == '\0')
         return (-1);

      firstOcc = i;

      while (src[i] == str[j] && src[i] != '\0' && str[j] != '\0')
      {
         i++;
         j++;
      }

      if (str[j] == '\0')
         return (firstOcc);
      if (src[i] == '\0')
         return (-1);

      i = firstOcc + 1;
      j = 0;
   }

   return (-1);
}

/*
 * Convert integer to char buffer
 */
void itoa(int i, char b[])
{
    char const digit[] = "0123456789";
    char* p = b;
    if(i < 0){
        *p++ = '-';
        i *= -1;
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

/*
 * Convert char buffer to integer
 */
int atoi(char array[])
{
	int value = 0, index = 0;
	while(array[index] != '\0')
	{
		value = value * 10 + (array[index] - 48);
		index++;
	}

	return value;
}
