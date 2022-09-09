/**
 * @file    state_machine.c
 * @author  Austin Wolf
 * @brief
 */
#include "state_machine.h"
#include "status.h"
#include "logger.h"
#include "imu.h"
#include "app_scheduler.h"
#include "queue.h"

#define EVENT_QUEUE_LEN     (10u)


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

static event_t _buffer[EVENT_QUEUE_LEN] = {0};
static queue_t _event_queue = {0};


/**
 * @brief 
 * 
 * @param event 
 */
static void _process(event_t *event)
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

/**
 * @brief 
 * 
 */
static void _on_new_event(void * p_event_data, uint16_t event_size)
{
    event_t event = {0};
    status_e status = queue_pop(&_event_queue, 0, &event);
    if (status != STATUS_OK)
    {
        LOG_ERROR("queue_pop failed: %d", status);
        return;
    }

    _process(&event);
}

/**
 * @see state_machine.h
 */
void state_machine_create(void)
{
    _current_state = STATE_IDLE;

    status_e status = queue_create(&_event_queue, _buffer, EVENT_QUEUE_LEN, sizeof(event_t));
    if (status != STATUS_OK)
    {
        LOG_ERROR("queue_create failed, err: %d", status);
    }
}

/**
 * @see state_machine.h
 */
void state_machine_add_event(event_t *event)
{
    status_e status = queue_append(&_event_queue, event);
    if (STATUS_OK != status)
    {
        LOG_ERROR("queue_append failed, err: %d", status);
        return;
    }

    app_sched_event_put(NULL, 0, _on_new_event);
}

static state_e _idle_process(event_t *event)
{
    state_e next = _current_state;

    switch (event->event)
    {
        case EVENT_START_SAMPLING:
        {
            imu_config_t config = 
            {
                .rate = event->start_sampling.rate,
                .flags = event->start_sampling.flags,
            };

            status_e status = imu_start(&config);
            if (status != STATUS_OK)
            {
                LOG_ERROR("imu_start failed, err: %d", status);
            }
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
        case EVENT_DISCONNECTED:
        case EVENT_STOP_SAMPLING:
        {
            status_e status = imu_stop();
            if (status != STATUS_OK)
            {
                LOG_ERROR("imu_stop failed, err: %d", status);
            }
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
        case EVENT_DISCONNECTED:
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
