/**
 * @file    logger.c
 * @author  Austin Wolf
 * @brief
 */
#include "logger.h"
#include "app_timer.h"


/**
 * @see logger.h
 */
uint32_t timestamp_get(void)
{
    return app_timer_cnt_get();
}
