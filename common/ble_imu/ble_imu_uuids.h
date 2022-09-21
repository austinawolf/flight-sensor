#ifndef BLE_IMU_UUIDS_H__
#define BLE_IMU_UUIDS_H__

#include <stdint.h>
#include <stdbool.h>

#define IGNORED 0x00

#define BLE_UUID_IMU_SERVICE_BASE 	    {0x03,0xb0,0xf8,0x4b,0x02,0xd9,0xa9,0xb9,0xca,0x40,0x2c,0xc3,0xFF,0x18,0x44,0x92} //{924418ff-c32c-40ca-b9a9-d9024bf8b003}
#define BLE_UUID_IMU_SERVICE            0x18FF

#define BLE_UUID_DATA_CHAR_BASE 	    {0x01,0x12,0xc9,0x8e,0x8c,0x67,0x60,0xb6,0xbb,0x41,0x64,0x82,IGNORED,IGNORED,0x19,0x43} //{43192aff-8264-41bb-b660-678c8ec91201}
#define BLE_UUID_DATA_CHAR              0x2AFF
#define BLE_UUID_COMMAND_CHAR_BASE 		{0x01,0xad,0x59,0xa1,0x0f,0x1e,0x48,0xa5,0x17,0x4a,0xc0,0xa7,IGNORED,IGNORED,0x54,0xd9} //{d9541770-a7c0-4a17-a548-1e0fa159ad01}
#define BLE_UUID_COMMAND_CHAR           0x1770

#endif
