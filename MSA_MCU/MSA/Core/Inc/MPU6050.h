#ifndef _MPU6050_H_
#define _MPU6050_H_   
#include <stdint.h>

// Регистры MPU6050
#define SMPLRT_DIV        0x19
#define CONFIG            0x1A
#define GYRO_CONFIG       0x1B
#define ACCEL_CONFIG      0x1C
#define PWR_MGMT_1        0x6B
#define WHO_AM_I          0x75

typedef enum {
    MPU6050_ADDRESS_AD0_LOW = 0x68,
    MPU6050_ADDRESS_AD0_HIGH = 0x69
} mpu6050_addresses_t;

typedef enum {
    GYRO_SCALE_250DPS  = 0b00,
    GYRO_SCALE_500DPS  = 0b01,
    GYRO_SCALE_1000DPS = 0b10,
    GYRO_SCALE_2000DPS = 0b11
} gyro_scale_t;

typedef enum {
    ACCEL_RANGE_2G  = 0b00,
    ACCEL_RANGE_4G  = 0b01,
    ACCEL_RANGE_8G  = 0b10,
    ACCEL_RANGE_16G = 0b11
} accel_range_t;

typedef enum
{
    MPU6050_DELAY_3MS             = 0b11,
    MPU6050_DELAY_2MS             = 0b10,
    MPU6050_DELAY_1MS             = 0b01,
    MPU6050_NO_DELAY              = 0b00,
} mpu6050_onDelay_t;

typedef enum
{
    MPU6050_DHPF_HOLD             = 0b111,
    MPU6050_DHPF_0_63HZ           = 0b100,
    MPU6050_DHPF_1_25HZ           = 0b011,
    MPU6050_DHPF_2_5HZ            = 0b010,
    MPU6050_DHPF_5HZ              = 0b001,
    MPU6050_DHPF_RESET            = 0b000,
} mpu6050_dhpf_t;

typedef enum
{
    MPU6050_DLPF_6                = 0b110,
    MPU6050_DLPF_5                = 0b101,
    MPU6050_DLPF_4                = 0b100,
    MPU6050_DLPF_3                = 0b011,
    MPU6050_DLPF_2                = 0b010,
    MPU6050_DLPF_1                = 0b001,
    MPU6050_DLPF_0                = 0b000,
} mpu6050_dlpf_t;

typedef enum
{
    MPU6050_CLOCK_KEEP_RESET      = 0b111,
    MPU6050_CLOCK_EXTERNAL_19MHZ  = 0b101,
    MPU6050_CLOCK_EXTERNAL_32KHZ  = 0b100,
    MPU6050_CLOCK_PLL_ZGYRO       = 0b011,
    MPU6050_CLOCK_PLL_YGYRO       = 0b010,
    MPU6050_CLOCK_PLL_XGYRO       = 0b001,
    MPU6050_CLOCK_INTERNAL_8MHZ   = 0b000
} mpu6050_clockSource_t;


typedef struct {
    uint8_t device_address;
    uint8_t i2c_port;
} MPU6050_t;

void MPU6050_ReadReg(MPU6050_t *mpu, uint8_t reg_addr, uint8_t data);
uint8_t MPU6050_ReadReg(MPU6050_t *mpu, uint8_t reg_addr);
void MPU6050_Init(MPU6050_t *mpu);
uint8_t MPU6050_CheckID(MPU6050_t *mpu);
int16_t MPU6050_ReadTemperature(MPU6050_t *mpu);
void MPU6050_ReadAccel(MPU6050_t *mpu, int16_t *accel_x, int16_t *accel_y, int16_t *accel_z);
void MPU6050_ReadGyro(MPU6050_t *mpu, int16_t *gyro_x, int16_t *gyro_y, int16_t *gyro_z);


#endif /* _MPU6050_H_ */