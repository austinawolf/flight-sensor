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
 * @brief 
 * 
 */
#define LOG_ERROR       NRF_LOG_ERROR

/**
 * @brief 
 * 
 */
#define LOG_WARNING     NRF_LOG_WARNING

/**
 * @brief 
 * 
 */
#define LOG_INFO        NRF_LOG_INFO

/**
 * @brief 
 * 
 */
#define LOG_DEBUG       NRF_LOG_DEBUG

/**
 * @brief 
 * 
 */
#define LOG_FLUSH       NRF_LOG_FLUSH

/**
 * @brief 
 * 
 * @return status_e 
 */
status_e logger_create(void);

#endif
