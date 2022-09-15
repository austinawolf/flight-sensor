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

typedef enum
{
    SAMPLE_FLAG_GYRO = (1 << 0),
    SAMPLE_FLAG_ACCEL = (1 << 1),
    SAMPLE_FLAG_COMPASS = (1 << 2),
    SAMPLE_FLAG_QUAT = (1 << 3),
    SAMPLE_FLAG_TIMESTAMP = (1 << 4),
} imu_sample_flags_e;

typedef struct
{
	uint32_t timestamp;
	int16_t gyro[3];
    int16_t accel[3];
    int16_t compass[3];
	int32_t quat[4];
	imu_sample_flags_e flags;	
} imu_sample_t;

typedef void (*imu_event_callback_t) (void);

typedef struct
{
    imu_sample_rate_e rate;
    imu_sample_flags_e flags;
} imu_config_t;


status_e imu_create(void);

status_e imu_start(imu_config_t *config);

status_e imu_stop(void);

status_e imu_sample_read(imu_sample_t *sample, bool *sample_ready);

status_e imu_calibrate(void);

status_e imu_register_callback(imu_event_callback_t callback);

#endif
