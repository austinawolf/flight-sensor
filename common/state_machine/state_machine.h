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

/**
 * @brief Fence to indicate the end of the transition list 
 */
#define NULL_TRANSITION     (0xFFFFFFFF)

/**
 * @brief Definition of event code
 */
typedef uint32_t event_t;

/**
 * @brief Forward declaration of state
 */
typedef struct state_s state_t;

/**
 * @brief Definition of transition for a state
 */
typedef struct
{
    const char *name;
    event_t event;
    const state_t *next;
} transition_t;

/**
 * @brief Defintion of transition callback function
 */
typedef void (*transition_callback_t)(const state_t *new_state, const state_t *previous_state, const transition_t *transition);

/**
 * @brief Definition of state machine control structure
 */
typedef struct
{
    const state_t *current;
    const state_t *previous;
    void *context;
    transition_callback_t callback;
} state_machine_t;

/**
 * @brief Definition of a state in the state machine
 */
struct state_s
{
    const char * name;
    const uint8_t id;
    void (*on_entry)(void *context);
    void (*on_exit)(void *context);
    void (*on_reentry)(void *context);
    const transition_t *transitions;
};

/**
 * @brief Initializes a state machine
 * 
 * @param state_machine pointer to state machine control
 * @param initial pointer to initial state
 * @param transition_callback function called with a transition occurs
 * @param context pointer passed to all state entry/exit functions
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e state_machine_create(state_machine_t *state_machine, const state_t *initial, transition_callback_t transition_callback, void *context);

/**
 * @brief Processes event through state machine
 * 
 * @param state_machine pointer to state machine control
 * @param event event to be processed
 * @return status_e STATUS_OK if success, otherwise see #status_e
 */
status_e state_machine_on_event(state_machine_t *state_machine, event_t event);

#endif
