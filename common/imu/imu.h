/**
 * @file    logger.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef IMU_H_
#define IMU_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"

/**
 * @brief 
 * 
 */
typedef enum
{
    SAMPLE_RATE_1_HZ,
    SAMPLE_RATE_5_HZ,
    SAMPLE_RATE_10_HZ,
    SAMPLE_RATE_20_HZ,
    SAMPLE_RATE_50_HZ,
    SAMPLE_RATE_100_HZ,
    SAMPLE_RATE_MAX_VALUE,
} imu_sample_rate_e;

/**
 * @brief 
 * 
 */
typedef enum
{
    SAMPLE_FLAG_GYRO = (1 << 0),
    SAMPLE_FLAG_ACCEL = (1 << 1),
    SAMPLE_FLAG_COMPASS = (1 << 2),
    SAMPLE_FLAG_QUAT = (1 << 3),
    SAMPLE_FLAG_TIMESTAMP = (1 << 4),
} imu_sample_flags_e;

/**
 * @brief 
 * 
 */
typedef struct
{
	uint32_t timestamp;
	int16_t gyro[3];
    int16_t accel[3];
    int16_t compass[3];
	int32_t quat[4];
	imu_sample_flags_e flags;	
} imu_sample_t;

/**
 * @brief 
 * 
 */
typedef void (*imu_sample_callback_t) (imu_sample_t *sample);

/**
 * @brief 
 * 
 */
typedef struct
{
    imu_sample_rate_e rate;
    imu_sample_flags_e flags;
} imu_config_t;

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e imu_create(void);

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e imu_start(imu_config_t *config);

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e imu_stop(void);

/**
 * @brief 
 * 
 */
status_e imu_sample_read(imu_sample_t *sample);

/**
 * @brief 
 * 
 */
status_e imu_register_sample_callback(imu_sample_callback_t callback);

#endif
