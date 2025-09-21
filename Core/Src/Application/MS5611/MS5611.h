/*****************************************************
 * MS5611.h
 *
 * Created on: Dec 11, 2021
 * Author: Onur Karaman
 *****************************************************/

#ifndef INC_MS5611_H_
#define INC_MS5611_H_

#include "stdint.h"
#include "common.h"
#include <stddef.h>

#define MS5611_DEVICE_ADDR	0x77

#define MS5611_RESET	0x1E

#define D1_OSR_256		0x40
#define D1_OSR_512 	 	0x42
#define D1_OSR_1024  	0x44
#define D1_OSR_2048 	0x46
#define D1_OSR_4096 	0x48

#define D2_OSR_256 	 	0x50
#define D2_OSR_512 	 	0x52
#define D2_OSR_1024 	0x54
#define D2_OSR_2048 	0x56
#define D2_OSR_4096	 	0x58

#define PROM_READ_C1  	0xA2
#define PROM_READ_C2  	0xA4
#define PROM_READ_C3  	0xA6
#define PROM_READ_C4  	0xA8
#define PROM_READ_C5  	0xAA
#define PROM_READ_C6  	0xAC

#define ADC_READ 	 	0x00

static struct calibrationParameters_t
{

	uint16_t C1;	// Pressure sensitivity | SENS(T1)
	uint16_t C2;	// Pressure offset | OFF(T1)
	uint16_t C3;	// Temperature coefficient of pressure sensitivity | TCS
	uint16_t C4;	// Temperature coefficient of pressure offset | TCO
	uint16_t C5;	// Reference temperature | T(REF)
	uint16_t C6;	// Temperature coefficient of the temperature | TEMPSENS

} CalibrationParameters;

static struct digitalValues_t
{

	uint32_t D1;	// Digital pressure value
	uint32_t D2;	// Digital temperature value

} DigitalValues;

static struct calculationParameters_t
{

	int32_t dT;		// Difference between actual and reference temperature
	int32_t TEMP;	// Actual temperature
	int64_t OFF;	// Offset at actual temperature
	int64_t SENS;	// Sensitivity at actual temperature
	int64_t P;		// Temperature compensated pressure
} CalculationParameters;

/* Function Prototypes */
errcode MS5611_Init(void);
float MS5611_GetTemperature(int osr);
float MS5611_GetPressure(int osr);

errcode MS5611_Service(void *buf, size_t size);
errcode MS5611_Close();

#endif /* INC_MS5611_H_ */