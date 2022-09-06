#ifndef TWI_INTERFACE_H_
#define TWI_INTERFACE_H_


#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "sdk_common.h"
#include "nrf_drv_common.h"
#include "app_util_platform.h"

/**@brief Function for initializing TWI.
 *
 * @details This function will be called once by main application to initialize TWI
 *
 */
void sds_twi_init (void);

/**@brief Function for finding TWI slaves.
 *
 * @details This function will be called when searching for TWI slaves during development
 *
 * @returns Address of slave found first. If -1, no TWI slaves were found
 */
int sds_twi_scan (void);

/**@brief Function for writing to a TWI slave
 *
 * @param[in] unsigned char  Slave Address.
 * @param[in] unsigned char  Register Address.
 * @param[in] unsigned char  Length of data.
 * @param[in] unsigned char const *  Pointer to data to write.
 */
int sds_twi_write(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char const *data);


/**@brief Function for reading from a TWI slave
 *
 * @param[in] unsigned char  Slave Address.
 * @param[in] unsigned char  Register Address.
 * @param[in] unsigned char  Length of data.
 * @param[out] unsigned char const *  Pointer to data to read.
 */
int sds_twi_read(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data);
#endif
