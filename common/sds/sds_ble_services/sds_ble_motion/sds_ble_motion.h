


#ifndef BLE_MOTION_H__
#define BLE_MOTION_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"


//sds
#include "sds_error.h"
#include "sds_motion_types.h"
#include "sds_ble_motion_types.h"

/**@brief   Macro for defining a ble_motion instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_MOTION_DEF(_name)                                                                          \
ble_motion_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_MOTION_BLE_OBSERVER_PRIO,                                                     \
                     ble_motion_on_ble_evt, &_name)

#include "sds_error.h"


/**@brief Motion Service event type. */
typedef enum
{
    BLE_MOTION_EVT_NOTIFICATION_ENABLED,   /**< Motion value notification enabled event. */
    BLE_MOTION_EVT_NOTIFICATION_DISABLED   /**< Motion value notification disabled event. */
} ble_motion_evt_type_t;

/**@brief Motion Service Event handler type. */
typedef void (*ble_motion_evt_handler_t) (ble_motion_evt_type_t evt_type);

/**@brief Motion Service Command handler type. */
typedef void (*ble_motion_command_handler_t) (sds_command_t * p_command);

/**@brief Motion Service init structure. This contains all options and data needed for
 *        initialization of the service. */
typedef struct
{
    ble_motion_command_handler_t		command_handler;                                  /**< Event handler to be called for handling events in the Motion Service. */
	ble_motion_evt_handler_t			evt_handler;
    ble_srv_cccd_security_mode_t 		motion_motionm_attr_md;                               /**< Initial security level for Motion service attribute */
} ble_motion_init_t;

/**@brief Motion Service structure. This contains various status information for the service. */
typedef struct 
{
    ble_motion_command_handler_t    	command_handler;                          /**< Event handler to be called for handling events in the Motion Service. */
    ble_motion_evt_handler_t			evt_handler;
	uint16_t                     		service_handle;                                       /**< Handle of Motion Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t     		motionm_handles;                                      /**< Handles related to the Motion characteristic. */
    uint16_t                     		conn_handle;                                          /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t                      		max_motionm_len;
	uint8_t						 		uuid_type;
} ble_motion_t;

/**@brief Function for initializing the Motion Service.
 *
 * @param[out]  p_motion    Motion Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_motion_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_motion_init(ble_motion_t * p_motion, ble_motion_init_t const * p_motion_init);


/**@brief Function for handling the GATT module's events.
 *
 * @details Handles all events from the GATT module of interest to the Motion Service.
 *
 * @param[in]   p_motion      Motion Service structure.
 * @param[in]   p_gatt_evt  Event received from the GATT module.
 */
void ble_motion_on_gatt_evt(ble_motion_t * p_motion, nrf_ble_gatt_evt_t const * p_gatt_evt);


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Motion Service.
 *
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 * @param[in]   p_context   Motion Service structure.
 */
void ble_motion_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

/**@brief Function for sending motion data if notification has been enabled.
 *
 * @details The application calls this function to send motion data.
 *          If notification has been enabled, the Motion measurement data is encoded and sent to
 *          the client.
 *
 * @param[in]   p_motion                 Motion Service structure.
 * @param[in]   motion               	 New Motion measurement.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */

uint32_t ble_motion_data_send(ble_motion_t * p_motion, motion_sample_t * p_motion_sample);

/**@brief Function for writing a response to the command charactersitics
 *
 * @details The application calls this function after recieiving a command.
 *
 * @param[in]   p_motion                 Motion Service structure.
 * @param[in]   opcode               	 Opcode. @ref sds_opcode_t
 * @param[in]   arg_len               	 Length of arguments.
 * @param[in]   p_args               	 Pointer to arguments.
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
 
uint32_t ble_motion_command_response_send(ble_motion_t * p_motion, sds_opcode_t opcode, uint8_t arg_len, uint8_t * p_args);

/**@brief Function for writing a response to the command charactersitics
 *
 * @details The application calls this function after recieiving a command.
 *
 * @param[in]   p_motion                 Motion Service structure.
 * @param[in]   opcode               	 Opcode. @ref sds_opcode_t
 * @param[in]   err_code               	 sds_return_t.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
 
uint32_t ble_motion_command_error_send(ble_motion_t * p_motion, sds_opcode_t opcode, sds_return_t err_code);

#ifdef __cplusplus
}
#endif

#endif // BLE_MOTION_H__

/** @} */
