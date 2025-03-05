#include "../Inc/MPU6050.h"
#include "stm32f3xx_hal_conf.h"

// Запись в регистр
void MPU6050_WriteReg(MPU6050_t *mpu, uint8_t reg_addr, uint8_t data) {
    uint8_t tx_data[2] = {reg_addr, data};
    HAL_I2C_Master_Transmit(&hi2c1, mpu->device_address << 1, tx_data, 2, HAL_MAX_DELAY);
}

// Чтение из регистра
uint8_t MPU6050_ReadReg(MPU6050_t *mpu, uint8_t reg_addr) {
    uint8_t rx_data;
    HAL_I2C_Master_Transmit(&hi2c1, mpu->device_address << 1, &reg_addr, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c1, mpu->device_address << 1, &rx_data, 1, HAL_MAX_DELAY);
    return rx_data;
}

// Функция инициализации MPU6050
void MPU6050_Init(MPU6050_t *mpu) {
    // Установка адреса устройства в зависимости от состояния пина AD0
    mpu->device_address = MPU6050_ADDRESS_AD0_LOW; // По умолчанию AD0 = LOW

    // Шаг 1: Сброс устройства
    // Регистр PWR_MGMT_1 содержит бит DEVICE_RESET. 
    // Установка этого бита в 1 вызывает полный сброс внутренних регистров MPU6050
    MPU6050_WriteReg(mpu, PWR_MGMT_1, 0x80); // Установка бита DEVICE_RESET
    HAL_Delay(100); // Ждем завершения сброса

    // Шаг 2: Выбор источника тактирования (внутренний осциллятор 8 МГц)
    // Регистр PWR_MGMT_1 содержит биты [2:0] - CLKSEL для выбора источника тактирования
    MPU6050_WriteReg(mpu, PWR_MGMT_1, MPU6050_CLOCK_PLL_XGYRO); // CLKSEL = 1 (PLL с X гироскопом)

    // Шаг 3: Настройка частоты выборки (Sample Rate Divider)
    MPU6050_WriteReg(mpu, SMPLRT_DIV, 0x07); // Делитель = 7 -> Sample Rate = 1kHz / (1 + 7) = 125 Hz

    // Шаг 4: Настройка фильтра низких частот (DLPF_CFG)
    MPU6050_WriteReg(mpu, CONFIG, MPU6050_DLPF_3); // DLPF_CFG = 3 -> Частота среза = 42 Гц

    // Шаг 5: Настройка чувствительности гироскопа
    MPU6050_WriteReg(mpu, GYRO_CONFIG, GYRO_SCALE_250DPS); // ±250°/s

    // Шаг 6: Настройка чувствительности акселерометра
    MPU6050_WriteReg(mpu, ACCEL_CONFIG, ACCEL_RANGE_2G); // ±2g

    // Шаг 7: Включение режима работы
    MPU6050_WriteReg(mpu, PWR_MGMT_1, MPU6050_CLOCK_PLL_XGYRO); // Отключаем режим сна
}

// Функция проверки идентичности MPU6050
uint8_t MPU6050_CheckID(MPU6050_t *mpu) {
    uint8_t id = MPU6050_ReadReg(mpu, WHO_AM_I);
    if (id == 0x68) { // Значение WHO_AM_I для MPU6050
        return 1; // Успешная идентификация
    }
    return 0; // Ошибка идентификации
}

// Чтение данных с датчика температуры
int16_t MPU6050_ReadTemperature(MPU6050_t *mpu) {
    uint8_t temp_high = MPU6050_ReadReg(mpu, 0x41); // TEMP_OUT_H
    uint8_t temp_low = MPU6050_ReadReg(mpu, 0x42);  // TEMP_OUT_L
    int16_t temp = (temp_high << 8) | temp_low;
    return temp;
}

// Чтение данных с акселерометра
void MPU6050_ReadAccel(MPU6050_t *mpu, int16_t *accel_x, int16_t *accel_y, int16_t *accel_z) {
    uint8_t buffer[6];
    HAL_I2C_Master_Transmit(&hi2c1, mpu->device_address << 1, &0x3B, 1, HAL_MAX_DELAY); // Начальный адрес ACCEL_XOUT_H
    HAL_I2C_Master_Receive(&hi2c1, mpu->device_address << 1, buffer, 6, HAL_MAX_DELAY);

    *accel_x = (buffer[0] << 8) | buffer[1];
    *accel_y = (buffer[2] << 8) | buffer[3];
    *accel_z = (buffer[4] << 8) | buffer[5];
}

// Чтение данных с гироскопа
void MPU6050_ReadGyro(MPU6050_t *mpu, int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z) {
    uint8_t buffer[6];
    HAL_I2C_Master_Transmit(&hi2c1, mpu->device_address << 1, &0x43, 1, HAL_MAX_DELAY); // Начальный адрес GYRO_XOUT_H
    HAL_I2C_Master_Receive(&hi2c1, mpu->device_address << 1, buffer, 6, HAL_MAX_DELAY);

    *gyro_x = (buffer[0] << 8) | buffer[1];
    *gyro_y = (buffer[2] << 8) | buffer[3];
    *gyro_z = (buffer[4] << 8) | buffer[5];
}