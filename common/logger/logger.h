/**
 * @file    logger.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


/**
 * @brief Log a message at error level
 */
#define LOG_ERROR NRF_LOG_ERROR

/**
 * @brief Log a message at warning level
 */
#define LOG_WARNING NRF_LOG_WARNING

/**
 * @brief Log a message at info level
 */
#define LOG_INFO NRF_LOG_INFO

/**
 * @brief Log a message at debug level
 */
#define LOG_DEBUG NRF_LOG_DEBUG

/**
 * @brief Flush any buffered logs
 */
#define LOG_FLUSH NRF_LOG_FLUSH

/**
 * @brief Initializes the logger
 * 
 * @return STATUS_OK on success 
 */
status_e logger_create(void);

#endif
