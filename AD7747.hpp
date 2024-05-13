#ifndef __AD7747_HPP
#define __AD7747_HPP

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*	Include your STM32 HAL main.h header file here	*/
#include <main.h>

/*
 *
 * 	Analog Devices AD7747 Driver for STM32 MCU
 *
 * 	Include your board's HAL root header file in main.h
 *
 *
 */

/*	7-bit Device I2C Address = 0x48	*/
#define DEV_ADDR (0x48 << 1)

#define FULL_SCALE 8.192

/*	Registers	*/
#define STATUS 0x00
#define DATA_H 0x01
#define DATA_M 0x02
#define DATA_L 0x03
#define VT_DATA_H 0x04
#define VT_DATA_M 0x05
#define VT_DATA_L 0x06
#define CAP_SETUP 0x07
#define VT_SETUP 0x08
#define EXC_SETUP 0x09
#define CONFIG 0x0A
#define CAP_DAC_A 0x0B
#define CAP_DAC_B 0x0C
#define CAP_OFFSET_H 0x0D
#define CAP_OFFSET_L 0x0E
#define CAP_GAIN_H 0x0F
#define CAP_GAIN_L 0x10
#define VOLT_GAIN_H 0x11
#define VOLT_GAIN_L 0x12

/*	Setup Masks	*/
#define DATA_CAP_SETUP 0xB0
#define DATA_START_CALIB 0x80
#define DATA_VT_SETUP 0x81
#define DATA_EXC_SETUP 0x0E
#define DATA_CFG 0x01
#define DATA_CAPDAC_A 0x00
#define DATA_CAPDAC_B 0x00
#define DATA_CAP_OFFSET_H 0x80
#define DATA_CAP_OFFSET_L 0x00

/*	Data is ready mask	*/
#define DATA_IS_READY ((readRegister(hi2c,STATUS) & 0x08) == 0)

#define CAPDAC_ENABLE 0x80
#define CAPDAC_DISABLE 0x00

/*	Configuration masks	*/
#define CLEAR_CONV_MODE 0xF8

/*	Timeout in milliseconds	*/
#define DATA_READY_TIMEOUT 3000

typedef enum {
	A = 0x00,
	B = 0x01,
} CAPDAC_t;

typedef enum {
	IDLE = 0x00,
	CONTINUOUS = 0x01,
	SINGLE = 0x02
} ConversionMode_t;

class AD7747 {

public :

	AD7747();

	bool begin(I2C_HandleTypeDef *hi2c);

	/*
	 *
	 * 	readRegister
	 *
	 * 	Reads a byte from register with byte address 'reg'
	 *
	 *
	 */

	uint8_t readRegister(I2C_HandleTypeDef *hi2c, uint8_t reg);

	/*
	 *
	 * 	readRegisters
	 *
	 * 	Read multiple registers at once in succession, beginning at 'startReg'
	 * 	up until 'startReg' + 'amount', storing it in 'data'
	 *
	 *
	 */

	bool readRegisters(I2C_HandleTypeDef *hi2c, uint8_t startReg, uint8_t *data, uint8_t amount);

	/*
	 *
	 * 	writeRegister
	 *
	 * 	Write byte 'data' to register with byte address 'reg'
	 *
	 *
	 */

	bool writeRegister(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t data);

	/*
	 *
	 * 	reset
	 *
	 * 	Send 0xBF to the device to initiate a software reset
	 *
	 *
	 */

	bool reset(I2C_HandleTypeDef *hi2c);

	/*
	 *
	 *	startConversion
	 *
	 * 	Write to CONFIG register to initiate an ADC conversion
	 *
	 *
	 */

	bool startConversion(I2C_HandleTypeDef *hi2c);

	/*
	 *
	 * 	conversionMode
	 *
	 *
	 * 	returns current conversion mode (idle, single, or continuous)
	 *
	 */

	ConversionMode_t conversionMode(I2C_HandleTypeDef *hi2c);

	bool setConversionMode(I2C_HandleTypeDef *hi2c, ConversionMode_t MODE);

	/*
	 *
	 * 	setCAPDAC
	 *
	 * 	enables and sets the specifiedCAPDAC to the given value
	 *
	 */

	bool setCAPDAC(I2C_HandleTypeDef *hi2c, CAPDAC_t CAPDAC, float value);

	/*
	 *
	 * 	getCapacitance
	 *
	 * 	Returns float capacitance value in pF
	 *
	 * 	Error Codes:
	 * 		-1.0 = Error in conversion
	 * 		-2.0 = Timeout
	 * 		-3.0 = Error in reading
	 *
	 */

	float getCapacitance(I2C_HandleTypeDef *hi2c);

	/*
	 *
	 * 	getTempC
	 *
	 * 	Returns float temperature value in degC
	 *
	 * 	Error Codes:
	 * 		-128.0 = Error in conversion
	 *		-129.0 = Timeout
	 *		-130.0 = Error in reading
	 *
	 */

	float getTempC(I2C_HandleTypeDef *hi2c);

	float getTempF(I2C_HandleTypeDef *hi2c);

};


#endif /*	__AD7747_HPP	*/
