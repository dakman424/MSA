/*
 * FM24CL16.c
 *
 *  Created on: Jan 21, 2025
 *      Author: knap-linux
 */

#include "FM24CL.h"
#include <string.h>

#if FM24CL16B_PRINT == 1
#include <stdio.h>
#endif

static const uint8_t s_device_address_write	= 0xA0;
static const uint8_t s_device_address_read	= 0xA1;

static FM24CL16B_State deinit(struct FM24CL16B *self);
static FM24CL16B_State write(struct FM24CL16B *self, const uint32_t address, const uint8_t *data, const size_t len);
static FM24CL16B_State read(struct FM24CL16B *self, const uint32_t address, uint8_t *data, const size_t len);
static FM24CL16B_State reset(struct FM24CL16B *self, const uint8_t value);
#if FM24CL16B_PRINT == 1
static FM24CL16B_State print(FM24CL16B *self, UART_HandleTypeDef *huart);
#endif

/**
 * @brief Initialize device object.
 *
 * @param self Device object pointer.
 * @param hi2c HAL I2C handle.
 * @param address Device I2C address.
 * @return FM24CL16B_State
 */
#if FM24CL16B_CMSIS_OS2 == 1
FM24CL16B_State FM24CL16B_init(struct FM24CL16B *self, I2C_HandleTypeDef *hi2c, osMutexId_t *mutex_handle)
#else
FM24CL16B_State FM24CL16B_init(struct FM24CL16B *self, I2C_HandleTypeDef *hi2c)
#endif
{
	self->m_hi2c = hi2c;
	self->m_timeout = FM24CL16B_DEFAULT_TIMEOUT;
#if FM24CL16B_CMSIS_OS2 == 1
	self->m_mutex_handle = mutex_handle;
	self->m_timeout_mutex = osWaitForever;
#endif
	self->deinit = deinit;
	self->write = write;
	self->read = read;
	self->reset = reset;
#if FM24CL16B_PRINT == 1
	self->print = print;
#endif
	self->m_init = 1;

	return FM24CL16B_OK;
}

/**
 * @brief Deinitialize device struct.
 *
 * @param self Device object pointer.
 * @return FM24CL16B_State
 */
static FM24CL16B_State deinit(struct FM24CL16B *self)
{
	self->m_hi2c = NULL;
	self->m_init = 0;
#if FM24CL16B_CMSIS_OS2 == 1
	self->m_mutex_handle = NULL;
#endif

	return FM24CL16B_OK;
}

/**
 * @brief Device write function. To write at selected address pass a buffer pointer and buffer length.
 *
 * @param self Device object pointer.
 * @param address Selected device address.
 * @param data Data to be written.
 * @param len Length of data to be written.
 * @return FM24CL16B_State
 */
static FM24CL16B_State write(struct FM24CL16B *self, const uint32_t address, const uint8_t *data, const size_t len)
{
	if (!self->m_init) return FM24CL16B_ERROR_INIT;
	if (address + len - 1 > FM24CL16B_MAX_ADDRESS) return FM24CL16B_ERROR_ADDRESS;

#if FM24CL16B_CMSIS_OS2 == 1
	if (osMutexAcquire(*self->m_mutex_handle, self->m_timeout_mutex) != osOK)
	{
		return FM24CL16B_ERROR_MUTEX;
	}
#endif

#if FM24CL16B_INTERRUPT == 1
	uint32_t timeout = HAL_GetTick() + self->m_timeout;
	while (self->m_hi2c->State != HAL_I2C_STATE_READY)
	{
		if (HAL_GetTick() > timeout)
		{
#if FM24CL16B_CMSIS_OS2 == 1
			osMutexRelease(*self->m_mutex_handle);
#endif
			return FM24CL16B_ERROR_TIMEOUT;
		}
	}
#endif

	uint8_t page = (address >> 8) & 0x07;
	uint8_t address_page_operation = s_device_address_write;
	address_page_operation |= (page << 1);

	self->m_data_tx[0] = (uint8_t)(address & 0xFF);

	memcpy(&self->m_data_tx[1], data, len);

#if FM24CL16B_INTERRUPT == 1
	if (HAL_I2C_Master_Transmit_IT(self->m_hi2c, address_page_operation, self->m_data_tx, len + 1) != HAL_OK)
#else
	if (HAL_I2C_Master_Transmit(self->m_hi2c, address_page_operation, self->m_data_tx, len + 1, self->m_timeout) != HAL_OK)
#endif
	{
#if FM24CL16B_CMSIS_OS2 == 1
		osMutexRelease(*self->m_mutex_handle);
#endif
		return FM24CL16B_ERROR_TX;
	}

#if FM24CL16B_CMSIS_OS2 == 1
	if (osMutexRelease(*self->m_mutex_handle) != osOK)
	{
		return FM24CL16B_ERROR_MUTEX;
	}
#endif

	return FM24CL16B_OK;
}

/**
 * @brief Device read function. To read from selected address pass a buffer pointer and buffer length.
 *
 * @param self Device object pointer.
 * @param address Selected device address.
 * @param data Outout buffer.
 * @param len Length of data to be read.
 * @return FM24CL16B_State
 */
static FM24CL16B_State read(struct FM24CL16B *self, const uint32_t address, uint8_t *data, const size_t len)
{
	if (!self->m_init) return FM24CL16B_ERROR_INIT;
	if (address + len - 1 > FM24CL16B_MAX_ADDRESS) return FM24CL16B_ERROR_ADDRESS;

#if FM24CL16B_CMSIS_OS2 == 1
	if (osMutexAcquire(*self->m_mutex_handle, self->m_timeout_mutex) != osOK)
	{
		return FM24CL16B_ERROR_MUTEX;
	}
#endif

#if FM24CL16B_INTERRUPT == 1
	uint32_t timeout = HAL_GetTick() + self->m_timeout;
	while (self->m_hi2c->State != HAL_I2C_STATE_READY)
	{
		if (HAL_GetTick() > timeout)
		{
#if FM24CL16B_CMSIS_OS2 == 1
			osMutexRelease(*self->m_mutex_handle);
#endif
			return FM24CL16B_ERROR_TIMEOUT;
		}
	}
#endif

	uint8_t page = (address >> 8) & 0x07;
	uint8_t address_page_operation = s_device_address_read;
	address_page_operation |= (page << 1);

	self->m_data_tx[0] = (uint8_t)(address & 0xFF);

	if (HAL_I2C_Master_Transmit(self->m_hi2c, address_page_operation, self->m_data_tx, 1, self->m_timeout) != HAL_OK)
	{
#if FM24CL16B_CMSIS_OS2 == 1
		osMutexRelease(*self->m_mutex_handle);
#endif
		return FM24CL16B_ERROR_TX;
	}

	if (HAL_I2C_Master_Receive(self->m_hi2c, address_page_operation, self->m_data_rx, len, self->m_timeout) != HAL_OK)
	{
#if FM24CL16B_CMSIS_OS2 == 1
		osMutexRelease(*self->m_mutex_handle);
#endif
		return FM24CL16B_ERROR_RX;
	}

	memcpy(data, self->m_data_rx, len);

#if FM24CL16B_CMSIS_OS2 == 1
	if (osMutexRelease(*self->m_mutex_handle) != osOK)
	{
		return FM24CL16B_ERROR_MUTEX;
	}
#endif

	return FM24CL16B_OK;
}

/**
 * @brief Reset device state to a selected value.
 *
 * @param self Device object pointer.
 * @param value Value to be set.
 * @return FM24CL16B_State
 */
static FM24CL16B_State reset(struct FM24CL16B *self, const uint8_t value)
{
    if (!self->m_init) return FM24CL16B_ERROR_INIT;

#if FM24CL16B_CMSIS_OS2 == 1
	if (osMutexAcquire(*self->m_mutex_handle, self->m_timeout_mutex) != osOK)
	{
		return FM24CL16B_ERROR_MUTEX;
	}
#endif

    uint32_t write_len = FM24CL16B_WRITE_LEN;
    uint32_t page_size = FM24CL16B_WRITE_LEN;

    memset(&self->m_data_tx[1], value, write_len);

    for (uint32_t address = 0; address < FM24CL16B_MAX_ADDRESS; address += write_len)
    {
        uint32_t remaining_in_page = page_size - (address % page_size);
        uint32_t chunk_size = (write_len > remaining_in_page) ? remaining_in_page : write_len;

        uint8_t page = (address >> 8) & 0x07;
		uint8_t address_page_operation = s_device_address_write;
		address_page_operation |= (page << 1);

		self->m_data_tx[0] = (uint8_t)(address & 0xFF);

        if (HAL_I2C_Master_Transmit(self->m_hi2c, address_page_operation, self->m_data_tx, chunk_size + 1, self->m_timeout) != HAL_OK)
        {
#if FM24CL16B_CMSIS_OS2 == 1
			osMutexRelease(*self->m_mutex_handle);
#endif
            return FM24CL16B_ERROR_TX;
        }
    }

#if FM24CL16B_CMSIS_OS2 == 1
	if (osMutexRelease(*self->m_mutex_handle) != osOK)
	{
		return FM24CL16B_ERROR_MUTEX;
	}
#endif

    return FM24CL16B_OK;
}


#if FM24CL16B_PRINT == 1
/**
 * @brief Print device state.
 *
 * @param self Device object pointer.
 * @param huart HAL UART handle pointer.
 * @return FM24CL16B_State
 */
static FM24CL16B_State print(struct FM24CL16B *self, UART_HandleTypeDef *huart)
{
	if (!self->m_init) return FM24CL16B_ERROR_INIT;

#if FM24CL16B_CMSIS_OS2 == 1
	if (osMutexAcquire(*self->m_mutex_handle, self->m_timeout_mutex) != osOK)
	{
		return FM24CL16B_ERROR_MUTEX;
	}
#endif

	uint32_t read_len = 8;
	uint8_t print_buff[16];

	for (uint32_t address = 0; address < FM24CL16B_MAX_ADDRESS; address += read_len)
	{
		uint8_t page = (address >> 8) & 0x07;
		uint8_t address_page_operation = s_device_address_write;
		address_page_operation |= (page << 1);

		self->m_data_tx[0] = (uint8_t)(address & 0xFF);

		if (HAL_I2C_Master_Transmit(self->m_hi2c, address_page_operation, self->m_data_tx, 1, self->m_timeout) != HAL_OK)
		{
#if FM24CL16B_CMSIS_OS2 == 1
			osMutexRelease(*self->m_mutex_handle);
#endif
			return FM24CL16B_ERROR_TX;
		}

		if (HAL_I2C_Master_Receive(self->m_hi2c, address_page_operation, self->m_data_rx, read_len, self->m_timeout) != HAL_OK)
		{
#if FM24CL16B_CMSIS_OS2 == 1
			osMutexRelease(*self->m_mutex_handle);
#endif
			return FM24CL16B_ERROR_RX;
		}

		sprintf((char*)print_buff, "%04X:", (uint16_t)address);
		if (HAL_UART_Transmit(huart, print_buff, strlen((char*)print_buff), FM24CL16B_TIMEOUT_UART) != HAL_OK)
		{
#if FM24CL16B_CMSIS_OS2 == 1
			osMutexRelease(*self->m_mutex_handle);
#endif
			return FM24CL16B_ERROR_UART;
		}

		for (uint32_t byteCtr = 0; byteCtr < read_len; ++byteCtr)
		{
			sprintf((char*)print_buff, " %02X", self->m_data_rx[byteCtr]);
			if (HAL_UART_Transmit(huart, print_buff, strlen((char*)print_buff), FM24CL16B_TIMEOUT_UART) != HAL_OK)
			{
#if FM24CL16B_CMSIS_OS2 == 1
				osMutexRelease(*self->m_mutex_handle);
#endif
				return FM24CL16B_ERROR_UART;
			}
		}

		sprintf((char*)print_buff, "\r\n");
		if (HAL_UART_Transmit(huart, print_buff, strlen((char*)print_buff), FM24CL16B_TIMEOUT_UART) != HAL_OK)
		{
#if FM24CL16B_CMSIS_OS2 == 1
			osMutexRelease(*self->m_mutex_handle);
#endif
			return FM24CL16B_ERROR_UART;
		}
	}

#if FM24CL16B_CMSIS_OS2 == 1
	if (osMutexRelease(*self->m_mutex_handle) != osOK)
	{
		return FM24CL16B_ERROR_MUTEX;
	}
#endif

	return FM24CL16B_OK;
}
#endif