#ifndef TWI_H_
#define TWI_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "sdk_common.h"
#include "nrf_drv_common.h"
#include "app_util_platform.h"

/**
 * @brief Function for initializing TWI.
 *
 * @details This function will be called once by main application to initialize TWI
 */
void twi_init (void);

/**
 * @brief Function for finding TWI slaves.
 *
 * @details This function will be called when searching for TWI slaves during development
 *
 * @returns Address of slave found first. If -1, no TWI slaves were found
 */
int twi_scan (void);

/**
 * @brief Function for writing to a TWI slave
 *
 * @param slave_addr Slave Address.
 * @param reg_addr Register Address.
 * @param length Length of data.
 * @param data Pointer to data to write.
 */
int twi_write(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char const *data);

/**
 * @brief Function for reading from a TWI slave
 *
 * @param slave_addr Slave Address.
 * @param reg_addr Register Address.
 * @param length Length of data.
 * @param data Pointer to data to read.
 */
int twi_read(unsigned char slave_addr, unsigned char reg_addr, unsigned char length, unsigned char *data);

#endif
