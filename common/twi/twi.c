#include "twi.h"
#include <stdio.h>
#include "boards.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "nrf_drv_twi.h"


/* TWI instance ID. */
#define TWI_INSTANCE_ID     0

 /* Number of possible TWI addresses. */
 #define TWI_ADDRESSES      127

/* TWI instance. */
static const nrf_drv_twi_t m_twi = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/**
 * @see twi.h
 */
void twi_init(void)
{
    ret_code_t err_code;
	
    const nrf_drv_twi_config_t twi_config = {
       .scl                = SENSOR_TWI_SCL,
       .sda                = SENSOR_TWI_SDA,
       .frequency          = NRF_DRV_TWI_FREQ_400K,
       .interrupt_priority = APP_IRQ_PRIORITY_HIGH,
       .clear_bus_init     = false
    };

    err_code = nrf_drv_twi_init(&m_twi, &twi_config, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    nrf_drv_twi_enable(&m_twi);
}

/**
 * @see twi.h
 */
int twi_scan(void)
{
	ret_code_t err_code;
    uint8_t address;
    uint8_t sample_data;

    for (address = 1; address <= TWI_ADDRESSES; address++)
    {
        err_code = nrf_drv_twi_rx(&m_twi, address, &sample_data, sizeof(sample_data));
        if (err_code == NRF_SUCCESS)
        {
			return address;
        }
    }
		return -1;
	
}

/**
 * @see twi.h
 */
int twi_write(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char const *data) {	
	ret_code_t err_code;
	
	/* Probably a better way to do this */
	unsigned char out[length+1];
	out[0] = reg_addr;
	memcpy(&out[1],data,length);
		
	err_code = nrf_drv_twi_tx(&m_twi, slave_addr, out, length+1, false);
	if (err_code != NRF_SUCCESS) return err_code;  
	
	return NRF_SUCCESS;
}

/**
 * @see twi.h
 */
int twi_read(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data) {
	ret_code_t err_code;
		
	err_code = nrf_drv_twi_tx(&m_twi, slave_addr, (const uint8_t*) &reg_addr, 1, false);
	if (err_code != NRF_SUCCESS) return err_code;  
	
	err_code = nrf_drv_twi_rx(&m_twi, slave_addr, data, length);							
	if (err_code != NRF_SUCCESS) return err_code;
  	
	return NRF_SUCCESS;
}
