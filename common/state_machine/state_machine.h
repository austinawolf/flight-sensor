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
#include "imu.h"

#define NULL_TRANSITION     (0xFFFFFFFF)

typedef uint32_t event_t;

typedef struct state_s state_t;

typedef struct
{
    const char *name;
    event_t event;
    const state_t *next;
} transition_t;

typedef void (*transition_callback_t)(const state_t *new_state, const state_t *previous_state, const transition_t *transition);

typedef struct
{
    const state_t *current;
    const state_t *previous;
    void *context;
    transition_callback_t callback;
} state_machine_t;

struct state_s
{
    const char * name;
    void (*on_entry)(void *context);
    void (*on_exit)(void *context);
    void (*on_reentry)(void *context);
    const transition_t *transitions;
};



status_e state_machine_create(state_machine_t *state_machine, const state_t *initial, transition_callback_t transition_callback, void *context);

status_e state_machine_on_event(state_machine_t *state_machine, event_t event);

#endif
