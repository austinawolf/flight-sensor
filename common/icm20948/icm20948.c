/**
 * @file    logger.c
 * @author  Austin Wolf
 * @brief
 */
#include "icm20948.h"
#include "boards.h"
#include "nrf_drv_gpiote.h"
#include "logger.h"
#include "inv_mems.h"
#include "datastore.h"

#define BIAS_SET_TO_DMP /* Set bias to DMP rather than offset registers */


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

signed char ACCEL_GYRO_ORIENTATION[]        = {0,-1,0,1,0,0,0,0,1}; /* Change ACCEL_GYRO_ORIENTATION according to actual mount matrix */
signed char COMPASS_ORIENTATION[]           = {1,0,0,0,-1,0,0,0,-1}; /* Change COMPASS_ORIENTATION according to actual mount matrix */

const unsigned char ACCEL_GYRO_CHIP_ADDR    = 0x68; /* Change ACCEL_GYRO_CHIP_ADDR if necessary */

/* Control Variables */
static icm20948_event_callback_t _event_callback = NULL;
static bool _imu_enabled = true;
static bool _compass_enabled = true;
static bool _quat_enabled = true;


/**
 * @brief Executes event callback if registered
 */
static void _on_sample_ready(void)
{
    if (_event_callback != NULL)
    {
        _event_callback();
    }
}

/**
 * @brief interrupt singal handler that indicates a sample is ready
 */
static void pin_change_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action) 
{
    _on_sample_ready();
}

/**
 * @brief Gets a sample from the icm20948 sample buffer
 */
static status_e _get_sample(icm20948_data_t *data, bool *sample_ready)
{
    unsigned short header = 0, header2 = 0;
    int data_left_in_fifo = 0;
    unsigned short sample_cnt_array[GENERAL_SENSORS_MAX] = { 0 };
    
    /* Read FIFO contents and parse it. */
    unsigned short total_sample_cnt = 0;
    if (inv_mems_fifo_swmirror(&data_left_in_fifo, &total_sample_cnt, sample_cnt_array))
    {
        return STATUS_ERROR;
    }
    
    if (total_sample_cnt == 0)
    {
        return STATUS_ERROR;
    }

    if (inv_mems_fifo_pop(&header, &header2, &data_left_in_fifo))
    {
        return STATUS_ERROR;
    }
        
    /* Get Accelerometer Data */
    if (header & ACCEL_SET) 
    {
        signed long long_data[3] = { 0 };
        
        dmp_get_accel(long_data);
        
        /* shift from q30 to q15 */
        data->accel[0] = long_data[0] >> 15;
        data->accel[1] = long_data[1] >> 15;
        data->accel[2] = long_data[2] >> 15;                    
        data->accel_accuracy = inv_get_accel_accuracy();
        data->has_accel = true;
    }

    /* Get Gyroscope Data */
    if (header & GYRO_SET) 
    {
        short short_data[3] = { 0 };
        signed long long_data[3] = { 0 };
        signed long  bias_data[3] = { 0 };
        signed long  raw_data[3] = { 0 };                    
        
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
        data->gyro[0] = long_data[0] >> 5;
        data->gyro[1] = long_data[1] >> 5;
        data->gyro[2] = long_data[2] >> 5;
        data->gyro_accuracy = inv_get_gyro_accuracy();
        data->has_gyro = true;
    }

    /* Get Cal Compass Data */                                
    if (header & CPASS_CALIBR_SET) {
        signed long long_data[3] = { 0 };
        dmp_get_calibrated_compass(long_data);
        data->compass_accuracy = inv_get_mag_accuracy();
    }

    /* Get Raw Compass Data */
    if (header & CPASS_SET)
    {
        signed long long_data[3] = { 0 };
        dmp_get_raw_compass(long_data);
        
        /* convert q30 to q15 */
        data->compass[0] = long_data[0] >> 15;
        data->compass[1] = long_data[1] >> 15;
        data->compass[2] = long_data[2] >> 15;
        data->has_compass = true;
    }
    
    /* 9axis orientation quaternion sample available from DMP FIFO */
    if (header & QUAT9_SET)
    {
        signed long  long_quat[3] = { 0 };
        dmp_get_9quaternion(long_quat);
        data->quat_accuracy = inv_get_rv_accuracy();
        inv_convert_rotation_vector(long_quat, rv_float);
        inv_compute_scalar_part(long_quat, (long *) &data->quat);
        data->has_quat = true;
    }

    *sample_ready = data_left_in_fifo != 0;

    return STATUS_OK;
}

/**
 * @brief 
 */
static void _set_calibration(icm20948_calibration_t *data)
{
    int dmp_bias[9] = 
    { 
        data->accel_bias[0],
        data->accel_bias[1],
        data->accel_bias[2],
        data->gyro_bias[0],
        data->gyro_bias[1],
        data->gyro_bias[2],
        0,
        0,
        0,
    };

    /* Update bias on DMP memory */
    dmp_set_bias(dmp_bias);
}

/**
 * @see icm20948.h
 */
status_e icm20948_create(void)
{
    inv_error_t result;

    /* setup pin change interrupt */
    nrf_drv_gpiote_in_config_t in_config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
    ret_code_t err_code = nrf_drv_gpiote_in_init(SENSOR_TWI_INT, &in_config, pin_change_handler);
    APP_ERROR_CHECK(err_code);
    nrf_drv_gpiote_in_event_enable(SENSOR_TWI_INT, true);    
    
    /* Setup accel and gyro mounting matrix and associated angle for current board */
    inv_set_chip_to_body_axis_quaternion(ACCEL_GYRO_ORIENTATION, 0.0);
    result = inv_initialize_lower_driver(SERIAL_INTERFACE_I2C, 0);
    APP_ERROR_CHECK(result);
    inv_set_slave_compass_id(0x24);

    icm20948_calibration_t cal = {0};
    status_e status = datastore_get_cal_data(&cal);
    if (STATUS_OK != status)
    {
        LOG_ERROR("datastore_get_cal_data failed: %d", status);
        return status;
    }

    _set_calibration(&cal);

    // inv_error_t err = inv_set_gyro_sf(0, 3);
    // APP_ERROR_CHECK(err);

    return STATUS_OK;
}

/**
 * @see icm20948.h
 */
status_e icm20948_start(void)
{    
    /* start orientation */
    inv_error_t err = dmp_reset_fifo();
    if (err != 0)
    {
        LOG_ERROR("dmp_reset_fifo failed, err: %d", err);
    }

    /* enable sensors */
    if (_quat_enabled) 
    {
        err = inv_enable_sensor(ANDROID_SENSOR_ORIENTATION, 1); //ANDROID_SENSOR_ORIENTATION ANDROID_SENSOR_ROTATION_VECTOR
    }
    
    if (_imu_enabled)
    {
        inv_enable_sensor(ANDROID_SENSOR_ACCELEROMETER, 1);
        inv_enable_sensor(ANDROID_SENSOR_GYROSCOPE, 1);        
    }
    
    if (_compass_enabled)
    {
        inv_enable_sensor(ANDROID_SENSOR_MAGNETIC_FIELD_UNCALIBRATED, 1);    
    }

    /* set output rate */
    inv_reset_dmp_odr_counters();

    return STATUS_OK;
}

/**
 * @see icm20948.h
 */
status_e icm20948_stop(void)
{
    /* stop orientation */
    dmp_reset_fifo();
    
    /* enable sensors */
    if (_quat_enabled) {
        inv_enable_sensor(ANDROID_SENSOR_ORIENTATION, 0);
        inv_enable_sensor(ANDROID_SENSOR_ROTATION_VECTOR, 0);

    }
    if (_imu_enabled) {
        inv_enable_sensor(ANDROID_SENSOR_ACCELEROMETER, 0);
        inv_enable_sensor(ANDROID_SENSOR_GYROSCOPE, 0);        
    }
    if (_compass_enabled) {
        inv_enable_sensor(ANDROID_SENSOR_MAGNETIC_FIELD_UNCALIBRATED, 0);    
    }

    return STATUS_OK;
}

/**
 * @see icm20948.h
 */
status_e icm20948_set_rate(uint16_t rate) 
{    
    /* set output rate */
    unsigned short base_rate = 1000; 
    inv_error_t result = 0;
    uint32_t odr;
    
    /* Set rates for each sensor output */
    if (_quat_enabled) {
        result = inv_set_odr(ANDROID_SENSOR_ORIENTATION, (unsigned short)(base_rate / rate));
        result = inv_set_odr(ANDROID_SENSOR_ROTATION_VECTOR, (unsigned short)(base_rate / rate));

    }
    if (_imu_enabled) {
        result = inv_set_odr(ANDROID_SENSOR_ACCELEROMETER, (unsigned short)(base_rate / rate));
        result = inv_set_odr(ANDROID_SENSOR_GYROSCOPE, (unsigned short)(base_rate / rate));
    }
    if (_compass_enabled) {
        result = inv_set_odr(ANDROID_SENSOR_MAGNETIC_FIELD_UNCALIBRATED, (unsigned short)(base_rate / rate));
    }
    APP_ERROR_CHECK(result);    
    inv_reset_dmp_odr_counters();
    
    inv_get_odr(ANDROID_SENSOR_ORIENTATION, &odr,  ODR_IN_Ms);
    
    dmp_reset_fifo();

    return STATUS_ERROR;
}

/**
 * @see icm20948.h
 */
status_e icm20948_read(icm20948_data_t *data, bool *sample_ready)
{
    return _get_sample(data, sample_ready);
}

/**
 * @see icm20948.h
 */
status_e icm20948_calibrate(void) {
    int16_t self_test_result = 0;
    
    /* Perform self-test */
    self_test_result = inv_mems_run_selftest();
    
    /* Log results */
    LOG_INFO("Compass=%s, Accel=%s, Gyro=%s", (self_test_result & 0x04) ? "PASS" : "FAIL", (self_test_result & 0x02) ? "PASS" : "FAIL", (self_test_result & 0x01) ? "PASS" : "FAIL");
    LOG_INFO("Accel (LSB@FSR 2g)");
    LOG_INFO("\tX:%d Y:%d Z:%d\r\n", a_average[0], a_average[1], a_average[2]);
    LOG_INFO("Gyro (LSB@FSR 250dps)");
    LOG_INFO("\tX:%d Y:%d Z:%d", g_average[0], g_average[1], g_average[2]);

    /* Nothing to do if FAIL on gyro and accel */
    if ((self_test_result & 0x03) != 0x03)
        return STATUS_ERROR;

    int scale = 2000 / 250; //self-test uses 250dps FSR, main() set the FSR to 2000dps

    // TODO: remove magic numbers
    icm20948_calibration_t cal =
    {
        .accel_bias = {
            a_average[0] * (1 << 11), 
            a_average[1] * (1 << 11), 
            (a_average[2] - 16384) * (1 << 11)
        },
        .gyro_bias = {
            g_average[0] * (1 << 15) / scale, 
            g_average[1] * (1 << 15) / scale, 
            g_average[2] * (1 << 15) / scale
        }
    };

    LOG_INFO("Accel biases: X:%d Y:%d Z:%d", cal.accel_bias[0], cal.accel_bias[1], cal.accel_bias[2]);
    LOG_INFO("Gyro biases: X:%d Y:%d Z:%d", cal.gyro_bias[0], cal.gyro_bias[1], cal.gyro_bias[2]);

    _set_calibration(&cal);

    (void) datastore_set_cal_data(&cal);
    (void) datastore_flush();

    return STATUS_OK;
}

/**
 * @see icm20948.h
 */
status_e icm20948_register_event_callback(icm20948_event_callback_t callback)
{
    _event_callback = callback;

    return STATUS_OK;
}
