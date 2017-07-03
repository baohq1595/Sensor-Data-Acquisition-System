/*
 * BH1750.h
 *
 *  Created on: Oct 2, 2016
 *      Author: trannhanduc95
 */

#ifndef BH1750_BH1750_H_
#define BH1750_BH1750_H_

#define POWER_OFF			0x00
#define POWER_ON			0x01
#define RESET				0x07
#define BH1750_ADDR			0x23
#define BH1750_ADDR_WRITE	0x46
#define	BH1750_ADDR_READ	0x47
#define	CONT_H_RESOL_MODE	0x10


extern void initI2C(void);
extern void i2cWrite(uint8_t addr, uint8_t regAddr);
extern void i2cReadData(uint8_t addr, unsigned short *data);
extern void bh1750_init(void);

unsigned short data;



#endif /* BH1750_BH1750_H_ */
