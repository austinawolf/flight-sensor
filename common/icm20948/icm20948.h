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

typedef void (*icm20948_event_callback_t)(void);

status_e icm20948_create(void);

status_e icm20948_start(void);

status_e icm20948_stop(void);

status_e icm20948_set_rate(uint16_t rate);

status_e icm20948_read(icm20948_data_t *data, bool *sample_ready);

status_e icm20948_calibrate(void);

status_e icm20948_register_event_callback(icm20948_event_callback_t callback);

#endif
