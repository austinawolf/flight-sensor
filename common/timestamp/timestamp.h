/**
 * @file    timestamp.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"


/**
 * @brief 
 * 
 * @return status_e 
 */
status_e timestamp_create(void);

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e timestamp_destory(void);

/**
 * @brief 
 * 
 * @return uint32_t 
 */
uint32_t timestamp_get(void);

#endif
