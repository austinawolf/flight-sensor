/**
 * @file    icm20948.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef ICM_20948_H_
#define ICM_20948_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"


/**
 * @brief Data generated by icm20948
 */
typedef struct
{
    bool has_accel;
    int16_t accel[3];
    int16_t accel_accuracy;
    bool has_gyro;
    int16_t gyro[3];
    int16_t gyro_accuracy; 
    bool has_compass;
    int16_t compass[3];
    int16_t compass_accuracy;
    bool has_quat;
    int32_t quat[4];
    int32_t quat_accuracy;
} icm20948_data_t;

/**
 * @brief Definitio of ICM20948 event callback
 */
typedef void (*icm20948_event_callback_t)(void);

/**
 * @brief Initializes the icm20948 driver
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e icm20948_create(void);

/**
 * @brief Starts sampling
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e icm20948_start(void);

/**
 * @brief Stops sampling
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e icm20948_stop(void);

/**
 * @brief Sets the sample rate of the icm20948
 * 
 * @param rate 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e icm20948_set_rate(uint16_t rate);

/**
 * @brief Reads a sample from buffer 
 * 
 * @param data pointer to store data
 * @param sample_ready true if sample ready, false otherwise
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e icm20948_read(icm20948_data_t *data, bool *sample_ready);

/**
 * @brief Runs the calibration
 * 
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e icm20948_calibrate(void);

/**
 * @brief Registers an event callback with the module
 * 
 * @param callback function to register
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e icm20948_register_event_callback(icm20948_event_callback_t callback);

#endif
