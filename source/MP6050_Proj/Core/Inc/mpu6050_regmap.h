/*
 * mpu6050_regmap.h
 *
 *  Created on: Feb 21, 2024
 *      Author: Smon
 */

#ifndef INC_MPU6050_REGMAP_H_
#define INC_MPU6050_REGMAP_H_

// Configuration

#define GYRO_CONFIG (0x1B)
#define FS_SEL_MASK ((1<<3) | (1<<4))

#define ACCEL_CONFIG (0x1C)
#define AFS_SEL_MASK ((1<<3) | (1<<4))

// Interrupt

#define INT_PIN_CFG (0x37)
#define INT_ENABLE  (0x38) // 00111000
#define DATA_RDY_EN (1<<0)

// Sensor Registers

#define SENSOR_OUT (0x3B) // Starting register for burst read
#define SENSOR_BURST_SIZE (14)

#define ACCEL_BURST_SIZE (6)
#define ACCEL_OUT   (0x3B) // Starting register for burst read

#define TEMP_BURST_SIZE (2)
#define TEMP_OUT     (0x41) // Starting register for burst read

#define GYRO_BURST_SIZE (6)
#define GYRO_OUT     (0x43) // Starting register for burst read

// Power Management

#define MPU6050_REG_PWR_MGMT_1 (0x6B)

// I2C Specific

#define MPU6050_REG_WHO_AM_I (0x75)

#endif /* INC_MPU6050_REGMAP_H_ */
