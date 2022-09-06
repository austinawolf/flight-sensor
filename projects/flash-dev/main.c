#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "logger.h"
#include "status.h"
#include "flash.h"
#include "sample_store.h"
#include "sdk_config.h"
#include "nrf_delay.h"


/**
 * @brief 
 * 
 */
typedef struct
{
    uint32_t timestamp;
    int16_t accel[3];
    int16_t gyro[3];
    uint16_t compass[3];
    int32_t quat[4];
} imu_sample_t;



static sample_store_t store = 
{
    .start = 0u,
    .len = ONE_MB,
    .sample_size = sizeof(imu_sample_t),
};

int main(void)
{
    status_e status;

    LOG_INFO("Flash Dev Project Started");

    status = logger_create();
    ASSERT_STATUS(status);

    status = flash_create();
    ASSERT_STATUS(status);

    status = sample_store_create(&store);
    ASSERT_STATUS(status);

    status = sample_store_reset(&store);
    ASSERT_STATUS(status);

    for (int i = 0; i < 210; i++)
    {
        imu_sample_t sample = 
        {
            .timestamp = 0xAAAAAAAA,
            .accel = {1, 2, 3},
            .gyro = {4, 5, 6},
            .compass = {7, 8, 9},
            .quat = {10, 11, 12, 13},
        };

        status_e status = sample_store_append(&store, &sample, sizeof(sample));
        ASSERT_STATUS(status);

        //LOG_INFO("Sample append: %d, status: %d", i, status);
        LOG_FLUSH();
        nrf_delay_ms(1);
    }

    for (int i = 0; i < 20; i++)
    {
        imu_sample_t sample = {0}; 

        status_e status = sample_store_read(&store, 0, &sample, sizeof(sample), 1);
        ASSERT_STATUS(status);
        
        bool is_busy = true;
        while(is_busy)
        {
            flash_is_busy(&is_busy);
        }

        if (sample.timestamp != 0xAAAAAAAA)
        {
            LOG_ERROR("invalid timestamp: 0x%x", sample.timestamp);
            break;
        }

        //LOG_INFO("Sample Read: 0x%x, status: %d", sample.timestamp, status);
        LOG_FLUSH();
    }

    LOG_INFO("DONE");

    while (true)
    {
        LOG_FLUSH();
    }
}

/** @} */
