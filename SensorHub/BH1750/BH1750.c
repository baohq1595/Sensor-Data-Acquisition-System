/*
 * BH1750.c
 *
 *  Created on: Oct 2, 2016
 *      Author: trannhanduc95
 */
#include "../include.h"
#include "BH1750.h"



void initI2C(void){
	//Enable I2C1 peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
	// Insert a few cycles after enabling the peripheral to allow the clock to be fully activated
	SysCtlDelay(2);
	// Enable GPIOA peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlDelay(2);

	GPIOPinConfigure(GPIO_PA6_I2C1SCL);
	GPIOPinConfigure(GPIO_PA7_I2C1SDA);
	// Use pin with I2C SCL peripheral
	GPIOPinTypeI2CSCL(GPIO_PORTA_BASE, GPIO_PIN_6);
	// Use pin with I2C peripheral
	GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_7);
	// Enable and set frequency to 100 kHz (flase), 400kHz(true)
	I2CMasterInitExpClk(I2C1_BASE, SysCtlClockGet(), false);
	SysCtlDelay(2);
}

void i2cWrite(uint8_t addr, uint8_t regAddr) {
    I2CMasterSlaveAddrSet(I2C1_BASE, addr, false);// Set to write mode
    I2CMasterDataPut(I2C1_BASE, regAddr);
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_START);// Send start condition
    while (I2CMasterBusy(I2C1_BASE));// Wait until transfer is done
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);// Send finish condition
    while (I2CMasterBusy(I2C1_BASE));
}

void i2cReadData(uint8_t addr, unsigned short *data) {
	uint8_t data_high, data_low;
    I2CMasterSlaveAddrSet(I2C1_BASE, addr, true);// Set to read mode
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);// Send start condition
    while (I2CMasterBusy(I2C1_BASE));
    data_high = I2CMasterDataGet(I2C1_BASE);
    I2CMasterControl(I2C1_BASE, I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
    while (I2CMasterBusy(I2C1_BASE));
    data_low = I2CMasterDataGet(I2C1_BASE);

    *data=data_high;
    *data=*data<<8;
    *data= *data | data_low;
    *data=*data/1.2;
}
void bh1750_init(void) {
	i2cWrite(BH1750_ADDR,POWER_OFF);
	i2cWrite(BH1750_ADDR,RESET);
	i2cWrite(BH1750_ADDR,POWER_ON);
	SysCtlDelay(SysCtlClockGet()/6);//delay 500ms
	i2cWrite(BH1750_ADDR,CONT_H_RESOL_MODE);
}
