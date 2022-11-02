/**
 * @file    
 * @author  Austin Wolf
 * @brief
 */

#include <string.h>
#include "datastore.h"
#include "flash.h"
#include "logger.h"
#include "status.h"

/**
 *
 */
#define DATASTORE_PREAMBLE  (0xDEADBEEF)

/**
 * 
 */
#define DATASTORE_VERSION   (1u)

/**
 * @brief 
 */
static void _flash_event_handler(flash_event_e event, void *context);

static datastore_t _shadow_copy = {0};
static flash_instance_t flash = {.start = 0, .len = 1, .event_callback = _flash_event_handler};

/**
 * @brief 
 */
static void _set_defaults(void)
{
    const datastore_t defaults = 
    {
        .preamble = DATASTORE_PREAMBLE,
        .version = DATASTORE_VERSION,
        .icm20948_calibration = 
        {
            .accel_bias = {0, 0, 0},
            .gyro_bias = {0, 0, 0}
        }
    };

    (void) memcpy(&_shadow_copy, &defaults, sizeof(datastore_t));
}

/**
 * @brief 
 */
static status_e _write_to_flash(void)
{
    return flash_erase(&flash, 0, FLASH_ERASE_SECTOR);;
}

/**
 * Event handler for events from flash driver
 */
static void _flash_event_handler(flash_event_e event, void *context)
{
    switch (event)
    {
        case FLASH_EVENT_WRITE_DONE:
            LOG_INFO("Datastore updated");
            break;
        case FLASH_EVENT_ERASE_DONE:
            flash_write(&flash, 0, 0, (uint8_t*) &_shadow_copy, sizeof(datastore_t));
            break;
        case FLASH_EVENT_READ_DONE:
            break;
        default:
            break;
    }
}

/**
 * @see datastore.h
 */
status_e datastore_create(void)
{
    // read from flash
    status_e status = flash_read(&flash, 0u, 0u, (uint8_t*) &_shadow_copy, sizeof(datastore_t));
    if (STATUS_OK != status)
    {
        return status;
    }

    // block until read completes
    bool is_busy = false;
    do
    {
        (void) flash_is_busy(&is_busy);
    } while (is_busy);

    // verify
    if ((_shadow_copy.preamble != DATASTORE_PREAMBLE) || (_shadow_copy.version != DATASTORE_VERSION))
    {
        LOG_INFO("Datastore invalid, resetting to defaults");
        datastore_reset();
    }

    return STATUS_OK;
}

/**
 * @see datastore.h
 */
status_e datastore_reset(void)
{
    _set_defaults();

    status_e status = _write_to_flash();

    return status;
}

/**
 * @see datastore.h
 */
status_e datastore_get_cal_data(icm20948_calibration_t *data)
{
    (void) memcpy(data, &_shadow_copy.icm20948_calibration, sizeof(icm20948_calibration_t));
    
    return STATUS_OK;
}

/**
 * @see datastore.h
 */
status_e datastore_set_cal_data(const icm20948_calibration_t *data)
{
    (void) memcpy(&_shadow_copy.icm20948_calibration, data, sizeof(icm20948_calibration_t));

    return STATUS_OK;
}

/**
 * @see datastore.h
 */
status_e datastore_flush(void)
{
    return _write_to_flash();
}
