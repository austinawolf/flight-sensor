#include "sds_sensor.h"

#ifdef SENSOR_ICM20948

#include "sds_config.h"

//nrf
#include "nrf_drv_gpiote.h"
#include "boards.h"

//sds
#include "sds_motion_types.h"
#include "sds_clock.h"
#include "imu_cal.h"

//sds_external
#include "inv_mems.h"

#define BIAS_SET_TO_DMP /* Set bias to DMP rather than offset registers */

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


#define STANDARD_ORIENTATION 1
#define UPRIGHT_ORIENTATION 2

/* Self Test */
int a_average[3] = { 0, 0, 0 };
int g_average[3] = { 0, 0, 0 };

/* Temp */
float rv_float[4];

/* Constants */
const unsigned char COMPASS_SLAVE_ID        = HW_AK09916; /* Change the COMPASS_SLAVE_ID to the correct ID of compass used. You can find the defines in inv_mems_hw_config.h*/
const unsigned char COMPASS_CHIP_ADDR       = 0x0C; /* Change COMPASS_CHIP_ADDR to 0x0C for ICM20948 which uses internal AK09916 */
const unsigned char PRESSURE_CHIP_ADDR      = 0x00; /* Change COMPASS_CHIP_ADDR to 0x0E for other AK09912/09911/09913/8963 */
long SOFT_IRON_MATRIX[]                     = {1073741824,0,0,0,1073741824,0,0,0,1073741824}; /* Change SOFT_IRON_MATRIX if necessary (q30) */

#if INITIAL_ORIENTATION == STANDARD_ORIENTATION
signed char ACCEL_GYRO_ORIENTATION[]        = {0,-1,0,1,0,0,0,0,1}; /* Change ACCEL_GYRO_ORIENTATION according to actual mount matrix */
signed char COMPASS_ORIENTATION[]           = {1,0,0,0,-1,0,0,0,-1}; /* Change COMPASS_ORIENTATION according to actual mount matrix */
#elif INITIAL_ORIENTATION == UPRIGHT_ORIENTATION
#error TODO
#else
#error Orientation Not Defined
#endif

const unsigned char ACCEL_GYRO_CHIP_ADDR    = 0x68; /* Change ACCEL_GYRO_CHIP_ADDR if necessary */

/* Data output variables */
static motion_sample_t motion_sample;
static uint8_t sample_number = 0;
//static float orientationFloat[3] = {0.0f,0.0f,0.0f};
//static long orientationQ16[3],temp_orientationQ16[3];
//static float grv_float[4] = {0.0f,0.0f,0.0f}; 

/* Accuracy variables. Not used. */
static int rv_accuracy = 0;
static int accel_accuracy = 0;
static int gyro_accuracy = 0;
static int compass_accuracy = 0;

/* Control block */
static struct sensor_control_block_t {
	uint16_t rate;
	motion_data_flags_t data_flags;
	void (*motion_callback) (void * p_context);
} cb;

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
	
	inv_error_t result;

	/* setup pin change interrupt */
	pin_interrupt_init();

	/* Setup sensor output config */
	cb.data_flags = data_flags;	
	cb.rate = 1;
        cb.motion_callback = motion_callback;
	
	/* Setup accel and gyro mounting matrix and associated angle for current board */
	inv_set_chip_to_body_axis_quaternion(ACCEL_GYRO_ORIENTATION, 0.0);
	result = inv_initialize_lower_driver(SERIAL_INTERFACE_I2C, 0);
	APP_ERROR_CHECK(result);
	inv_set_slave_compass_id(0x24);

        inv_set_gyro_fullscale(3);
	
}

void sds_sensor_start(void) {

	/* start orientation */
	dmp_reset_fifo();
	
	/* enable sensors */
	if (cb.data_flags & QUATERNION_DATA) {
		inv_enable_sensor(ANDROID_SENSOR_ORIENTATION, 1); //ANDROID_SENSOR_ORIENTATION ANDROID_SENSOR_ROTATION_VECTOR
	}
	
	
	if (cb.data_flags & IMU_DATA) {
		inv_enable_sensor(ANDROID_SENSOR_ACCELEROMETER, 1);
		inv_enable_sensor(ANDROID_SENSOR_GYROSCOPE, 1);		
	}
	if (cb.data_flags & COMPASS_DATA) {
		inv_enable_sensor(ANDROID_SENSOR_MAGNETIC_FIELD_UNCALIBRATED, 1);	
	}

	/* set output rate */
	inv_reset_dmp_odr_counters();
	
	NRF_LOG_INFO("Sensor turned ON");	
}

void sds_sensor_stop(void) {
	
	/* stop orientation */
	dmp_reset_fifo();
	
	/* enable sensors */
	if (cb.data_flags & QUATERNION_DATA) {
		inv_enable_sensor(ANDROID_SENSOR_ORIENTATION, 0);
		inv_enable_sensor(ANDROID_SENSOR_ROTATION_VECTOR, 0);

	}
	if (cb.data_flags & IMU_DATA) {
		inv_enable_sensor(ANDROID_SENSOR_ACCELEROMETER, 0);
		inv_enable_sensor(ANDROID_SENSOR_GYROSCOPE, 0);		
	}
	if (cb.data_flags & COMPASS_DATA) {
		inv_enable_sensor(ANDROID_SENSOR_MAGNETIC_FIELD_UNCALIBRATED, 0);	
	}
		
	NRF_LOG_INFO("Sensor turned OFF");
}

void sds_sensor_set_rate(uint32_t sensor_rate) {
	NRF_LOG_DEBUG("Set Sensor Rate.");
	
	/* set output rate */
	uint16_t rate = sensor_rate;
	unsigned short base_rate = 1000; 
	inv_error_t result = 0;
	uint32_t odr;
	
	/* Set rates for each sensor output */
	if (cb.data_flags & QUATERNION_DATA) {
		result = inv_set_odr(ANDROID_SENSOR_ORIENTATION, (unsigned short)(base_rate / rate));
		result = inv_set_odr(ANDROID_SENSOR_ROTATION_VECTOR, (unsigned short)(base_rate / rate));

	}
	if (cb.data_flags & IMU_DATA) {
		result = inv_set_odr(ANDROID_SENSOR_ACCELEROMETER, (unsigned short)(base_rate / rate));
		result = inv_set_odr(ANDROID_SENSOR_GYROSCOPE, (unsigned short)(base_rate / rate));
	}
	if (cb.data_flags & COMPASS_DATA) {
		result = inv_set_odr(ANDROID_SENSOR_MAGNETIC_FIELD_UNCALIBRATED, (unsigned short)(base_rate / rate));
	}
	APP_ERROR_CHECK(result);	
	inv_reset_dmp_odr_counters();
	
	inv_get_odr(ANDROID_SENSOR_ORIENTATION, &odr,  ODR_IN_Ms);
	NRF_LOG_DEBUG("Sample Period = %d ms.", odr);
	
	dmp_reset_fifo();		
}

sds_return_t sds_sensor_calibrate(void) {
	int self_test_result = 0;
	int dmp_bias[9] = { 0 };

	NRF_LOG_INFO("Selftest...Started");

	/* Perform self-test */
	self_test_result = inv_mems_run_selftest();
	NRF_LOG_INFO("Selftest...Done...Ret=%d", self_test_result);
	NRF_LOG_INFO("Result: Compass=%s, Accel=%s, Gyro=%s", (self_test_result & 0x04) ? "PASS" : "FAIL", (self_test_result & 0x02) ? "PASS" : "FAIL", (self_test_result & 0x01) ? "PASS" : "FAIL");
	NRF_LOG_INFO("Accel Average (LSB@FSR 2g)");
	NRF_LOG_INFO("\tX:%d Y:%d Z:%d\r\n", a_average[0], a_average[1], a_average[2]);
	NRF_LOG_INFO("Gyro Average (LSB@FSR 250dps)");
	NRF_LOG_INFO("\tX:%d Y:%d Z:%d", g_average[0], g_average[1], g_average[2]);

	/* Nothing to do if FAIL on gyro and accel */
	if ((self_test_result & 0x03) != 0x03)
		return SDS_ERROR;

	/* Handle bias got by self-test */
	dmp_bias[0] = a_average[0] * (1 << 11);   // Change from LSB to format expected by DMP
	dmp_bias[1] = a_average[1] * (1 << 11);
	dmp_bias[2] = (a_average[2] - 16384) * (1 << 11); //remove the gravity and scale (FSR=2 in selftest)
	int scale = 2000 / 250; //self-test uses 250dps FSR, main() set the FSR to 2000dps
	dmp_bias[3] = g_average[0] * (1 << 15) / scale;
	dmp_bias[4] = g_average[1] * (1 << 15) / scale;
	dmp_bias[5] = g_average[2] * (1 << 15) / scale;

	NRF_LOG_INFO("Factory Cal - Accel DMP biases: \tX:%d Y:%d Z:%d", dmp_bias[0], dmp_bias[1], dmp_bias[2]);
	NRF_LOG_INFO("Factory Cal - Gyro DMP biases:  \tX:%d Y:%d Z:%d\r\n", dmp_bias[3], dmp_bias[4], dmp_bias[5]);
	
	/* Update bias on DMP memory */
	dmp_set_bias(dmp_bias);
	NRF_LOG_INFO("\r\nSetting the DMP biases with one-axis factory calibration values...done\r\n");
	
	return SDS_SUCCESS;
}

static void push_sample(void) {
	/* Push sample to main */	
	NRF_LOG_DEBUG("Push Sample.");	
	cb.motion_callback(&motion_sample);
}

static void fifo_handler(void)
{
	NRF_LOG_DEBUG("Fifo handler call.");
	
	short int_read_back = 0;
	unsigned short header = 0, header2 = 0;
	int data_left_in_fifo = 0;
	short short_data[3] = { 0 };
	signed long  long_data[3] = { 0 };
	signed long  long_quat[3] = { 0 };
	unsigned short sample_cnt_array[GENERAL_SENSORS_MAX] = { 0 };
	
	/* Process Incoming INT and Get/Pack FIFO Data */
	inv_identify_interrupt(&int_read_back);

	if (int_read_back & (BIT_MSG_DMP_INT | BIT_MSG_DMP_INT_0 | BIT_MSG_DMP_INT_2 | BIT_MSG_DMP_INT_5)) {
		
		/* Read FIFO contents and parse it. */
		unsigned short total_sample_cnt = 0;

		do {
			if (inv_mems_fifo_swmirror(&data_left_in_fifo, &total_sample_cnt, sample_cnt_array))
				break;
			
			while (total_sample_cnt--){
				if (inv_mems_fifo_pop(&header, &header2, &data_left_in_fifo))
					break;
			
				/* Clear motion sample type */
				memset(&motion_sample, 0, sizeof(motion_sample));
				
				/* Load timestamp, sample number, and data flags */
				sds_get_ms(&motion_sample.timestamp);
				motion_sample.event = sample_number++;
				motion_sample.data_flags = cb.data_flags;
				NRF_LOG_DEBUG("Sample Event @ %d ms.", motion_sample.timestamp);
				
				/* Calc Period for Debugging */
				// static unsigned long previous;
				// NRF_LOG_DEBUG("Sample Period=%d", motion_sample.timestamp - previous);
				// previous = motion_sample.timestamp;
				
				/* Get Accelerometer Data */
				if (header & ACCEL_SET) {
					NRF_LOG_DEBUG("Get Accel.");
					
					dmp_get_accel(long_data);
					
					/* shift from q30 to q15 */
					motion_sample.accel[0] = long_data[0] >> 15;
					motion_sample.accel[1] = long_data[1] >> 15;
					motion_sample.accel[2] = long_data[2] >> 15;					
					
					accel_accuracy = inv_get_accel_accuracy();
					UNUSED_VARIABLE(accel_accuracy);
					
					NRF_LOG_DEBUG("Accel: x=%d, y=%d, z=%d", motion_sample.accel[0], motion_sample.accel[1], motion_sample.accel[2]);

				}

				/* Get Gyroscope Data */
				if (header & GYRO_SET) {
					signed long  bias_data[3] = { 0 };
					signed long  raw_data[3] = { 0 };					
					
					NRF_LOG_DEBUG("Get Gyro.");										
					
					/* get raw */
					dmp_get_raw_gyro(short_data);
					raw_data[0] = (long)short_data[0];
					raw_data[1] = (long)short_data[1];
					raw_data[2] = (long)short_data[2];
					
					/* get bias */
					dmp_get_gyro_bias(short_data);
					bias_data[0] = (long)short_data[0];
					bias_data[1] = (long)short_data[1];
					bias_data[2] = (long)short_data[2];

					/* get calibrated gyro */
					raw_data[0] = raw_data[0] << 5;
					raw_data[1] = raw_data[1] << 5;
					raw_data[2] = raw_data[2] << 5;
					inv_mems_dmp_get_calibrated_gyro(long_data, raw_data, bias_data);
					
					/* shift from q20 back to q15 */
					motion_sample.gyro[0] = long_data[0] >> 5;
					motion_sample.gyro[1] = long_data[1] >> 5;
					motion_sample.gyro[2] = long_data[2] >> 5;
					
					gyro_accuracy = inv_get_gyro_accuracy();
					UNUSED_VARIABLE(gyro_accuracy);
					
					NRF_LOG_DEBUG("Gyro: x=%d, y=%d, z=%d", motion_sample.gyro[0], motion_sample.gyro[1], motion_sample.gyro[2]);
				}

				/* Get Cal Compass Data */								
				if (header & CPASS_CALIBR_SET) {
					NRF_LOG_DEBUG("Get Cal Compass.");
					
					dmp_get_calibrated_compass(long_data);
					compass_accuracy = inv_get_mag_accuracy();
					UNUSED_VARIABLE(compass_accuracy);
					
					NRF_LOG_DEBUG("Compass: x=%d, y=%d, z=%d", long_data[0], long_data[1], long_data[2]);
				}

				/* Get Raw Compass Data */
				if (header & CPASS_SET) {
					NRF_LOG_DEBUG("Get Raw Compass.");
					
					dmp_get_raw_compass(long_data);
					//dmp_get_calibrated_compass(long_data);
					
					/* convert q30 to q15 */
					motion_sample.compass[0] = long_data[0] >> 15;
					motion_sample.compass[1] = long_data[1] >> 15;
					motion_sample.compass[2] = long_data[2] >> 15;
					
					NRF_LOG_DEBUG("Compass: x=%d, y=%d, z=%d", motion_sample.compass[0], motion_sample.compass[1], motion_sample.compass[2]);					
				}
				
				/* 9axis orientation quaternion sample available from DMP FIFO */
				if (header & QUAT9_SET) {
					NRF_LOG_DEBUG("Get Quat9.");
					
					dmp_get_9quaternion(long_quat);
					rv_accuracy = (int)((float)inv_get_rv_accuracy() / (float)(1ULL << (29)));
					inv_convert_rotation_vector(long_quat, rv_float);
					inv_compute_scalar_part(long_quat, (long *) &motion_sample.quat);
					UNUSED_VARIABLE(rv_accuracy);
					
					/* Used to calculate local Euler angles */
					//inv_convert_rotation_vector_1(long_quat, temp_orientationQ16);
					//inv_mems_augmented_sensors_get_orientation(orientationQ16, temp_orientationQ16);
					
					//orientationFloat[0] = inv_q16_to_float(orientationQ16[0]);
					//orientationFloat[1] = inv_q16_to_float(orientationQ16[1]);
					//orientationFloat[2] = inv_q16_to_float(orientationQ16[2]);
					
					//NRF_LOG_DEBUG("9-axis Quat: q0=%i,q1=%i,q2=%i,q3=%i, acc=%i",quat[0], quat[1], quat[2], quat[3], rv_accuracy);					
					//NRF_LOG_RAW_INFO("Euler: " NRF_LOG_FLOAT_MARKER ", ", NRF_LOG_FLOAT(orientationFloat[0]));
					//NRF_LOG_RAW_INFO(NRF_LOG_FLOAT_MARKER ", ", NRF_LOG_FLOAT(orientationFloat[1]));
					//NRF_LOG_RAW_INFO(NRF_LOG_FLOAT_MARKER "\r\n", NRF_LOG_FLOAT(orientationFloat[2]));

				}
				
				push_sample();

			}
			
			NRF_LOG_DEBUG("Data left in fifo: %d", data_left_in_fifo);
		} while (data_left_in_fifo);


		if (int_read_back & BIT_MSG_DMP_INT_3) {
			NRF_LOG_DEBUG("Step Detected>>>>>>>\r\n");
		}

		if (header & PED_STEPDET_SET) {

			unsigned long steps = 0;
			static unsigned long old_steps;
			dmp_get_pedometer_num_of_steps(&steps);
			if (steps != old_steps) {
				NRF_LOG_DEBUG("\tStep Counter %d\r\n", steps);
				old_steps = steps;
			}
		}

		if (int_read_back & BIT_MSG_DMP_INT_2) {
			NRF_LOG_DEBUG(">> SMD Interrupt *********\r\n");
		}
		
		
	}
}

#endif
