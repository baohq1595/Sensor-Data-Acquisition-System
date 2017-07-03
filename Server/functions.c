/*
 * functions.c
 *
 *  Created on: Sep 6, 2016
 *      Author: BAOBAO
 */

#include "functions.h"
#include "timer.h"

char cmdBuffer[128];

char *Substring(char *src, char *dst, int start, int stop);
int SearchIndexOf(char *src, char *str);
int ESPRecv(char *data, char *target, unsigned int timeout, bool check);

void CONSUARTConfig()
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlDelay(3);
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);

    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlDelay(3);

    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    UARTStdioConfig(0, 115200, 120000000);
}

void ESPUARTConfig()
{
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
	SysCtlDelay(3);
	GPIOPinConfigure(GPIO_PC4_U7RX);
	GPIOPinConfigure(GPIO_PC5_U7TX);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART7);
	SysCtlDelay(3);

	GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);

	UARTClockSourceSet(UART7_BASE, UART_CLOCK_SYSTEM);

	UARTConfigSetExpClk(UART7_BASE, 120000000, 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	UARTEnable(UART7_BASE);
}

void SendATCmd(const char *cmd)
{
	unsigned int index = 0;
	while(UARTBusy(UART7_BASE));
	while(cmd[index] != '\0')
	{
		UARTCharPut(UART7_BASE, cmd[index++]);
	}
	UARTCharPut(UART7_BASE, '\r');
	UARTCharPut(UART7_BASE, '\n');
}

void SendStr(const char *buffer)
{
	unsigned int index = 0;
	while(UARTBusy(UART7_BASE));
	while(buffer[index] != '\0')
	{
		UARTCharPut(UART7_BASE, buffer[index++]);
	}
}

void SendPackage(const char *buffer)
{
	unsigned int index = 0;
	char temp_forDebug;
	while(UARTBusy(UART7_BASE));
	for (index = 0; index < PACKAGE_MAX_SIZE; index++)
	{
		temp_forDebug = buffer[index];
		UARTCharPut(UART7_BASE, temp_forDebug);
	}
}

int ESPRecv(char *data, char *target, unsigned int timeout, bool check)
{
	unsigned long start = getMilliseconds();
	unsigned int index = 0;
	char temp;

	memset(data, 0, sizeof(data));

	while (getMilliseconds() - start < timeout)
	{
		while (UARTCharsAvail(UART7_BASE))
		{
			temp = UARTCharGet(UART7_BASE);
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
	return ESPRecv_Find(buffer, "OK", 5000, true);
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
	return ESPRecv_FindAndFilter(buffer, "OK", "\r\r\n", "\r\n\r\nOK", 1500, true, list);
}

bool CIPCLOSEesp(char *buffer)
{
	SendATCmd("AT+CIPCLOSE");
	return ESPRecv_Find(buffer, "OK", 5000, true);
}

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

bool CIPSENDesp(char *buffer, char *content, int len, int id, char *flag)
{
	memset(cmdBuffer, 0, sizeof(cmdBuffer));
	char len_str[5];
	char id_str[2];
	bool isSendOK = false;

	itoa(id, id_str);
	itoa(len, len_str);

	char* AT_CMD_SEND = "AT+CIPSEND=";
	char* comma = ",";
	strcpy(cmdBuffer, AT_CMD_SEND);
	strcat(cmdBuffer, id_str);
	strcat(cmdBuffer, comma);
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

bool CIPSTOesp(char *buffer)
{
	SendATCmd("AT+CIPSTO=7200");
	return ESPRecv_Find(buffer, "OK", 2000, true);
}

char *Substring(char *src, char *dst, int start, int stop)
{
	int len = stop - start;
	strncpy(dst, src + start, len);

	return dst;
}

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
