/**
 * @file   status.h
 * @author Austin Wolf
 * @brief
 */

#ifndef STATUS_H_
#define STATUS_H_

#include "app_error.h"


/**
 * @brief 
 * 
 */
#define ASSERT_STATUS(ERR_CODE)                             \
    do                                                      \
    {                                                       \
        const status_e LOCAL_ERR_CODE = (ERR_CODE);         \
        if (LOCAL_ERR_CODE != STATUS_OK)                    \
        {                                                   \
            APP_ERROR_HANDLER(LOCAL_ERR_CODE);              \
        }                                                   \
    } while (0)

/**
 *
 */
typedef enum {
    STATUS_OK,
    STATUS_ERROR,
    STATUS_ERROR_INVALID_LENGTH,
    STATUS_ERROR_INVALID_PARAM,
    STATUS_ERROR_INVALID_STATE,
    STATUS_ERROR_INVALID_VALUE,
    STATUS_ERROR_EMPTY,
    STATUS_ERROR_DATA_SIZE,
    STATUS_ERROR_BUFFER_FULL,
    STATUS_ERROR_FLASH_FULL,
    STATUS_ERROR_INTERNAL,
} status_e;

#endif /* STATUS_H_ */
