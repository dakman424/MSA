/******************************************************************************************
 * MS5611.c
 *
 * Created on: Dec 11, 2021
 * Author: Onur Karaman
******************************************************************************************/

#include "MS5611.h"
#include "stm32f4xx_hal.h"
#include "math.h"
#include <string.h>

extern I2C_HandleTypeDef hi2c1;

/******************************************************************************************
 ;	Function: 		MS5611_Reset
 ;	Description: 		Reset MS5611
 ; 	Inputs: 		Nothing
 ;	Returns:		Nothing
******************************************************************************************/
static void MS5611_Reset(void)
{
	uint8_t I2C_COMMAND[1] = {0};
	I2C_COMMAND[0] = MS5611_RESET;

	HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), I2C_COMMAND, sizeof(I2C_COMMAND), HAL_MAX_DELAY);
}

/******************************************************************************************
 ;	Function: 		MS5611_ReadCalibrationData
 ;	Description: 		Read Calibration Parameters from MS5611
 ; 	Inputs: 		Nothing
 ;	Returns:		Nothing
******************************************************************************************/
static void MS5611_ReadCalibrationData(void)
{
	uint8_t rx_Buffer[2] = {0};

	uint8_t I2C_COMMAND[6] = {0};
	I2C_COMMAND[0] = PROM_READ_C1;
	I2C_COMMAND[1] = PROM_READ_C2;
	I2C_COMMAND[2] = PROM_READ_C3;
	I2C_COMMAND[3] = PROM_READ_C4;
	I2C_COMMAND[4] = PROM_READ_C5;
	I2C_COMMAND[5] = PROM_READ_C6;

	HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[0], sizeof(I2C_COMMAND[0]), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
	CalibrationParameters.C1 = ((rx_Buffer[0] << 8) | (rx_Buffer[1]));

	HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[1], sizeof(I2C_COMMAND[1]), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
	CalibrationParameters.C2 = ((rx_Buffer[0] << 8) | (rx_Buffer[1]));

	HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[2], sizeof(I2C_COMMAND[2]), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
	CalibrationParameters.C3 = ((rx_Buffer[0] << 8) | (rx_Buffer[1]));

	HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[3], sizeof(I2C_COMMAND[3]), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
	CalibrationParameters.C4 = ((rx_Buffer[0] << 8) | (rx_Buffer[1]));

	HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[4], sizeof(I2C_COMMAND[4]), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
	CalibrationParameters.C5 = ((rx_Buffer[0] << 8) | (rx_Buffer[1]));

	HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[5], sizeof(I2C_COMMAND[5]), HAL_MAX_DELAY);
	HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
	CalibrationParameters.C6 = ((rx_Buffer[0] << 8) | (rx_Buffer[1]));
}

/******************************************************************************************
 ;	Function: 		MS5611_ReadDigitalValues
 ;	Description: 		Read Pressure and Temperature values from MS5611
 ; 	Inputs: 		Sampling Rate
 ;	Returns:		Nothing
******************************************************************************************/
static void MS5611_ReadDigitalValues(int osr)
{
	uint8_t rx_Buffer[3] = {0};

	uint8_t I2C_COMMAND[11] = {0};
	I2C_COMMAND[0] = D1_OSR_256;
	I2C_COMMAND[1] = D1_OSR_512;
	I2C_COMMAND[2] = D1_OSR_1024;
	I2C_COMMAND[3] = D1_OSR_2048;
	I2C_COMMAND[4] = D1_OSR_4096;
	I2C_COMMAND[5] = D2_OSR_256;
	I2C_COMMAND[6] = D2_OSR_512;
	I2C_COMMAND[7] = D2_OSR_1024;
	I2C_COMMAND[8] = D2_OSR_2048;
	I2C_COMMAND[9] = D2_OSR_4096;
	I2C_COMMAND[10] = ADC_READ;

	switch(osr)
	{
	case 256:
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[0], sizeof(I2C_COMMAND[0]), HAL_MAX_DELAY);
		HAL_Delay(50);
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[10], sizeof(I2C_COMMAND[10]), HAL_MAX_DELAY);
		HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
		DigitalValues.D1 = ((rx_Buffer[0] << 16) | (rx_Buffer[1] << 8) | (rx_Buffer[2]));

		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[5], sizeof(I2C_COMMAND[5]), HAL_MAX_DELAY);
		HAL_Delay(50);
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[10], sizeof(I2C_COMMAND[10]), HAL_MAX_DELAY);
		HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
		DigitalValues.D2 = ((rx_Buffer[0] << 16) | (rx_Buffer[1] << 8) | (rx_Buffer[2]));
		break;

	case 512:
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[1], sizeof(I2C_COMMAND[1]), HAL_MAX_DELAY);
		HAL_Delay(50);
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[10], sizeof(I2C_COMMAND[10]), HAL_MAX_DELAY);
		HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
		DigitalValues.D1 = ((rx_Buffer[0] << 16) | (rx_Buffer[1] << 8) | (rx_Buffer[2]));

		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[6], sizeof(I2C_COMMAND[6]), HAL_MAX_DELAY);
		HAL_Delay(50);
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[10], sizeof(I2C_COMMAND[10]), HAL_MAX_DELAY);
		HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
		DigitalValues.D2 = ((rx_Buffer[0] << 16) | (rx_Buffer[1] << 8) | (rx_Buffer[2]));
		break;

	case 1024:
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[2], sizeof(I2C_COMMAND[2]), HAL_MAX_DELAY);
		HAL_Delay(50);
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[10], sizeof(I2C_COMMAND[10]), HAL_MAX_DELAY);
		HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
		DigitalValues.D1 = ((rx_Buffer[0] << 16) | (rx_Buffer[1] << 8) | (rx_Buffer[2]));

		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[7], sizeof(I2C_COMMAND[7]), HAL_MAX_DELAY);
		HAL_Delay(50);
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[10], sizeof(I2C_COMMAND[10]), HAL_MAX_DELAY);
		HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
		DigitalValues.D2 = ((rx_Buffer[0] << 16) | (rx_Buffer[1] << 8) | (rx_Buffer[2]));
		break;

	case 2048:
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[3], sizeof(I2C_COMMAND[3]), HAL_MAX_DELAY);
		HAL_Delay(50);
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[10], sizeof(I2C_COMMAND[10]), HAL_MAX_DELAY);
		HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
		DigitalValues.D1 = ((rx_Buffer[0] << 16) | (rx_Buffer[1] << 8) | (rx_Buffer[2]));

		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[8], sizeof(I2C_COMMAND[8]), HAL_MAX_DELAY);
		HAL_Delay(50);
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[10], sizeof(I2C_COMMAND[10]), HAL_MAX_DELAY);
		HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
		DigitalValues.D2 = ((rx_Buffer[0] << 16) | (rx_Buffer[1] << 8) | (rx_Buffer[2]));
		break;

	case 4096:
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[4], sizeof(I2C_COMMAND[4]), HAL_MAX_DELAY);
		HAL_Delay(50);
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[10], sizeof(I2C_COMMAND[10]), HAL_MAX_DELAY);
		HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
		DigitalValues.D1 = ((rx_Buffer[0] << 16) | (rx_Buffer[1] << 8) | (rx_Buffer[2]));

		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[9], sizeof(I2C_COMMAND[9]), HAL_MAX_DELAY);
		HAL_Delay(50);
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[10], sizeof(I2C_COMMAND[10]), HAL_MAX_DELAY);
		HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
		DigitalValues.D2 = ((rx_Buffer[0] << 16) | (rx_Buffer[1] << 8) | (rx_Buffer[2]));
		break;

	default:
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[2], sizeof(I2C_COMMAND[2]), HAL_MAX_DELAY);
		HAL_Delay(50);
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[10], sizeof(I2C_COMMAND[10]), HAL_MAX_DELAY);
		HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
		DigitalValues.D1 = ((rx_Buffer[0] << 16) | (rx_Buffer[1] << 8) | (rx_Buffer[2]));

		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[7], sizeof(I2C_COMMAND[7]), HAL_MAX_DELAY);
		HAL_Delay(50);
		HAL_I2C_Master_Transmit(&hi2c1, (MS5611_DEVICE_ADDR<<1), &I2C_COMMAND[10], sizeof(I2C_COMMAND[10]), HAL_MAX_DELAY);
		HAL_I2C_Master_Receive(&hi2c1, (MS5611_DEVICE_ADDR<<1), rx_Buffer, sizeof(rx_Buffer), HAL_MAX_DELAY);
		DigitalValues.D2 = ((rx_Buffer[0] << 16) | (rx_Buffer[1] << 8) | (rx_Buffer[2]));
		break;
	}
}

/******************************************************************************************
 ;	Function: 		MS5611_DoCalculations
 ;	Description: 		Do Necessary Calculations
 ; 	Inputs: 		Nothing
 ;	Returns:		Nothing
******************************************************************************************/
static void MS5611_DoCalculations(void)
{
	/* For dT, OFF, and SENS max and min values should be defined */

	/* dT */
	CalculationParameters.dT = DigitalValues.D2 - (CalibrationParameters.C5 * pow(2,8));

	if(CalculationParameters.dT < -16776960)
		CalculationParameters.dT = -16776960;

	if(CalculationParameters.dT > 16777216)
		CalculationParameters.dT = 16777216;

	/* TEMP */
	CalculationParameters.TEMP = 2000 + (CalculationParameters.dT * CalibrationParameters.C6 /pow(2,26));

	/* OFF */
	CalculationParameters.OFF = (CalibrationParameters.C2 * pow(2,16)) +
			((CalibrationParameters.C4 * CalculationParameters.dT) / pow(2,7));

	if(CalculationParameters.OFF < -8589672450)
		CalculationParameters.OFF = -8589672450;

	if(CalculationParameters.OFF > 12884705280)
		CalculationParameters.OFF = 12884705280;

	/* SENS */
	CalculationParameters.SENS = (CalibrationParameters.C1 * pow(2,15)) +
			((CalibrationParameters.C3 * CalculationParameters.dT) / pow(2,8));

	if(CalculationParameters.SENS < -4294836225)
		CalculationParameters.SENS = -4294836225;

	if(CalculationParameters.SENS > 6442352640)
		CalculationParameters.SENS = 6442352640;

	/* P */
	CalculationParameters.P = ((DigitalValues.D1 * CalculationParameters.SENS / pow(2,21) - CalculationParameters.OFF)/ pow(2,15));
}

/******************************************************************************************
 ;	Function: 		MS5611_GetPressure
 ;	Description: 		Return Actual Pressure Value
 ; 	Inputs: 		Sampling Rate
 ;	Returns:		Actual Pressure Value
******************************************************************************************/
float MS5611_GetPressure(int osr)
{
	float actualPressure = 0;

	MS5611_ReadDigitalValues(osr);
	MS5611_DoCalculations();

	actualPressure = CalculationParameters.P / 100.00;

	return actualPressure;
}

/******************************************************************************************
 ;	Function: 		MS5611_GetTemperature
 ;	Description: 		Return Actual Temperature Value
 ; 	Inputs: 		Sampling Rate
 ;	Returns:		Actual Temperature Value
******************************************************************************************/
float MS5611_GetTemperature(int osr)
{
	float actualTemperature = 0;

	MS5611_ReadDigitalValues(osr);
	MS5611_DoCalculations();

	actualTemperature = CalculationParameters.TEMP / 100.00;

	return actualTemperature;
}

/******************************************************************************************
 ;	Function: 		MS5611_Init
 ;	Description: 		Initialize MS5611
 ; 	Inputs: 		Nothing
 ;	Returns:		Nothing
******************************************************************************************/
errcode  MS5611_Init(void)
{
	MS5611_Reset();
    for(uint16_t i = 0; i < 65535; i++);
	MS5611_ReadCalibrationData();
    if (!CalibrationParameters.C1 || !CalibrationParameters.C2 || !CalibrationParameters.C3 
    || !CalibrationParameters.C4 || !CalibrationParameters.C5 || !CalibrationParameters.C6) {
        return ERROR_ERROR;
    } else {
        return ERROR_OK;
    }
}

errcode MS5611_Service(void *buf, size_t size) {
    float temp = MS5611_GetTemperature(D1_OSR_256);
    float pressure = MS5611_GetPressure(D1_OSR_256);
    memcpy(buf, &temp, sizeof(float));
    memcpy((buf + sizeof(float)), &pressure, sizeof(float));
    return ERROR_OK;
}
errcode MS5611_Close() {
    return ERROR_ERROR;
}