#ifdef SENSOR_ICM20948

#include <stdio.h>
#include <string.h>
#include "nrf_delay.h"
#include "driver/inv_mems_hw_config.h"
#include "invn/common/invn_types.h"
#include "twi.h"
#include "timestamp.h"


/**
 * @brief 
 * 
 * @param reg 
 * @param length 
 * @param data 
 * @return int 
 */
int inv_serial_interface_write_hook(uint16_t reg, uint32_t length, uint8_t *data)
{
    int ret;
	ret = twi_write(ACCEL_GYRO_CHIP_ADDR, reg, length, data);
    return ret;
}

/**
 * @brief 
 * 
 * @param reg 
 * @param length 
 * @param data 
 * @return int 
 */
int inv_serial_interface_read_hook(uint16_t reg, uint32_t length, uint8_t *data)
{
	int ret;
	ret =  twi_read(ACCEL_GYRO_CHIP_ADDR, reg, length, data);
	return ret;
}

/**
 *  @brief  Sleep function.
**/
void inv_sleep(unsigned long mSecs)
{
	nrf_delay_ms(mSecs);
}

/**
 *  @brief  Sleep function.
**/
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
    return timestamp_get();
}

#endif
