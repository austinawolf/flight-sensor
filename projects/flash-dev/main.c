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

    uint32_t write_count = 0;
    uint32_t read_count = 0;

    while (true)
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
        if (status == STATUS_ERROR_FLASH_FULL)
        {
            break;
        }
        else if (status != STATUS_OK)
        {
            APP_ERROR_CHECK(-1);
        }

        write_count++;

        LOG_FLUSH();
        nrf_delay_ms(5);
    }
    
    LOG_INFO("Write Count: %d", write_count);

    while(session_store_is_busy()) {}

    status = session_store_close();
    ASSERT_STATUS(status);

    while(session_store_is_busy()) {}

    while (true)
    {
        imu_sample_t sample = {0};

        status_e status = session_store_read(read_count, &sample);
        if (status == STATUS_ERROR_INVALID_PARAM)
        {
            break;
        }
        else if (status != STATUS_OK)
        {
            APP_ERROR_CHECK(-1);
        }

        if (sample.timestamp != 0xAAAAAAAA)
        {
            ASSERT_STATUS(-1);
        }

        read_count++;
    }

    LOG_INFO("Read Count: %d", read_count);

    while (true)
    {
        LOG_INFO("DONE");
        LOG_FLUSH();
        nrf_delay_ms(2000);
    }
}

/** @} */
