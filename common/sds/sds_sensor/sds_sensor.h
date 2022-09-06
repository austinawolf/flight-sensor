#ifndef SDS_SENSOR_H_
#define SDS_SENSOR_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "sds_twi.h"
#include "sds_config.h"
#include "sdk_common.h"
#include "sds_motion_types.h"
#include "sds_error.h"

typedef uint32_t rate_t;

void sds_sensor_init(motion_data_flags_t data_flags, void (*motion_callback) (void * p_context));
void sds_sensor_start(void);
void sds_sensor_stop(void);
void sds_sensor_set_rate(uint32_t sensor_rate);
void sds_sensor_get_sample(motion_sample_t * motion_sample);
sds_return_t sds_sensor_calibrate(void);

#endif
