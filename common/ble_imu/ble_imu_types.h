/**
 * @file    logger.h
 * @author  Austin Wolf
 * @brief
 */

#ifndef BLE_IMU_TYPES_H_
#define BLE_IMU_TYPES_H_

#include <stdint.h>
#include <stdbool.h>


#define IMU_DATA_PREAMBLE     (0xDD)

#define IMU_DATA_SIZE         (40)

/**
 * @brief 
 * 
 */
typedef union
{
    uint8_t bytes[IMU_DATA_SIZE];
    struct __attribute__((packed))
    {   
        uint32_t timestamp;
        uint8_t preamble;
        uint8_t flags;
        int32_t quat[4];
        int16_t gyro[3];
        int16_t accel[3];
        int16_t compass[3];
        // uint16_t index;
    } field;
} ble_imu_data_t;

#endif
