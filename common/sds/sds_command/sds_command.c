/** @file */ 
#include "sds_command.h"

//nrf
#include "app_uart.h"
#include "nrf_delay.h"

//sds
#include "sds_log_config.h"

#define NRF_LOG_MODULE_NAME cmd
#if SDS_CMD_CONFIG_LOG_ENABLED
#define NRF_LOG_LEVEL SDS_CMD_CONFIG_LOG_LEVEL
#define NRF_LOG_INFO_COLOR SDS_CMD_CONFIG_INFO_COLOR
#define NRF_LOG_DEBUG_COLOR SDS_CMD_CONFIG_DEBUG_COLOR
#else
#define NRF_LOG_LEVEL       0
#endif
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define SERIAL_PACKET_PREAMBLE 0xee
#define SERIAL_PACKET_TRAILER 0x0d
#define MIN_COMMAND_LENGTH 3
#define MAX_COMMAND_LENGTH 20

uint32_t get_serial_command(command_buffer_t * p_command_buffer) {
	
    uint8_t index = 0;
    uint32_t timeout_clock = 0;
    uint32_t err_code;
    uint8_t cr;
	
	/* check for preamble */
	if (app_uart_get(&cr) == NRF_SUCCESS) {
		if (cr != SERIAL_PACKET_PREAMBLE) {
			return SDS_INVALID_PREAMBLE;
		}
	}
	else {
		return SDS_INVALID_PREAMBLE;
	}
    NRF_LOG_DEBUG("Preamble: 0x%x", cr);

	/* clear buffer */
	memset(p_command_buffer->data, 0, CHAR_LEN);
		
	/* get length and check */
	app_uart_get(&cr);
	p_command_buffer->len = cr;

	NRF_LOG_DEBUG("Length: %d", p_command_buffer->len);
	if (p_command_buffer->len < MIN_COMMAND_LENGTH) {
		return SDS_SHORT_COMMAND;
	}
	if (p_command_buffer->len > MAX_COMMAND_LENGTH) {
		return SDS_SHORT_COMMAND;
	}
		
	/* get data */
	index = 0;	
	for (int i = 0; i < p_command_buffer->len; i++)
	{	
		/* wait for new data */
		while (app_uart_get(&cr) == NRF_ERROR_NOT_FOUND) {
			
		}	
		p_command_buffer->data[index] = cr;	
		NRF_LOG_DEBUG("Data: 0x%x.", p_command_buffer->data[index]);
		index++;
		
	}

	/* wait for trailer and check */
	while (app_uart_get(&cr) == NRF_ERROR_NOT_FOUND) {
		
	}
	NRF_LOG_DEBUG("Trailer: 0x%x.", cr);
	if (cr == SERIAL_PACKET_TRAILER) {
		NRF_LOG_DEBUG("Success.");		
		return SDS_SUCCESS;
	}
	else {
		return SDS_ERROR;
	}
		
}


void pass_serial_notif(app_fifo_t * p_fifo, sds_notif_t * p_notif) {

    uint32_t err_code;	
	
	err_code = app_fifo_put(p_fifo, SERIAL_PACKET_PREAMBLE);
	APP_ERROR_CHECK(err_code);
	NRF_LOG_DEBUG("Preamble: 0x%x", SERIAL_PACKET_PREAMBLE);
	
	err_code = app_fifo_put(p_fifo, p_notif->len);
	APP_ERROR_CHECK(err_code);
	NRF_LOG_DEBUG("Length: %d", p_notif->len);
	
	for (int i = 0; i < p_notif->len; i++) {
		
		err_code = app_fifo_put(p_fifo, p_notif->p_data[i]);
		APP_ERROR_CHECK(err_code);
		//NRF_LOG_DEBUG("Data: 0x%x", p_notif->p_data[i]);
						
	}
	
	err_code = app_fifo_put(p_fifo, SERIAL_PACKET_TRAILER);
	APP_ERROR_CHECK(err_code);
	
    uint32_t tmp = p_fifo->read_pos;
    uint32_t len = p_fifo->write_pos - tmp;	
	NRF_LOG_DEBUG("Fifo Status: %d", len);
		
}
