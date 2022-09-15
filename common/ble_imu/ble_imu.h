#ifndef BLE_IMU_H__
#define BLE_IMU_H__

#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_gatt.h"
#include "imu.h"

#ifdef __cplusplus
extern "C" {
#endif


#define BLE_IMU_BLE_OBSERVER_PRIO   3


#define IGNORED 0x00

/** @defgroup 
 * @{ */
#define BLE_UUID_IMU_SERVICE_BASE 	    {0x03,0xb0,0xf8,0x4b,0x02,0xd9,0xa9,0xb9,0xca,0x40,0x2c,0xc3,0xFF,0x18,0x44,0x92} //{924418ff-c32c-40ca-b9a9-d9024bf8b003}
#define BLE_UUID_IMU_SERVICE            0x18FF

/** @defgroup
 * @{ */
#define BLE_UUID_DATA_CHAR_BASE 	    {0x01,0x12,0xc9,0x8e,0x8c,0x67,0x60,0xb6,0xbb,0x41,0x64,0x82,IGNORED,IGNORED,0x19,0x43} //{43192aff-8264-41bb-b660-678c8ec91201}
#define BLE_UUID_DATA_CHAR              0x2AFF   /**< Orientation characteristic UUID. */
#define BLE_UUID_COMMAND_CHAR_BASE 		{0x01,0xad,0x59,0xa1,0x0f,0x1e,0x48,0xa5,0x17,0x4a,0xc0,0xa7,IGNORED,IGNORED,0x54,0xd9} //{d9541770-a7c0-4a17-a548-1e0fa159ad01}
#define BLE_UUID_COMMAND_CHAR           0x1770   /**< Command Orientation characteristic UUID. */	
	

/**@brief   Macro for defining a ble_imu instance.
 *
 * @param   _name   Name of the instance.
 * @hideinitializer
 */
#define BLE_IMU_DEF(_name)                                                                          \
static ble_imu_t _name;                                                                             \
NRF_SDH_BLE_OBSERVER(_name ## _obs,                                                                 \
                     BLE_IMU_BLE_OBSERVER_PRIO,                                                     \
                     ble_imu_on_ble_evt, &_name)

/**@brief Heart Rate Service event. */
typedef struct
{
    ble_imu_evt_type_t evt_type;    /**< Type of event. */
} ble_imu_evt_t;

// Forward declaration of the ble_imu_t type.
typedef struct ble_imu_s ble_imu_t;

/**@brief Heart Rate Service event handler type. */
typedef void (*ble_imu_evt_handler_t) (ble_imu_t * p_imu, ble_imu_evt_t * p_evt);

/**@brief Heart Rate Service init structure. This contains all options and data needed for
 *        initialization of the service. */
typedef struct
{
    ble_imu_evt_handler_t        evt_handler;                                          /**< Event handler to be called for handling events in the Heart Rate Service. */
    security_req_t               hrm_cccd_wr_sec;                                      /**< Security requirement for writing the HRM characteristic CCCD. */
    security_req_t               bsl_rd_sec;                                           /**< Security requirement for reading the BSL characteristic value. */
} ble_imu_init_t;

/**@brief Heart Rate Service structure. This contains various status information for the service. */
struct ble_imu_s
{
    ble_imu_evt_handler_t        evt_handler;                                          /**< Event handler to be called for handling events in the Heart Rate Service. */
    uint16_t                     service_handle;                                       /**< Handle of Heart Rate Service (as provided by the BLE stack). */
    ble_gatts_char_handles_t     command_handles;                                          /**< Handles related to the Heart Rate Measurement characteristic. */
    ble_gatts_char_handles_t     data_handles;                                          /**< Handles related to the Body Sensor Location characteristic. */
    uint16_t                     conn_handle;                                          /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection). */
    uint8_t                      uuid_type;
};


/**@brief Function for initializing the Heart Rate Service.
 *
 * @param[out]  p_imu       Heart Rate Service structure. This structure will have to be supplied by
 *                          the application. It will be initialized by this function, and will later
 *                          be used to identify this particular service instance.
 * @param[in]   p_imu_init  Information needed to initialize the service.
 *
 * @return      NRF_SUCCESS on successful initialization of service, otherwise an error code.
 */
uint32_t ble_imu_init(ble_imu_t * p_imu, ble_imu_init_t const * p_imu_init);


/**@brief Function for handling the GATT module's events.
 *
 * @details Handles all events from the GATT module of interest to the Heart Rate Service.
 *
 * @param[in]   p_imu      Heart Rate Service structure.
 * @param[in]   p_gatt_evt  Event received from the GATT module.
 */
void ble_imu_on_gatt_evt(ble_imu_t * p_imu, nrf_ble_gatt_evt_t const * p_gatt_evt);


/**@brief Function for handling the Application's BLE Stack events.
 *
 * @details Handles all events from the BLE stack of interest to the Heart Rate Service.
 *
 * @param[in]   p_ble_evt   Event received from the BLE stack.
 * @param[in]   p_context   Heart Rate Service structure.
 */
void ble_imu_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

/**@brief Function for sending heart rate measurement if notification has been enabled.
 *
 * @details The application calls this function after having performed a heart rate measurement.
 *          If notification has been enabled, the heart rate measurement data is encoded and sent to
 *          the client.
 *
 * @param[in]   p_imu                    Heart Rate Service structure.
 * @param[in]   heart_rate               New heart rate measurement.
 *
 * @return      NRF_SUCCESS on success, otherwise an error code.
 */
uint32_t ble_imu_sample_send(ble_imu_t * p_imu, imu_sample_t *sample);

#ifdef __cplusplus
}
#endif

#endif // BLE_IMU_H__

/** @} */
