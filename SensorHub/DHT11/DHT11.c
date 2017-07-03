/*
 * DHT11.c
 *
 *  Created on: Oct 1, 2016
 *      Author: trannhanduc95
 */

#include "../include.h"
#include "DHT11.h"


// Private methods

float dht_readTemperature(void);
float dht_readHumidity(void);
uint8_t dht_read(void);
void DHTIntHandler(void);


// Private data
uint8_t data_buffer[6];
uint32_t dht_timing;
bool quit_timing = false;

void DHT_init(void){
	//Provide clock for Port F
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER5);
	TimerConfigure(TIMER5_BASE, TIMER_CFG_ONE_SHOT_UP);//Full-with
	TimerLoadSet(TIMER5_BASE, TIMER_A, DHT_TIMEOUT+1000);

	GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_BOTH_EDGES); // config interrupt both edges
	GPIOIntRegister(GPIO_PORTF_BASE, DHTIntHandler);

	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);
}

// RETURN:
// 1 = success reading from sensor;
// 0 = error on reading from sensor;
// also return T and H

uint8_t dht_readTH(DHT_TypeDef *values) {
    if (dht_read() == 1) {
        // temperature
        values->celsius_temp = dht_readTemperature();
        // humidity
        values->humidity = dht_readHumidity();
        return 1;
    }
    return 0;
}

float dht_readTemperature() {
    float t;
    t = data_buffer[2];

    t = data_buffer[2] & 0x7F;
    t *= 256;
    t += data_buffer[3];
    t /= 10.0;

    if (data_buffer[2] & 0x80)
        t *= -1.0;

    if (t > 80.0) {
    	t = 80.0;
    }
    if (t < -40.0) {
    	t = -40.0;
    }

    return t;
  }

float dht_readHumidity() {
    float h;
    h = data_buffer[0];
    h = data_buffer[0];
    h *= 256;
    h += data_buffer[1];
    h /= 10.0;

    if (h > 100.0) {
    	h = 100.0;
    }
    if (h < 0.0) {
    	h = 0.0;
    }
    return h;
}

uint8_t dht_read(void) {
    uint8_t ii, byteIdx, bitCounter;

     // clear data
    for (ii=0; ii< 5; ii++)
    *(data_buffer + ii) = 0;

     // send start signal low and wait 1-18 ms
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, 0x00);
    SysCtlDelay(DHT_WAIT_18ms);

    // send pull up signal high and wait 20-40 us
    GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_PIN_4);

    // wait at least 20us
    SysCtlDelay(DHT_WAIT_20us);

    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
    while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == GPIO_PIN_4);

    // acknowledge ~80us low
    while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == 0x00);
    // acknowledge ~80us high
    while(GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == GPIO_PIN_4);

    // gets data 5 bytes, 40 bits
    // every bit start with low level for 50us then high level last
    // 0 = 26-28us
    // 1 = 70us
    byteIdx = 0;
    bitCounter = 7;
    for (ii=0; ii<40; ii++) {
    	quit_timing = false;

        // do elaboration
      	while(!quit_timing);

        // check bit (timing is 1us) 26-28 -> 0 - 70us -> 1
        if (dht_timing > DHT_TIME_BIT_1) {    // >70us
            data_buffer[byteIdx] |= (1 << bitCounter);
          }

         if (bitCounter == 0) {
             bitCounter = 7;
             byteIdx++;
         }
         else bitCounter--;
      }
      return 1;
  }

void DHTIntHandler(){
    volatile int32_t start_timer;

    GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_4); // clear interrupt flag
   	if (GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4) == GPIO_PIN_4) { // check low to high
    TimerEnable(TIMER5_BASE, TIMER_A);
    start_timer = TimerValueGet(TIMER5_BASE, TIMER_A);
    quit_timing = false;
    }
   	else { // check high to low, finish
    TimerDisable(TIMER5_BASE, TIMER_A);
    dht_timing = TimerValueGet(TIMER5_BASE, TIMER_A);
    dht_timing -= start_timer;
    quit_timing = true;
    HWREG(TIMER5_BASE + TIMER_O_TAV) = 0;//Loads value 0 into the timer
   	}
}
