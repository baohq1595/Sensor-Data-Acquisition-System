/*
 * functions.h
 *
 *  Created on: Sep 6, 2016
 *      Author: BAOBAO
 */

#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "inc/hw_gpio.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_ints.h"
#include "driverlib/fpu.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"

#include "utils/uartstdio.h"
#include "driverlib/uart.h"
#include "driverlib/interrupt.h"

#include "package.h"

#define MAX_SIZE 	100
#define STRING 		"STRING"
#define PACKAGE 	"PACKAGE"

/*
 * Configuring functions
 */
extern void ConfigureUART();		//UART0 to console
extern void ESPUARTConfig();		//UART7 to  ESP
extern void LEDConfig();
extern void TimerTesing();

/*
 * UART send-receive functions
 */
extern void SendATCmd(const char *);		//Send AT command to ESP
extern void SendStr(const char *);		//Send data package into ESP
extern void SendPackage(const char *);
extern int ESPRecv(char *data, char *target, unsigned int timeout, bool check);			//Receive data from ESP
extern bool ESPRecv_Find(char *data, char *target, unsigned int timeout, bool check);	//Receive data from ESP
extern bool ESPRecv_FindAndFilter(char *buffer, char *target, char *begin, char *end, unsigned int timeout, bool check, char *data);  //Receive data from ESP

/*
 * Wifi functions
 */
bool ATE0esp(char *buffer);
bool ATE1esp(char *buffer);
extern bool ATesp(char *buffer);		//Setup Testing
extern bool RSTesp(char *buffer);		//Reset ESP module
extern bool CWMODEesp(char *buffer);	//Set ESP module to SoftAP and Station
extern bool CWJAPesp(char *buffer, const char *ssid, const char *psswd);		//Join ESP module to AP
extern bool CWQAPesp(char *buffer);		//Quit AP

/*
 * TCP/IP, UDP functions
 */
extern bool CIPMUXesp(char *buffer);				//Set ESP module to allow multiconnection
extern bool CIFSResp(char *buffer, char *list);		//Get local IP address
extern bool CIPCLOSEesp(char *buffer);				//Close TCP or UDP connection
extern bool CIPSENDesp(char *buffer, char *content, int len, char *flag);	//Send data
extern bool CIPSERVEResp(char* buffer, int mode, int port);	//Setup server on ESP module
extern bool CIPSTOesp(char *buffer);				//Set server timeout - 10s
extern bool CIPCLOSEesp(char *buffer);
extern bool CIPSTARTesp(char *buffer, const char *ip, const char *port);

/*
 * Data structure related functions
 */

/*
 * Common functions
 */
extern void memclr(char *buffer, int value, uint32_t size);		//reset memory
extern int SearchIndexOf(char *src, char *str);					//Find first occurence of string
extern void itoa(int i, char b[]);								//integer to string
extern int atoi(char array[]);
//extern void ftoa(float f,char *buf);							//float to string
extern double atof(char* num);									//string to double

#endif /* FUNCTIONS_H_ */
