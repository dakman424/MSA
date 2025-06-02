/*
 * FM24CL16.h
 *
 *  Created on: Jan 21, 2025
 *      Author: knap-linux
 */

#ifndef FM24CL16B_H_
#define FM24CL16B_H_
#include "stdint.h"
#include "stm32f4xx_hal.h"
#ifdef __cplusplus
extern "C" {
#endif

#define FM24CL16B_VERSION "0.2.1"
#define FM24CL16B_DEFAULT_TIMEOUT (uint32_t)100 // ms
#define FM24CL16B_INTERRUPT 0
#define FM24CL16B_MAX_ADDRESS (uint32_t)2047 // bytes
#define FM24CL16B_BUFFLEN_RX 64
#define FM24CL16B_BUFFLEN_TX 64
#define FM24CL16B_WRITE_LEN (uint32_t)32 // must be smaller than bufflen
#define FM24CL16B_PRINT 0
#define FM24CL16B_CMSIS_OS2 0
#if FM24CL16B_PRINT == 1
#define FM24CL16B_TIMEOUT_UART (uint32_t)100 // ms
#endif

#if FM24CL16B_CMSIS_OS2 == 1
#include "cmsis_os.h"
#endif

typedef enum
{
	FM24CL16B_NONE = -1,
	FM24CL16B_OK = 0,
	FM24CL16B_ERROR_INIT,
	FM24CL16B_ERROR_I2C_ADDRESS,
	FM24CL16B_ERROR_ADDRESS,
	FM24CL16B_ERROR_TX,
	FM24CL16B_ERROR_RX,
	FM24CL16B_ERROR_UART,
	FM24CL16B_ERROR_TIMEOUT,
#if FM24CL16B_CMSIS_OS2 == 1
	FM24CL16B_ERROR_MUTEX
#endif
} FM24CL16B_State;

typedef struct FM24CL16B FM24CL16B;

struct FM24CL16B
{
	I2C_HandleTypeDef *m_hi2c;
	uint8_t m_init;
	uint8_t m_data_rx[FM24CL16B_BUFFLEN_RX];
	uint8_t m_data_tx[FM24CL16B_BUFFLEN_TX];
	uint32_t m_timeout;
#if FM24CL16B_CMSIS_OS2 == 1
	osMutexId_t *m_mutex_handle;
	uint32_t m_timeout_mutex;
#endif

	FM24CL16B_State (*deinit)(struct FM24CL16B *self);
	FM24CL16B_State (*write)(struct FM24CL16B *self, const uint32_t address, const uint8_t *data, const size_t len);
	FM24CL16B_State (*read)(struct FM24CL16B *self, const uint32_t address, uint8_t *data, const size_t len);
	FM24CL16B_State (*reset)(struct FM24CL16B *self, const uint8_t value);
#if FM24CL16B_PRINT == 1
	FM24CL16B_State (*print)(struct FM24CL16B *self, UART_HandleTypeDef *huart);
#endif
};

#if FM24CL16B_CMSIS_OS2 == 1
FM24CL16B_State FM24CL16B_init(struct FM24CL16B *self, I2C_HandleTypeDef *hi2c, osMutexId_t *mutex_handle);
#else
FM24CL16B_State FM24CL16B_init(struct FM24CL16B *self, I2C_HandleTypeDef *hi2c);
#endif

#ifdef __cplusplus
}
#endif

#endif /* FM24CL16B_H_ */