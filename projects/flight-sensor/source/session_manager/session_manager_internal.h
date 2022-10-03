/**
 * @file    session_manager_internal.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef SESSION_MANAGER_INTERNAL_H_
#define SESSION_MANAGER_INTERNAL_H_

#include <stdint.h>
#include <stdbool.h>
#include "state_machine.h"
#include "app_timer.h"
#include "session_manager.h"
#include "session_store.h"


typedef enum
{
    SESSION_EVENT_STREAM,
    SESSION_EVENT_RECORD,
    SESSION_EVENT_PLAYBACK,
    SESSION_EVENT_STOP,
    SESSION_EVENT_TIMEOUT,
    SESSION_EVENT_DISCONNECTED,
    SESSION_EVENT_CALIBRATE,
    SESSION_EVENT_SUCCESS,
    SESSION_EVENT_ERROR,
    SESSION_EVENT_PLAYBACK_DONE,
    SESSION_EVENT_FLASH_FULL,
} session_event_e;

typedef struct
{
    state_machine_t sm;
    imu_sample_rate_e rate;
    session_destination_e destination;
    uint8_t flags;
    bool stream_enabled;
    uint32_t session_time;
    app_timer_id_t timer;
    uint32_t playback_index;
} session_manager_control_t;

extern const state_t *session_initial_state;

void on_calibration_done(bool success);

#endif
