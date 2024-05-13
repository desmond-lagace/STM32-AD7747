#include <AD7747.hpp>

uint8_t rawCapTempValue[6];
float capdac_A_value;
float capdac_B_value;

AD7747::AD7747() { }

bool AD7747::begin(I2C_HandleTypeDef *hi2c) {

	if ( !reset(hi2c) ) return false;
	HAL_Delay(5);
	// set-up CAPDACs
	writeRegister(hi2c,CAP_DAC_A,DATA_CAPDAC_A);
	writeRegister(hi2c,CAP_DAC_B,DATA_CAPDAC_B);
	// set-up cap offset registers
	writeRegister(hi2c,CAP_OFFSET_H,DATA_CAP_OFFSET_H);
	writeRegister(hi2c,CAP_OFFSET_L,DATA_CAP_OFFSET_L);
	// calibrate
	writeRegister(hi2c,CAP_SETUP,DATA_START_CALIB);
	// set-up cap register
	writeRegister(hi2c,CAP_SETUP,DATA_CAP_SETUP);
	// set-up temperature register
	writeRegister(hi2c,VT_SETUP,DATA_VT_SETUP);
	// set-up excitation register
	writeRegister(hi2c,EXC_SETUP,DATA_EXC_SETUP);
	// configure
	writeRegister(hi2c,CONFIG,DATA_CFG);
	return true;

}

bool AD7747::reset(I2C_HandleTypeDef *hi2c) {
	/*	Write reset code to device	*/
	uint8_t resetCode = 0xBF;
	if ( HAL_I2C_Master_Transmit(hi2c,DEV_ADDR,&resetCode,1,HAL_MAX_DELAY) != HAL_OK ) {
		return false;
	}
	return true;
}

bool AD7747::writeRegister(I2C_HandleTypeDef *hi2c, uint8_t reg, uint8_t data) {
	HAL_StatusTypeDef I2C_Status = HAL_I2C_Mem_Write(hi2c,DEV_ADDR,reg,I2C_MEMADD_SIZE_8BIT,&data,1,HAL_MAX_DELAY);
	HAL_Delay(5);
	if ( I2C_Status != HAL_OK ) {
		return false;
	}
	return true;
}

uint8_t AD7747::readRegister(I2C_HandleTypeDef *hi2c, uint8_t reg) {
	uint8_t rxBuffer = 0;
	HAL_StatusTypeDef I2C_Status = HAL_I2C_Mem_Read(hi2c,DEV_ADDR,reg,I2C_MEMADD_SIZE_8BIT,&rxBuffer,1,HAL_MAX_DELAY);
	if ( I2C_Status != HAL_OK ) {
		return 0xFF;
	}
	return rxBuffer;
}

bool AD7747::readRegisters(I2C_HandleTypeDef *hi2c, uint8_t startReg, uint8_t *buffer, uint8_t amount) {
	HAL_StatusTypeDef I2C_Status = HAL_ERROR;
	if ( startReg + amount < 0x13 ) {
		I2C_Status = HAL_I2C_Mem_Read(hi2c,DEV_ADDR,startReg,I2C_MEMADD_SIZE_8BIT,buffer,amount,HAL_MAX_DELAY);
	}
	if ( I2C_Status != HAL_OK ) {
		return false;
	}
	else {
		return true;
	}
}

bool AD7747::startConversion(I2C_HandleTypeDef *hi2c) {
	uint8_t value = readRegister(hi2c,CONFIG);
	value &= CLEAR_CONV_MODE;
	value |= 0x02;
	if ( !(writeRegister(hi2c,CONFIG,value)) ) {
		return false;
	}
	return true;
}

float AD7747::getCapacitance(I2C_HandleTypeDef *hi2c) {

	if ( conversionMode(hi2c) != CONTINUOUS ) {
		startConversion(hi2c);
	}

	uint32_t timer = HAL_GetTick();
	while ( !(DATA_IS_READY) ) {
		if ( HAL_GetTick() - timer >= DATA_READY_TIMEOUT ) {
			return -2.0;
		}
	}
	if ( !readRegisters(hi2c,DATA_H,rawCapTempValue,6) ) {
		return -3.0;
	}
	long rawCapValue = (long) ( ( ((long) rawCapTempValue[0] << 16) + ((long) rawCapTempValue[1] << 8) + ((long) rawCapTempValue[2] ))) - 0x800000;
	float realCapValue = (float) ( rawCapValue / 1024000.0 );
	return realCapValue;
}

float AD7747::getTempC(I2C_HandleTypeDef *hi2c) {

	if ( conversionMode(hi2c) != CONTINUOUS ) {
		startConversion(hi2c);
	}

	uint32_t timer = HAL_GetTick();
	while ( !((readRegister(hi2c,STATUS) & 0x08) == 0) ) {
		if ( HAL_GetTick() - timer >= DATA_READY_TIMEOUT ) {
			return -129.0;
		}
	}
	if ( !readRegisters(hi2c,DATA_H,rawCapTempValue,6) ) {
		return -130.0;
	}
	long rawTempValue = (long) ( ((long) rawCapTempValue[3] << 16) + ((long) rawCapTempValue[4] << 8 ) + ((long) rawCapTempValue[5]) );
	float realTempValue = (float) ( rawTempValue / 2048.0 - 4096.0 );
	return realTempValue;
}

float AD7747::getTempF(I2C_HandleTypeDef *hi2c) {

	return (getTempC(hi2c) * (9/5) + 32.0);

}

bool AD7747::setCAPDAC(I2C_HandleTypeDef *hi2c, CAPDAC_t CAPDAC, float value) {

	/*	find lsb (least significant bit) pF value	*/
	float lsb_value = FULL_SCALE / 32.0;
	/*	calculate capdac value by diving and rounding up	*/
	uint8_t capdac_value = (uint8_t) ceil(value / lsb_value);
	if ( capdac_value > 0x3F ) capdac_value = 0x3F;
	capdac_value |= 0x80; // enable

	if ( CAPDAC == A ) {
		if ( !(writeRegister(hi2c,CAP_DAC_A,capdac_value)) ) {
			capdac_A_value = capdac_value;
			return false;
		}
	}
	if ( CAPDAC == B ) {
		if ( !(writeRegister(hi2c,CAP_DAC_B,capdac_value)) ) {
			capdac_B_value = capdac_value;
			return false;
		}
	}
	return true;

}

ConversionMode_t AD7747::conversionMode(I2C_HandleTypeDef *hi2c) {

	ConversionMode_t mode = (ConversionMode_t) (readRegister(hi2c,CONFIG) & 0x03);
	return mode;

}

bool AD7747::setConversionMode(I2C_HandleTypeDef *hi2c, ConversionMode_t MODE) {

	if ( MODE < 0x00 || MODE > 0x02 ) return false;
	uint8_t cfg = readRegister(hi2c,CONFIG);
	cfg &= CLEAR_CONV_MODE;
	cfg |= MODE;
	if ( !writeRegister(hi2c,CONFIG,cfg) ) return false;
	return true;
}







