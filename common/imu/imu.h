/**
 * @file    imu.h
 * @author  Austin Wolf
 * @brief   Generic API for IMU drivers
 */

#ifndef IMU_H_
#define IMU_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"


/**
 * @brief Possible sample rates used by the driver
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
 * @brief Sample flags that indicate what data is contained in IMU sample
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
 * @brief Definition of an IMU sample
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
 * @brief Definition of callback to handle events from the driver
 */
typedef void (*imu_event_callback_t) (void);

/**
 * @brief Definition of IMU configuration structure used to start sampling
 */
typedef struct
{
    imu_sample_rate_e rate;
    imu_sample_flags_e flags;
} imu_config_t;

/**
 * @brief Initializes the device
 * 
 * @return status_e 
 */
status_e imu_create(void);

/**
 * @brief Starts motion sampling
 * 
 * @param config pointer to config structure
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e imu_start(imu_config_t *config);

/**
 * @brief Stops motion sampling
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e imu_stop(void);

/**
 * @brief Reads a sample from buffer
 * 
 * @param sample Sample to read
 * @param sample_ready true if another sample is ready, false otherwise
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e imu_sample_read(imu_sample_t *sample, bool *sample_ready);

/**
 * @brief Executes the device calibration/self-test routine
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e imu_calibrate(void);

/**
 * @brief Registers an event callback with the module
 * 
 * @param callback pointer to function
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e imu_register_callback(imu_event_callback_t callback);

#endif
