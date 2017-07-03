/*
 * timer.h
 *
 *  Created on: Sep 6, 2016
 *      Author: BAOBAO
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>

extern void TimerInit(uint32_t ui32SysClkFreq);
extern unsigned long getMilliseconds();
extern void Delayms(uint32_t Period);
static unsigned long milliseconds = 0;


#endif /* TIMER_H_ */
