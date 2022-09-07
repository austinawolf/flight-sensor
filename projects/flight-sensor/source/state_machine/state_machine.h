/**
 * @file    state_machine.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef STATE_MACHINE_H_
#define STATE_MACHINE_H_

#include <stdint.h>
#include <stdbool.h>
#include "status.h"


typedef enum
{
    EVENT_START_SAMPLING,
    EVENT_STOP_SAMPLING,
    EVENT_START_PLAYBACK,
    EVENT_STOP_PLAYBACK,
    EVENT_CLEAR_ERROR,
} event_e;

typedef struct
{
    event_e event;
    union {
        struct {
            uint8_t rate;
            uint8_t flags;
            uint8_t destination;
            uint8_t time;
        } start_sampling;
    };
} event_t;


/**
 * @brief 
 * 
 * @return status_e 
 */
void state_machine_create(void);

/**
 * @brief
 * 
 * @param event 
 */
void state_machine_process(event_t *event);

#endif
