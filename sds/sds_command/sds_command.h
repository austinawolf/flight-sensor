/** @file */ 

#ifndef SDS_COMMAND_H_
#define SDS_COMMAND_H_

#include <stdbool.h>
#include <stddef.h>
#include "app_fifo.h"
#include "sds_ble_motion_types.h"
#include "sds_motion_types.h"

/**
    * @brief gets timestamp in ms
    * @param pointer to desired timestamp result 
    * @return none
*/

#define CHAR_LEN 20


typedef struct {
    uint8_t data[CHAR_LEN];
    uint8_t len;
} command_buffer_t;


uint32_t command_init(void);
uint32_t get_serial_command(command_buffer_t * p_command_buffer);
void pass_serial_notif(app_fifo_t * p_fifo, sds_notif_t * p_notif);

#endif
