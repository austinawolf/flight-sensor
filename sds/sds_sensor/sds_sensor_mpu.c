#include "sds_sensor.h"

#ifdef SENSOR_MPU9250

#include "sds_config.h"

//nrf
#include "nrf_drv_gpiote.h"
#include "boards.h"

//sds
#include "sds_motion_types.h"
#include "sds_clock.h"
#include "imu_cal.h"

//mpu9250 drivers
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "dmpKey.h"
#include "dmpmap.h"

#include "sds_log_config.h"
#define NRF_LOG_MODULE_NAME sensor
#if SDS_SENSOR_CONFIG_LOG_ENABLED
#define NRF_LOG_LEVEL SDS_SENSOR_CONFIG_LOG_LEVEL
#define NRF_LOG_INFO_COLOR SDS_SENSOR_CONFIG_INFO_COLOR
#define NRF_LOG_DEBUG_COLOR SDS_SENSOR_CONFIG_DEBUG_COLOR
#else
#define NRF_LOG_LEVEL       0
#endif
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define MPU_INTERNAL_RATE 100

long gyro_bias[3] = {0,0,0};
const long accel_bias[3] = {0,0,0};

/* Control block */
static struct sensor_control_block_t {
	uint16_t rate;
	motion_data_flags_t data_flags;
	void (*motion_callback) (void * p_context);	
} cb;

/* Data output variables */
static long quat[4] = {0,0,0,0};
static short gyro[3] = {0,0,0};
static short accel[3] = {0,0,0};
static short compass[3] = {0,0,0};
static uint8_t sample_number = 0;

static void fifo_handler(void);
static void pin_interrupt_init(void);
static void pin_change_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action);
static void push_sample(void);

static void pin_change_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) {
	fifo_handler();
}

static void pin_interrupt_init(void) {
   ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);

    err_code = nrf_drv_gpiote_in_init(SENSOR_TWI_INT, &in_config, pin_change_handler);
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_event_enable(SENSOR_TWI_INT, true);	
}

void sds_sensor_init(motion_data_flags_t data_flags, void (*motion_callback) (void * p_context)) {

	NRF_LOG_INFO("Sensor Initializing.");
	
	/* setup chip */
	int ret;
	unsigned char sensors = 0;
    unsigned short dmp_features = 0;
	struct int_param_s int_param =
	{
		.cb = NULL,
		.pin = 0,
		.lp_exit = 0,
		.active_low = 1,
	};    
    
	/* setup pin change interrupt */
	pin_interrupt_init();

	/* Setup sensor output config */
	cb.data_flags = data_flags;
    cb.motion_callback = motion_callback;
  
	/* enable sensors */
	if (cb.data_flags & QUATERNION_DATA) {
		dmp_features |= DMP_FEATURE_6X_LP_QUAT;       
		dmp_features |= DMP_FEATURE_GYRO_CAL;
        sensors |= INV_XYZ_GYRO | INV_XYZ_ACCEL;
	}
	if (cb.data_flags & IMU_DATA) {
        
        dmp_features |= DMP_FEATURE_SEND_RAW_ACCEL | DMP_FEATURE_SEND_CAL_GYRO;		

	}
	if (cb.data_flags & COMPASS_DATA) {
		sensors |= INV_XYZ_COMPASS;
	}    
	
	//mpu init
	ret = mpu_init(&int_param);
	APP_ERROR_CHECK(ret);

	//config
	ret = mpu_set_sensors(sensors);
	APP_ERROR_CHECK(ret);
	
	if (data_flags & QUATERNION_DATA) {

		mpu_set_sample_rate(MPU_INTERNAL_RATE);

		mpu_set_accel_bias_6500_reg(accel_bias);
		mpu_set_gyro_bias_reg(gyro_bias);

		ret = mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
		APP_ERROR_CHECK(ret);
		
		//dmp
		ret = dmp_load_motion_driver_firmware();
		APP_ERROR_CHECK(ret);	

		dmp_register_tap_cb(NULL);
		dmp_register_android_orient_cb(NULL);

		ret = dmp_enable_feature(dmp_features);		
		APP_ERROR_CHECK(ret);	
	}	
}

void sds_sensor_start(void) {

	/* start orientation */
	int32_t ret;
	ret = mpu_set_dmp_state(1);
    APP_ERROR_CHECK(ret);
	NRF_LOG_INFO("Sensor turned ON");	
}

void sds_sensor_stop(void) {
	
	/* stop orientation */
	/* start orientation */
	int32_t ret;
	ret = mpu_set_dmp_state(0);
    APP_ERROR_CHECK(ret);
	NRF_LOG_INFO("Sensor turned OFF");
}

void sds_sensor_set_rate(uint32_t sensor_rate) {
	NRF_LOG_DEBUG("Set Sensor Rate.");
	
	/* set sample rate */
	int32_t ret;
	ret = dmp_set_fifo_rate(sensor_rate);
	APP_ERROR_CHECK(ret);
}

sds_return_t sds_sensor_calibrate(void) {
    /* not implemeted */
    
	return SDS_ERROR;
}

static void push_sample(void) {
	/* Set rates for each sensor output */
	static motion_sample_t motion_sample;
	
	NRF_LOG_DEBUG("Push Sample.");
	
	memset(&motion_sample, 0, sizeof(motion_sample_t));
	
	motion_sample.event = sample_number++;
	motion_sample.data_flags = cb.data_flags;
	
	if (cb.data_flags & QUATERNION_DATA) {
		memcpy(motion_sample.quat,quat,sizeof(quat));
	}
	if (cb.data_flags & IMU_DATA) {
		memcpy(motion_sample.accel,accel,sizeof(accel));
		memcpy(motion_sample.gyro,gyro,sizeof(gyro));		
	}
	if (cb.data_flags & COMPASS_DATA) {
		memcpy(motion_sample.compass,compass,sizeof(compass));	
	}
	sds_get_ms(&motion_sample.timestamp);

	cb.motion_callback(&motion_sample);
	
	return;
}


static void fifo_handler(void)
{
	short sensors;
	uint8_t more;
	timestamp_ms_t temp_timestamp;
	int16_t temp_gyro[3], temp_accel[3], temp_compass[3];
	int32_t temp_quat[4];
	int8_t status;
    

    do {
        status = dmp_read_fifo_mpu(temp_gyro, temp_accel, (long *) temp_quat, (unsigned long *) &temp_timestamp, &sensors, &more);
        if (status) {
            return;
        }
        else {
            NRF_LOG_DEBUG("Sample @ %d ms", temp_timestamp);
            
            /* enable sensors */
            if (cb.data_flags & QUATERNION_DATA) {
                memcpy(quat, temp_quat, sizeof(quat));
            }
            if (cb.data_flags & IMU_DATA) {
                memcpy(accel, temp_accel, sizeof(accel));
                memcpy(gyro, temp_gyro, sizeof(gyro));	
            }
            if (cb.data_flags & COMPASS_DATA) {
                memcpy(compass, temp_compass, sizeof(compass));
            }   
			push_sample();			
        }
		
    }  
    while (more);
        
}

#endif
