#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "logger.h"
#include "status.h"
#include "flash.h"
#include "session_store.h"
#include "sdk_config.h"
#include "nrf_delay.h"

#define SAMPLES_TO_WRITE    (1000)
#define SAMPLES_TO_READ     (1000)


int main(void)
{
    status_e status;

    LOG_INFO("Flash Dev Project Started");

    status = logger_create();
    ASSERT_STATUS(status);

    status = flash_create();
    ASSERT_STATUS(status);

    status = session_store_create();
    ASSERT_STATUS(status);

    status = session_store_open();
    ASSERT_STATUS(status);

    while(session_store_is_busy())
    {

    }

    for (int i = 0; i < SAMPLES_TO_WRITE; i++)
    {
        imu_sample_t sample = 
        {
            .timestamp = 0xAAAAAAAA,
            .accel = {1, 2, 3},
            .gyro = {4, 5, 6},
            .compass = {7, 8, 9},
            .quat = {10, 11, 12, 13},
        };

        status_e status = session_store_append(&sample);
        ASSERT_STATUS(status);

        LOG_FLUSH();

        nrf_delay_ms(5);
    }

    status = session_store_close();
    ASSERT_STATUS(status);

    while(session_store_is_busy())
    {
        ;
    }

    for (int i = 0; i < SAMPLES_TO_READ; i++)
    {
        imu_sample_t sample = {0};

        status_e status = session_store_read(i, &sample);
        ASSERT_STATUS(status);

        if (sample.timestamp != 0xAAAAAAAA)
        {
            ASSERT_STATUS(-1);
        }
    }

    while (true)
    {
        LOG_INFO("DONE");
        LOG_FLUSH();
        nrf_delay_ms(5000);
    }
}

/** @} */
