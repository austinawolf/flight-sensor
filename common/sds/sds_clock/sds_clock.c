/** @file */ 

#include "sds_clock.h"
#include "app_timer.h"

#ifdef USE_SDS_LOG_CONFIG
#include "sds_log_config.h"
#endif

#define NRF_LOG_MODULE_NAME sds_clk
#if SDS_CLOCK_CONFIG_LOG_ENABLED
#define NRF_LOG_LEVEL SDS_CLOCK_CONFIG_LOG_LEVEL
#define NRF_LOG_INFO_COLOR SDS_CLOCK_CONFIG_INFO_COLOR
#define NRF_LOG_DEBUG_COLOR SDS_CLOCK_CONFIG_DEBUG_COLOR
#else
#define NRF_LOG_LEVEL       0
#endif
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
NRF_LOG_MODULE_REGISTER();

#define RESOLUTION_MS 1

APP_TIMER_DEF(m_clock_timer);  /**< Clock timer. */

static unsigned long time_elapsed_ms = 0;

static void event_handler(void * p_context);

void sds_clock_init(void) {
    ret_code_t err_code;
    err_code = app_timer_create(&m_clock_timer,
                                APP_TIMER_MODE_REPEATED,
                                event_handler);
    APP_ERROR_CHECK(err_code);
	
    err_code = app_timer_start(m_clock_timer, APP_TIMER_TICKS(RESOLUTION_MS), NULL);
    APP_ERROR_CHECK(err_code);
}

/**
    * @brief Accuracy not guaranteed
*/
void sds_get_ms(unsigned long * p_timestamp) {	
	*p_timestamp = time_elapsed_ms;	
}

/**
    * @brief Old implementation. Need to compare new function against old to show accuracy.
*/
void sds_get_ms1(unsigned long * p_timestamp) {	
	if (p_timestamp == NULL) return;
	uint32_t count = app_timer_cnt_get();
	
	*p_timestamp = count * ((APP_TIMER_CONFIG_RTC_FREQUENCY + 1) * 1000) / APP_TIMER_CLOCK_FREQ;
	
	return;
}


static void event_handler(void * p_context) {
	time_elapsed_ms += RESOLUTION_MS;
}
