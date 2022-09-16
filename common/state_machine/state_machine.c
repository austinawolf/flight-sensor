/**
 * @file    state_machine.c
 * @author  Austin Wolf
 * @brief
 */
#include "state_machine.h"
#include "status.h"
#include "logger.h"

status_e state_machine_create(state_machine_t *state_machine, const state_t *initial, transition_callback_t transition_callback, void *context)
{
    state_machine->current = initial;
    state_machine->previous = NULL;
    state_machine->context = context;
    state_machine->callback = transition_callback;

    return STATUS_OK;
}

static const transition_t *_get_transition(state_machine_t *state_machine, uint32_t event)
{
    uint8_t i = 0u;
    while (true)
    {
        const transition_t *transition = &state_machine->current->transitions[i];
        if (transition->event == NULL_TRANSITION)
        {
            return NULL;
        }
        else if (event == transition->event)
        {
            return transition;
        }
        else
        {
            i++;
        }
    }

}

status_e state_machine_on_event(state_machine_t *state_machine, event_t event)
{
    const transition_t *transition = _get_transition(state_machine, event);
    if (transition == NULL)
    {
        return STATUS_ERROR;
    }

    if (transition->next == state_machine->current)
    {
        state_machine->previous = state_machine->current;
        if (state_machine->current->on_reentry != NULL)
        {
            state_machine->current->on_reentry(state_machine->context);
        }
    }
    else
    {
        if (state_machine->current->on_exit != NULL)
        {
            state_machine->current->on_exit(state_machine->context);
        }

        state_machine->previous = state_machine->current;
        state_machine->current = transition->next;

        if (state_machine->current->on_entry != NULL)
        {
            state_machine->current->on_entry(state_machine->context);
        }
    }

    if (state_machine->callback)
    {
        state_machine->callback(state_machine->current, state_machine->previous, transition);
    }

    return STATUS_OK;
}