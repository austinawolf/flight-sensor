/**
 * @file    logger.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef SESSION_MANAGER_INTERNAL_H_
#define SESSION_MANAGER_INTERNAL_H_

#include <stdint.h>
#include <stdbool.h>
#include "state_machine.h"
#include "app_timer.h"


typedef enum
{
    SESSION_EVENT_START_SAMPLING,
    SESSION_EVENT_STOP_SAMPLING,
    SESSION_EVENT_TIMEOUT,
    SESSION_EVENT_START_PLAYBACK,
    SESSION_EVENT_STOP_PLAYBACK,
    SESSION_EVENT_DISCONNECTED,
    SESSION_EVENT_CALIBRATE,
    SESSION_EVENT_SUCCESS,
    SESSION_EVENT_ERROR,
} session_event_e;

typedef struct
{
    state_machine_t sm;
    uint8_t rate;
    uint8_t destination;
    uint8_t flags;
    uint32_t session_time;
    app_timer_id_t timer;
} session_manager_control_t;

extern const state_t *session_initial_state;

#endif
