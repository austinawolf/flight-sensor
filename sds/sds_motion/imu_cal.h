#ifndef IMU_CAL_H_
#define IMU_CAL_H_

#include <stddef.h>
#include <stdio.h>
#include "sds_config.h"

/* IMU CONVERSIONS */
#define RAW_GYRO_TO_RADS (float) 2000.0f*2.0f/0xFFFFf * 3.14f/180.0f
#define RAW_ACCEL_TO_GS (float) 2.0f * 2.0f/0xFFFFf
	
//program biases
#define SCALE_ACC_OFFSET 1/8
#define SCALE_GYRO_OFFSET 2
#define RAW_1G_REFERENCE (uint16_t) 16383

#endif
