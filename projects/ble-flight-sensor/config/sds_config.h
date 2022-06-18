/** @file */ 

#ifndef CONFIG_H
#define CONFIG_H

//ble config
#define DEV_MODE


//*** <<< Use Configuration Wizard in Context Menu >>> ***
//==========================================================

// <o> Data Collection Mode
 
// <0=> Quaternion Only
// <1=> Quaternion, IMU, Compass

#ifndef SDS_MOTION_CONFIG_MODE
#define SDS_MOTION_CONFIG_MODE 1
#endif

// <o> Default Sampling Rate
 
// <0=> 1 Hz
// <1=> 2 Hz
// <2=> 5 Hz
// <3=> 10 Hz
// <4=> 20 Hz
// <5=> 40 Hz
// <6=> 60 Hz
// <7=> 80 Hz
// <8=> 100 Hz
// <9=> 200 Hz


#ifndef SDS_DEFAULT_MOTION_RATE
#define SDS_DEFAULT_MOTION_RATE 4
#endif

// <o> BLE Motion Service UUID Selection - Allows for multiple devices to be used without interfering during testing

// <0=> CLINICAL
// <1=> DEBUG 1
// <2=> DEBUG 2

#ifndef SDS_BLE_MOTION_SERVICE_UUID_SELECTION
#define SDS_BLE_MOTION_SERVICE_UUID_SELECTION 0
#endif

// <o> BLE Connection Interval (ms) - Range: 8-4000

#ifndef CONN_INTERVAL_MS
#define CONN_INTERVAL_MS 20
#endif

//==========================================================
// <q> Sampling On CCCD Write  - Starts sampling with Motion CCCD has been setup to accept notifications
 

#ifndef SAMPLE_ON_CCCD_WRITE
#define SAMPLE_ON_CCCD_WRITE 0
#endif

// <o> Initial Orientation Setting - Defines the orientation that the sensor must be calibrated to
 
// <1=> Level
// <2=> Upright

#ifndef INITIAL_ORIENTATION
#define INITIAL_ORIENTATION 1
#endif

// <o> BLE Motion Observer Priority  
// <i> Priority with which BLE events are dispatched to the Heart Rate Service.

#ifndef BLE_MOTION_BLE_OBSERVER_PRIO
#define BLE_MOTION_BLE_OBSERVER_PRIO 2
#endif

#endif

