/**
 * @file    icm20948.c
 * @author  Austin Wolf
 * @brief
 */
#include "imu.h"
#include "icm20948.h"
#include "timestamp.h"
#include "boards.h"
#include "nrf_drv_twi.h"
#include "nrf_drv_gpiote.h"
#include "logger.h"
#include "inv_mems.h"
#include "logger.h"


/**
 * @brief Stores event callback, NULL if unused
 */
static icm20948_event_callback_t _event_callback = NULL;

/**
 * @brief Executes event callback, if registered
 */
static void _icm_event_callback(void)
{
	if (_event_callback != NULL)
	{
		_event_callback();
	}
}

/**
 * @see imu.h
 */
status_e imu_create(void)
{	
	icm20948_create();
	icm20948_register_event_callback(_icm_event_callback);

    return STATUS_OK;
}

/**
 * @see imu.h
 */
status_e imu_start(imu_config_t *config)
{   
	const uint16_t rates[SAMPLE_RATE_MAX_VALUE] = 
	{
		[SAMPLE_RATE_1_HZ] 		1,
		[SAMPLE_RATE_5_HZ] 		5,
		[SAMPLE_RATE_10_HZ] 	10,
		[SAMPLE_RATE_20_HZ] 	20,
		[SAMPLE_RATE_50_HZ] 	50,
		[SAMPLE_RATE_100_HZ] 	100,
	};

	icm20948_set_rate(rates[config->rate]);	
	
	icm20948_start();

    return STATUS_OK;
}

/**
 * @see imu.h
 */
status_e imu_stop(void)
{
	icm20948_stop();

    return STATUS_OK;
}

/**
 * @see imu.h
 */
status_e imu_sample_read(imu_sample_t *sample, bool *sample_ready)
{	icm20948_data_t data = {0};
	icm20948_read(&data, sample_ready);

	sample->timestamp = timestamp_get();
	sample->flags |= SAMPLE_FLAG_TIMESTAMP;

	if (data.has_accel && data.has_gyro)
	{
		memcpy(sample->gyro, data.gyro, sizeof(sample->gyro));
		memcpy(sample->accel, data.accel, sizeof(sample->accel));
		sample->flags |= SAMPLE_FLAG_GYRO | SAMPLE_FLAG_ACCEL;
	}

	if (data.has_compass)
	{
		memcpy(sample->compass, data.compass, sizeof(sample->compass));
		sample->flags |= SAMPLE_FLAG_COMPASS;
	}

	if (data.has_quat)
	{
		memcpy(sample->quat, data.quat, sizeof(sample->quat));
		sample->flags |= SAMPLE_FLAG_QUAT;
	}

    return STATUS_OK;
}

/**
 * @see imu.h
 */
status_e imu_register_callback(imu_event_callback_t callback)
{
    _event_callback = callback;

    return STATUS_OK;
}

/**
 * @see imu.h
 */
status_e imu_calibrate(void)
{
	icm20948_calibrate();

	return STATUS_OK;
}
