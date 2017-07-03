/*
 * timer.c
 *
 *  Created on: Sep 6, 2016
 *      Author: BAOBAO
 */

#include "timer.h"

#include <stdint.h>
#include <stdbool.h>
#include <math.h>
//#include "inc/tm4c1294ncpdt.h"
#include "inc/hw_ints.h"
#include "inc/hw_timer.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "inc/hw_nvic.h"

void TimerInit(uint32_t ui32SysClkFreq)
{

	//Period to load into timer register
	uint32_t ui32Period;

	//Enable Timer0 Peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

	//Configure Timer0 to periodic timer, count down by default
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	//1ms ~ 1000Hz
	//Formula: System clock devide by expected Frequency
	ui32Period = ui32SysClkFreq / 1000;

	//Load period value into timer - full width load into TimerA only
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);

	//Enable Timer0 interrupt, set interrupt as timer time-out, enable NVIC
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	//Start timer
	TimerEnable(TIMER0_BASE, TIMER_A);
}

void Timer0IntHandler(void)
{
	//Clear interrupt flag
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);

	//TODO
	++milliseconds;
}

unsigned long getMilliseconds()
{
	return milliseconds;
}

void Delayms(uint32_t Period)
{
	uint32_t Start;
	Start = getMilliseconds();
	while(getMilliseconds() - Start < Period)
	{
		//do nothing
		;
	}
}
