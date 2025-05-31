/**
 * @file MPU6050.h
 * @date Feb 20, 2024
 * @author @s-grundner
 *
 * [DS] datasheet: https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf
 * [RM] register map: https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Register-Map1.pdf
 */

#ifndef INC_MPU6050_H_
#define INC_MPU6050_H_

#include "mpu6050_regmap.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include <stdlib.h>

 // ------------------------------------------------------------
 // PARAMETERS
 // ------------------------------------------------------------

#define MPU6050_AD0 0
#define MPU6050_DEV_ID   (0x68)
#define MPU6050_I2C_ADDR ((MPU6050_DEV_ID | MPU6050_AD0) << 1)

// ------------------------------------------------------------
// Enumeration Types
// ------------------------------------------------------------

typedef enum {
	MPU6050_ACCEL_RANGE_2G = 0, // 16384 LSB/g
	MPU6050_ACCEL_RANGE_4G,		//  8192 LSB/g
	MPU6050_ACCEL_RANGE_8G,		//  4096 LSB/g
	MPU6050_ACCEL_RANGE_16G,	//  2048 LSB/g
} mpu6050_accel_range_t;

typedef enum {
	MPU6050_GYRO_RANGE_250DPS = 0,	// 131  LSB/°/s
	MPU6050_GYRO_RANGE_500DPS,		// 65.5 LSB/°/s
	MPU6050_GYRO_RANGE_1000DPS,		// 32.8 LSB/°/s
	MPU6050_GYRO_RANGE_2000DPS,		// 16.4 LSB/°/s
} mpu6050_gyro_range_t;

// ------------------------------------------------------------
// Structure Types
// ------------------------------------------------------------

typedef struct {
	float x;
	float y;
	float z;
} float_vector3_t;

typedef struct {
	I2C_HandleTypeDef* hi2c;
	mpu6050_accel_range_t accel_range;
	mpu6050_gyro_range_t gyro_range;
} mpu6050_config_t;

// Opaque incomplete handle structure type
typedef struct __mpu6050_handle_s* mpu6050_handle_t;

// ------------------------------------------------------------
// Configuration Functions
// ------------------------------------------------------------

HAL_StatusTypeDef MPU6050_Init(mpu6050_handle_t* dev, mpu6050_config_t* dev_cfg);
void              MPU6050_Exit(mpu6050_handle_t dev);
HAL_StatusTypeDef MPU6050_SetAccelRange(mpu6050_handle_t dev, mpu6050_accel_range_t accel_range);
HAL_StatusTypeDef MPU6050_SetGyroRange(mpu6050_handle_t dev, mpu6050_gyro_range_t gyro_range);

// ------------------------------------------------------------
// Get Stuff
// ------------------------------------------------------------

float_vector3_t MPU6050_ReadAcceleration(mpu6050_handle_t dev);
float_vector3_t MPU6050_ReadGyroscope(mpu6050_handle_t dev);
float           MPU6050_ReadTemperatureC(mpu6050_handle_t dev);

// ------------------------------------------------------------
// Low Level Functions
// ------------------------------------------------------------

uint8_t MPU6050_DataReady(mpu6050_handle_t dev);
void    MPU6050_SetIntrFlag(mpu6050_handle_t dev);
void    MPU6050_ResetIntrFlag(mpu6050_handle_t dev);

HAL_StatusTypeDef MPU6050_ReadRegBurst(mpu6050_handle_t dev, uint8_t reg, uint8_t* data, uint8_t len);
HAL_StatusTypeDef MPU6050_ReadReg(mpu6050_handle_t dev, uint8_t reg, uint8_t* data);
HAL_StatusTypeDef MPU6050_WriteReg(mpu6050_handle_t dev, uint8_t reg, uint8_t data);

#endif /* INC_MPU6050_H_ */
