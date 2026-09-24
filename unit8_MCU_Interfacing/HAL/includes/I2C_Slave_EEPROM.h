/*
 * I2C_Slave_EEPROM.h
 *
 *	Created on	: SEP, 24 2026
 *  Author: Mohamed aldremly
 */

#ifndef INCLUDES_I2C_SLAVE_EEPROM_H_
#define INCLUDES_I2C_SLAVE_EEPROM_H_

#include "Stm32_F103C6_I2C_driver.h"

//EEPROM is I2C Slave
//Idle mode: device is high impmedance state and wait for data
//Master Transmitter mode
//Master Receiver Mode

#define EEPROM_Slave_address	0x2A

void eeprom_init(void);
unsigned char eeprom_write_Nbytes(unsigned int Memory_address,unsigned char* bytes,uint8_t Data_Length);
unsigned char eeprom_read_byte(unsigned int Memory_address,uint8_t* dataOut,uint8_t data_Len);

#endif /* INCLUDES_I2C_SLAVE_EEPROM_H_ */
