/**
 * @file MPU6050.c
 * @date Feb 20, 2024
 * @author @s-grudner
 *
 */

#include <mpu6050.h>

 // ------------------------------------------------------------
 // Calculation Constants and Formulas
 // ------------------------------------------------------------

#define TEMP_REG_TO_C(x) ((x)/340 + 36.53f)

#define GYRO_LSB_SENSE_250DPS  (131.0f)
#define GYRO_LSB_SENSE_500DPS   (65.5f)
#define GYRO_LSB_SENSE_1000DPS  (32.8f)
#define GYRO_LSB_SENSE_2000DPS  (16.4f)

#define	ACCEL_LSB_SENSE_2G  (1<<14)
#define	ACCEL_LSB_SENSE_4G  (1<<13)
#define	ACCEL_LSB_SENSE_8G  (1<<12)
#define	ACCEL_LSB_SENSE_16G (1<<11)

// ------------------------------------------------------------
// Opaque Handle Structure Type Implementation
// ------------------------------------------------------------

struct __mpu6050_handle_s {
	HAL_StatusTypeDef err;
	uint8_t data_ready;

	mpu6050_config_t cfg;
	float gyro_lsb_sens;
	float accel_lsb_sens;

	float_vector3_t accel; // in units of g (9.81mps2)
	float_vector3_t gyro;  // in °/s
	float temp;
};
typedef struct __mpu6050_handle_s __mpu6050_handle_s;

// ------------------------------------------------------------
// INITIALISATION
// ------------------------------------------------------------

HAL_StatusTypeDef MPU6050_Init(__mpu6050_handle_s** dev, mpu6050_config_t* dev_cfg)
{
	// allocate handle and set to 0
	__mpu6050_handle_s* init = (__mpu6050_handle_s*)calloc(1, sizeof(__mpu6050_handle_s));
	if (init == NULL) return HAL_ERROR; // out of memory

	init->cfg = *dev_cfg;

	// device check
	uint8_t who_am_i;
	HAL_StatusTypeDef err = MPU6050_ReadReg(init, MPU6050_REG_WHO_AM_I, &who_am_i);

	if (err != HAL_OK) return err; // Failed to perform i2c transmission
	if (who_am_i != MPU6050_DEV_ID) return HAL_ERROR; // wrong device

	// Configure Interrupt
	MPU6050_WriteReg(init, INT_ENABLE, DATA_RDY_EN);

	// Configure Sensitivity
	MPU6050_SetAccelRange(init, dev_cfg->accel_range);
	MPU6050_SetGyroRange(init, dev_cfg->gyro_range);

	// Wake Up Device
	MPU6050_WriteReg(init, MPU6050_REG_PWR_MGMT_1, 0x00);

	*dev = init; // finally point to outer device handle
	return (*dev)->err;
}

void MPU6050_Exit(mpu6050_handle_t dev)
{
	HAL_I2C_DeInit(dev->cfg.hi2c);
	free(dev);
}

// ------------------------------------------------------------
// CONFIG
// ------------------------------------------------------------

HAL_StatusTypeDef MPU6050_SetAccelRange(mpu6050_handle_t dev, mpu6050_accel_range_t accel_range)
{
	dev->err = MPU6050_WriteReg(dev, ACCEL_CONFIG, accel_range << 3);
	if (dev->err != HAL_OK) return dev->err;
	dev->accel_lsb_sens = (float)(1 << (14 - accel_range));
	return dev->err;
}

HAL_StatusTypeDef MPU6050_SetGyroRange(mpu6050_handle_t dev, mpu6050_gyro_range_t gyro_range)
{
	dev->err = MPU6050_WriteReg(dev, GYRO_CONFIG, gyro_range << 3);
	if (dev->err != HAL_OK) return dev->err;
	float lsb[4] = {
		GYRO_LSB_SENSE_250DPS,
		GYRO_LSB_SENSE_500DPS,
		GYRO_LSB_SENSE_1000DPS,
		GYRO_LSB_SENSE_2000DPS,
	};
	dev->gyro_lsb_sens = lsb[gyro_range];
	return dev->err;
}

// Return decoded accelerometer data (2s Complement)
float_vector3_t MPU6050_ReadAcceleration(mpu6050_handle_t dev)
{
	if (!dev->data_ready) return dev->accel;

	// perform burst read from ACCEL_XOUT Register
	uint8_t reg_data[ACCEL_BURST_SIZE]; // Contains split raw data registers
	dev->err = MPU6050_ReadRegBurst(dev, ACCEL_OUT, reg_data, ACCEL_BURST_SIZE);

	// return old data. error should be checked in user application.
	if (dev->err != HAL_OK) return dev->accel;

	int16_t sgn_data[ACCEL_BURST_SIZE >> 1]; // contains merged registers with signs

	// 2*i (even) are H Registers, 2*i+1 (odd) are L Registers
	for (int i = 0; i < ACCEL_BURST_SIZE >> 2; ++i)
	{
		sgn_data[i] = (reg_data[2 * i] << 8 | reg_data[2 * i + 1]);
	}
	dev->accel.x = (float)sgn_data[0] / dev->accel_lsb_sens;
	dev->accel.y = (float)sgn_data[1] / dev->accel_lsb_sens;
	dev->accel.z = (float)sgn_data[2] / dev->accel_lsb_sens;

	// reset interrupt flag
	MPU6050_ResetIntrFlag(dev);
	return dev->accel;
}

// Return decoded gyroscope data (2s Complement)
// float_vector3_t MPU6050_ReadGyroscope(mpu6050_handle_t dev);

// Return decoded gyroscope data (Formula [RM p. 30])
// float MPU6050_ReadTemperatureC(mpu6050_handle_t dev);

// ------------------------------------------------------------
// Low Level Functions
// ------------------------------------------------------------

uint8_t MPU6050_DataReady(mpu6050_handle_t dev)
{
	return dev->data_ready;
}

void MPU6050_SetIntrFlag(mpu6050_handle_t dev)
{
	dev->data_ready = 1;
}

void MPU6050_ResetIntrFlag(mpu6050_handle_t dev)
{
	dev->data_ready = 0;
}

HAL_StatusTypeDef MPU6050_ReadReg(mpu6050_handle_t dev, uint8_t reg, uint8_t* data)
{
	return HAL_I2C_Mem_Read(dev->cfg.hi2c, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MPU6050_ReadRegBurst(mpu6050_handle_t dev, uint8_t reg, uint8_t* data, uint8_t len)
{
	return HAL_I2C_Mem_Read(dev->cfg.hi2c, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, data, len, HAL_MAX_DELAY);
}

HAL_StatusTypeDef MPU6050_WriteReg(mpu6050_handle_t dev, uint8_t reg, uint8_t data)
{
	return HAL_I2C_Mem_Write(dev->cfg.hi2c, MPU6050_I2C_ADDR, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
}
