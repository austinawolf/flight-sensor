/**
 * @file    state_machine.c
 * @author  Austin Wolf
 * @brief
 */
#include "state_machine.h"
#include "status.h"
#include "logger.h"


typedef enum
{
    STATE_UNINIATIALIZED,
    STATE_IDLE,
    STATE_SAMPLING,
    STATE_PLAYBACK,
    STATE_ERROR,
    STATE_MAX_VALUE,
} state_e;

typedef state_e (*event_processor_t) (event_t *event);

typedef struct
{
    const char * name;
    const event_processor_t processor;
} state_t;

static state_e _idle_process(event_t *event);
static state_e _sampling_process(event_t *event);
static state_e _playback_process(event_t *event);
static state_e _error_process(event_t *event);

static state_t _states[STATE_MAX_VALUE] =
{
    [STATE_UNINIATIALIZED]  {.name = "UNINITIALIZED",    .processor=NULL},
    [STATE_IDLE]            {.name = "IDLE",             .processor=_idle_process},
    [STATE_SAMPLING]        {.name = "SAMPLING",         .processor=_sampling_process},
    [STATE_PLAYBACK]        {.name = "PLAYBACK",         .processor=_playback_process},
    [STATE_ERROR]           {.name = "ERROR",            .processor=_error_process},
};

static state_e _current_state = STATE_UNINIATIALIZED;

/**
 * @see state_machine.h
 */
void state_machine_create(void)
{
    _current_state = STATE_IDLE;
}

/**
 * @see state_machine.h
 */
void state_machine_process(event_t *event)
{
    if (_states[_current_state].processor == NULL)
    {
        return;
    }

    state_e next = _states[_current_state].processor(event);

    if (_current_state != next)
    {
        LOG_INFO("State Update: %s -> %s", _states[_current_state].name, _states[next].name);
        _current_state = next;
    }
}

static state_e _idle_process(event_t *event)
{
    state_e next = _current_state;

    switch (event->event)
    {
        case EVENT_START_SAMPLING:
        {
            next = STATE_SAMPLING;
            break;
        }
        case EVENT_START_PLAYBACK:
        {
            next = STATE_PLAYBACK;
            break;
        
        }
        default:
            break;
    }

    return next;
}

static state_e _sampling_process(event_t *event)
{
    state_e next = _current_state;

    switch (event->event)
    {
        case EVENT_STOP_SAMPLING:
        {
            next = STATE_IDLE;
            break;
        }
        default:
            break;
    }

    return next;
}

static state_e _playback_process(event_t *event)
{
    state_e next = _current_state;

    switch (event->event)
    {
        case EVENT_STOP_PLAYBACK:
        {
            next = STATE_IDLE;
            break;
        }
        default:
            break;
    }

    return next;
}

static state_e _error_process(event_t *event)
{
    state_e next = _current_state;

    switch (event->event)
    {
        case EVENT_CLEAR_ERROR:
        {
            next = STATE_IDLE;
            break;
        }
        default:
            break;
    }

    return next;
}
