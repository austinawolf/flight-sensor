#include <stdint.h>
#include <string.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_sdm.h"
#include "app_error.h"
#include "nrf_pwr_mgmt.h"
#include "logger.h"
#include "nrf_ble_lesc.h"
#include "ble_helper.h"
#include "state_machine.h"
#include "imu.h"


/**@brief Function for handling the idle state (main loop).
 *
 * @details If there is no pending log operation, then sleep until next the next event occurs.
 */
static void idle_state_handle(void)
{
    ret_code_t err_code;

    err_code = nrf_ble_lesc_request_handler();
    APP_ERROR_CHECK(err_code);

    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}


static void _imu_sample_callback(imu_sample_t *sample)
{
    LOG_INFO("Sample ready");
}

/**@brief Function for application main entry.
 */
int main(void)
{
    // Initialize.
    logger_create();
    ble_helper_create();
    state_machine_create();
    imu_create();

    LOG_INFO("Flight Sensor Started.");

    imu_register_sample_callback(_imu_sample_callback);

    // Enter main loop.
    for (;;)
    {
        idle_state_handle();
    }
}


