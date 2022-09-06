#ifndef SDS_MOTION_H_
#define SDS_MOTION_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include "sdk_common.h"
#include "sds_motion_types.h"
#include "sds_error.h"

typedef struct {
	void (*event_callback) (void * p_context);		
	motion_mode_t motion_mode;
	motion_rate_t motion_rate;    
} motion_init_t;


/**
    * @brief initializes motion drivers, and sensor level drivers
    * @param p_motion_init pointer to init structure
*/
void sds_motion_init(const motion_init_t * p_motion_init);

/**
    * @brief starts motion sample generation
*/
sds_return_t sds_motion_start(void);

/**
    * @brief stops motion sample generation
*/
sds_return_t sds_motion_stop(void);

/**
    * @brief sets sample rate
    * @param motion_rate_t motion rate enumeration
*/
sds_return_t sds_motion_set_rate(motion_rate_t motion_rate);

/**
    * @brief gets sample rate enumeration
*/
motion_rate_t sds_motion_get_rate(void);

/**
    * @brief runs calibration, takes a few seconds
*/
sds_return_t sds_motion_run_calibration(void);

/**
    * @brief gets motion rate and motion motion information
*/
void get_session_info(session_info_t * p_session_info);


#endif
