/**
 * @file    timestamp.c
 * @author  Austin Wolf
 * @brief   keeps milliseconds since boot
 */
#include "logger.h"
#include "app_timer.h"


/**
 * @brief Time of a single tick, in milliseconds
 */
#define TICK_PERIOD_MS (1u)

/**
 * @brief Construct a new app timer def object
 */
APP_TIMER_DEF(m_timer);

/**
 * @brief Total elapsed time
 */
static volatile uint32_t _time_elapsed_ms = 0u;

/**
 * @brief callback for timer tick
 */
static void _on_tick(void * p_context)
{
    (void) p_context;

    _time_elapsed_ms++;
}

/**
 * @see timestamp.h
 */
status_e timestamp_create(void)
{
    uint32_t err_code = app_timer_create(&m_timer,
                                            APP_TIMER_MODE_REPEATED,
                                            _on_tick);
    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR;
    }

    err_code = app_timer_start(m_timer, APP_TIMER_TICKS(TICK_PERIOD_MS), NULL);
    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

/**
 * @see timestamp.h
 */
status_e timestamp_destory(void)
{
    uint32_t err_code = app_timer_stop(m_timer);
    if (err_code != NRF_SUCCESS)
    {
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

/**
 * @see timestamp.h
 */
uint32_t timestamp_get(void)
{
    return _time_elapsed_ms;
}
