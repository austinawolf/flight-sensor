#ifdef SENSOR_ICM20948


#include <stdio.h>
#include <string.h>

#include "nrf_delay.h"

#include "driver/inv_mems_hw_config.h"
#include "invn/common/invn_types.h"

#include "sds_twi.h"


int inv_serial_interface_write_hook(uint16_t reg, uint32_t length, uint8_t *data)
{
    int ret;
	ret = sds_twi_write(ACCEL_GYRO_CHIP_ADDR, reg, length, data);
    return ret;
}

int inv_serial_interface_read_hook(uint16_t reg, uint32_t length, uint8_t *data)
{
	int ret;
	ret =  sds_twi_read(ACCEL_GYRO_CHIP_ADDR, reg, length, data);
	return ret;
}

/**
 *  @brief  Sleep function.
**/
void inv_sleep(unsigned long mSecs)
{
	nrf_delay_ms(mSecs);
}

void inv_sleep_100us(unsigned long nHowMany100MicroSecondsToSleep)
{
    (void)nHowMany100MicroSecondsToSleep;
    nrf_delay_ms(1);
}

/**
 *  @brief  get system's internal tick count.
 *          Used for time reference.
 *  @return current tick count.
**/
long inv_get_tick_count(void)
{
    unsigned long count;

    //get_tick_count(&count);
	count = 0;

    return (long)count;
}

#endif
