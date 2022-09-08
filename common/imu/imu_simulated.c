/**
 * @file    logger.c
 * @author  Austin Wolf
 * @brief
 */
#include "imu.h"
#include "app_timer.h"
#include "timestamp.h"


APP_TIMER_DEF(m_sample_timer_id);


/**
 * @brief 
 * 
 */
static imu_sample_callback_t _sample_callback = NULL;

/**
 * @brief 
 * 
 */
static imu_sample_flags_e _flags = 0u;

/**
 * @brief 
 * 
 * @param p_context 
 */
static void sampling_timer_timeout_handler(void * p_context)
{
    (void) p_context;

    if (_sample_callback == NULL)
    {
        return;
    }

    imu_sample_t sample = 
    {
        .timestamp = timestamp_get(),
        .accel = {0, 1, 2},
        .gyro = {3, 4, 5},
        .compass = {6, 7, 8},
        .quat = {9, 10, 11, 12},
        .flags = _flags,
    };

    _sample_callback(&sample);
}

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e imu_create(void)
{
    uint32_t err_code = app_timer_create(&m_sample_timer_id,
                                            APP_TIMER_MODE_REPEATED,
                                            sampling_timer_timeout_handler);
    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e imu_start(imu_config_t *config)
{    
    const uint32_t sample_period_lookup[SAMPLE_RATE_MAX_VALUE] = 
    {
        [SAMPLE_RATE_1_HZ]      1000,
        [SAMPLE_RATE_5_HZ]      200,
        [SAMPLE_RATE_10_HZ]     100,
        [SAMPLE_RATE_20_HZ]     50,
        [SAMPLE_RATE_50_HZ]     20,
        [SAMPLE_RATE_100_HZ]    10,
    };

    _flags = config->flags;

    uint32_t timeout_ticks = APP_TIMER_TICKS(sample_period_lookup[config->rate]);
    uint32_t err_code = app_timer_start(m_sample_timer_id, timeout_ticks, NULL);
    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR;
    }
    
    return STATUS_OK;
}

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e imu_stop(void)
{
    uint32_t err_code = app_timer_stop(m_sample_timer_id);
    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

/**
 * @brief 
 * 
 */
status_e imu_sample_read(imu_sample_t *sample)
{
    sample->timestamp = timestamp_get();
    sample->accel[0] = 1;
    sample->accel[1] = 2;
    sample->accel[2] = 3;
    sample->gyro[0] = 4;
    sample->gyro[1] = 5;
    sample->gyro[2] = 6;
    sample->compass[0] = 7;
    sample->compass[1] = 8;
    sample->compass[2] = 9;
    sample->quat[0] = 10;
    sample->quat[1] = 11;
    sample->quat[2] = 12;
    sample->quat[3] = 13;
    sample->flags = _flags;

    return STATUS_OK;
}

/**
 * @brief 
 * 
 */
status_e imu_register_sample_callback(imu_sample_callback_t callback)
{
    _sample_callback = callback;

    return STATUS_OK;
}
