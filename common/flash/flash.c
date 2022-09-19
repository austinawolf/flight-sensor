/**
 * @file    flash.c
 * @author  Austin Wolf
 * @brief
 */
#include "flash.h"
#include "logger.h"
#include "nrf_drv_qspi.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"


#define QSPI_STD_CMD_WRSR   0x01
#define QSPI_STD_CMD_RSTEN  0x66
#define QSPI_STD_CMD_RST    0x99


static volatile bool m_finished = false;


typedef struct 
{
    volatile bool is_busy;
    flash_event_callback_t callback;
    void *context;
} flash_control_t;


static flash_control_t _control = {0};


static void qspi_handler(nrf_drv_qspi_evt_t event, void * p_context)
{
    (void) event;

    _control.is_busy = false;
    if (_control.callback != NULL)
    {
        _control.callback(FLASH_EVENT_DONE, p_context);
    }
}

static void configure_memory()
{
    uint8_t temporary = 0x40;
    uint32_t err_code;

    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode    = QSPI_STD_CMD_RSTEN,
        .length    = NRF_QSPI_CINSTR_LEN_1B,
        .io2_level = true,
        .io3_level = true,
        .wipwait   = true,
        .wren      = true
    };

    // Send reset enable
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    // Send reset command
    cinstr_cfg.opcode = QSPI_STD_CMD_RST;
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    // Switch to qspi mode
    cinstr_cfg.opcode = QSPI_STD_CMD_WRSR;
    cinstr_cfg.length = NRF_QSPI_CINSTR_LEN_2B;
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, &temporary, NULL);
    APP_ERROR_CHECK(err_code);
}

/**
 * @see flash.h
 */
status_e flash_create(void)
{
    nrf_drv_qspi_config_t config = NRF_DRV_QSPI_DEFAULT_CONFIG;

    uint32_t err_code = nrf_drv_qspi_init(&config, qspi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    configure_memory();

    //nrf_drv_qspi_uninit();

    return STATUS_OK;
}

/**
 * @see flash.h
 */
status_e flash_read(uint32_t address, uint8_t *data, uint32_t len)
{
    _control.callback = NULL;
    _control.is_busy = true;
    uint32_t err_code = nrf_drv_qspi_read(data, len, address);
    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

/**
 * @see flash.h
 */
status_e flash_write(uint32_t address, const uint8_t *data, uint32_t len, flash_event_callback_t callback, void *context)
{
    // LOG_INFO("Writing...");

    _control.callback = callback;
    _control.is_busy = true;
    _control.context = context;

    uint32_t err_code = nrf_drv_qspi_write(data, len, address);
    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR;
    }    

    return STATUS_OK;
}

/**
 * @see flash.h
 */
status_e flash_erase(uint32_t address, flash_erase_e type, flash_event_callback_t callback, void *context)
{
    m_finished = false;
    nrf_qspi_erase_len_t erase_len = 0u;

    // LOG_INFO("Erasing...");

    switch (type)
    {
        case FLASH_ERASE_SECTOR:
            erase_len = NRF_QSPI_ERASE_LEN_4KB;
            break;
        case FLASH_ERASE_BLOCK:
            erase_len = NRF_QSPI_ERASE_LEN_64KB;
            break;
        case FLASH_ERASE_ALL:
            erase_len = NRF_QSPI_ERASE_LEN_ALL;
            break;
    }

    _control.callback = callback;
    _control.is_busy = true;
    _control.context = context;

    uint32_t err_code = nrf_drv_qspi_erase(erase_len, address);
    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

/**
 * @see flash.h
 */
status_e flash_is_busy(bool *is_busy)
{
    *is_busy = _control.is_busy;

    return STATUS_OK;
}
