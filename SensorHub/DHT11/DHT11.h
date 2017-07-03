/*
 * DHT11.h
 *
 *  Created on: Oct 1, 2016
 *      Author: trannhanduc95
 */

#ifndef DHT11
//#define DHT11_DHT11_H_
#define DHT11

#define MCU_CLOCK SysCtlClockGet()

#define DHT_WAIT_18ms ((MCU_CLOCK*18)/3000)
#define DHT_WAIT_20us ((MCU_CLOCK*2)/300000)

#define DHT_TIMEOUT    ((MCU_CLOCK*9)/100000) // 90us
#define DHT_TIME_BIT_1 ((MCU_CLOCK*7)/100000) // 70us


typedef struct {
    float celsius_temp;
    float humidity;
} DHT_TypeDef;


// Interface
extern void DHT_init(void);
extern uint8_t dht_readTH(DHT_TypeDef *);



#endif /* DHT11_DHT11_H_ */
