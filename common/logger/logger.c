/**
 * @file    logger.c
 * @author  Austin Wolf
 * @brief   wrapper for Nordic's logger module
 */

#include "logger.h"


/**
 * @see logger.h
 */
status_e logger_create(void)
{
    uint32_t err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);

    NRF_LOG_DEFAULT_BACKENDS_INIT();

    return STATUS_OK;
}
