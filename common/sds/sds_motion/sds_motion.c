#include "sds_motion.h"
#include "sds_sensor.h"
#include "app_timer.h"
#include "imu_cal.h"

#include "sds_log_config.h"
#define NRF_LOG_MODULE_NAME mot
#if SDS_MOTION_CONFIG_LOG_ENABLED
#define NRF_LOG_LEVEL SDS_MOTION_CONFIG_LOG_LEVEL
#define NRF_LOG_INFO_COLOR SDS_MOTION_CONFIG_INFO_COLOR
#define NRF_LOG_DEBUG_COLOR SDS_MOTION_CONFIG_DEBUG_COLOR
#else
#define NRF_LOG_LEVEL       0
#endif
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
NRF_LOG_MODULE_REGISTER();

/**
    * @brief Type used to indicate if the motion module is on or off
*/
typedef enum {
	SAMPLE_OFF,
	SAMPLE_ON,
} motion_state_t;

/**
    * @brief Control block to store rate, state, mode, and callback
*/
static struct motion_control_block_t {
	void (*event_callback) (void * p_context);
	motion_rate_t motion_rate;
	motion_state_t motion_state;
	motion_mode_t motion_mode;
} cb;

static void motion_data_handler(void * p_context);

/**
    * @brief Enumeration of supported sample rates
*/
const uint16_t sample_rate_lookup[MAX_SAMPLE_RATE - MIN_SAMPLE_RATE + 1] = 
{
	(1), 	// _1_HZ
	(2), 	// _2_HZ
	(5), 	// _5_HZ  
	(10), 	// _10_HZ
	(20), 	// _20_HZ
	(40), 	// _40_HZ
	(60), 	// _60_HZ
	(80), 	// _80_HZ
	(100),	// _100_HZ
	(200),  // _200_HZ
};

/**
    * @brief Motion Initialization Function
*/
void sds_motion_init(const motion_init_t * p_motion_init) {
	motion_data_flags_t data_flags = 0;
	
	NRF_LOG_INFO("Motion Initializing.");
	
	/* build motion control block structure */
	cb.event_callback = p_motion_init->event_callback;
	cb.motion_rate = p_motion_init->motion_rate;
	cb.motion_state = SAMPLE_OFF;
	cb.motion_mode = p_motion_init->motion_mode;
	
	/* Set data flags to indicate which data is wanted */
	if (cb.motion_mode == QUATERNION_ONLY) {
		data_flags |= QUATERNION_DATA;
	}
	else if (cb.motion_mode == COLLECT_ALL) {
		data_flags |= QUATERNION_DATA;
		data_flags |= IMU_DATA;
		data_flags |= COMPASS_DATA;
	}
	else {
		APP_ERROR_CHECK(SDS_ERROR);
	}
	
	/* Initialize sensor and set sample rate */
	sds_sensor_init(data_flags, motion_data_handler);
	sds_sensor_set_rate(sample_rate_lookup[cb.motion_rate]);
}

/**
    * @brief Starts Sampling
*/
sds_return_t sds_motion_start(void) {
	
	if (cb.motion_state != SAMPLE_ON) {	
		cb.motion_state = SAMPLE_ON;
		sds_sensor_start();
		return SDS_SUCCESS;
	}
	return SDS_SUCCESS;	
}

/**
    * @brief Stops Sampling
*/
sds_return_t sds_motion_stop(void) {
	
	if (cb.motion_state != SAMPLE_OFF) {	
		cb.motion_state = SAMPLE_OFF;
		sds_sensor_stop();
		return SDS_SUCCESS;
	}
	return SDS_SUCCESS;	
}

/**
    * @brief Gets session info from control block
*/
void get_session_info(session_info_t * p_session_info) {
	p_session_info->motion_mode = cb.motion_mode;
	p_session_info->motion_rate = cb.motion_rate;  
}

/**
    * @brief Gets rate from control block
*/
motion_rate_t sds_motion_get_rate(void) {
	return cb.motion_rate;
}

/**
    * @brief Sets sampling rate. Cannot use during a session.
*/
sds_return_t sds_motion_set_rate(motion_rate_t motion_rate) {

	if ( motion_rate > MAX_SAMPLE_RATE ) {
		return SDS_INVALID_ARG;
	}	
	
    if ( cb.motion_state == SAMPLE_ON ) {
        return SDS_INVALID_STATE;       
    }

	/* set rate to control block */
	cb.motion_rate = motion_rate;
	
	/* pass rate to sensor layer */
	sds_sensor_set_rate(sample_rate_lookup[motion_rate]);
	
	NRF_LOG_INFO("Sample Rate Set to %d Hz.", sample_rate_lookup[motion_rate]);

	return SDS_SUCCESS;
}

sds_return_t sds_motion_run_calibration(void) {
	
	sds_return_t err_code;

	NRF_LOG_INFO("Motion Cal Start.");
	
	/* Blocking function Call to device layer. Will run for a few seconds */
	err_code = sds_sensor_calibrate();

	return err_code;
}


static void motion_data_handler(void * p_context)
{
	/* p_context is a ptr to a static motion_sample_t declared at sensor layer */
	NRF_LOG_DEBUG("Motion Data Event.");		
	cb.event_callback(p_context);
}


