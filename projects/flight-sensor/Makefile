PROJECT_NAME     := flight-sensor
TARGETS          := nrf52840_xxaa
OUTPUT_DIRECTORY := _build

PROJ_DIR := .
ROOT_DIR := ./../../
COMMON_ROOT := ./../../common
SDK_ROOT := ./../../nRF5_SDK
EXTERNAL_ROOT := ./../../external
SOURCE_ROOT := $(PROJ_DIR)/source
BUILD_DIR := $(ROOT_DIR)/build

$(OUTPUT_DIRECTORY)/nrf52840_xxaa.out: \
  LINKER_SCRIPT  := ble_app_hrs_gcc_nrf52.ld

# Source files common to all targets
SRC_FILES += $(PROJ_DIR)/main.c
SRC_FILES += $(SOURCE_ROOT)/command_handler/command_handler.c
SRC_FILES += $(SOURCE_ROOT)/ble_helper/ble_helper.c
SRC_FILES += $(SOURCE_ROOT)/session_manager/session_manager.c
SRC_FILES += $(SOURCE_ROOT)/session_manager/session_state_machine.c
SRC_FILES += $(COMMON_ROOT)/logger/logger.c
SRC_FILES += $(COMMON_ROOT)/ble_imu/ble_imu.c
SRC_FILES += $(COMMON_ROOT)/ble_imu/ble_imu_encoder.c
SRC_FILES += $(COMMON_ROOT)/icm20948/icm20948.c
SRC_FILES += $(COMMON_ROOT)/imu/imu_icm20948.c
SRC_FILES += $(COMMON_ROOT)/imu/imu_simulated.c
SRC_FILES += $(COMMON_ROOT)/timestamp/timestamp.c
SRC_FILES += $(COMMON_ROOT)/queue/queue.c
SRC_FILES += $(COMMON_ROOT)/twi/twi.c
SRC_FILES += $(COMMON_ROOT)/state_machine/state_machine.c
SRC_FILES += $(COMMON_ROOT)/session_store/session_store.c
SRC_FILES += $(COMMON_ROOT)/flash/flash.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/common/inv_mems_drv_hook.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/dmp3/inv_mems_interface_mapping.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/driver/inv_mems_augmented_sensors.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/driver/inv_mems_base_control.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/driver/inv_mems_base_driver.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/driver/inv_mems_data_converter.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/driver/inv_mems_load_firmware.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/driver/inv_mems_mpu_fifo_control.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/driver/inv_mems_mpu_selftest.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/driver/inv_mems_secondary_transport.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/driver/inv_mems_slave_compass.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/driver/inv_mems_slave_pressure.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/driver/inv_mems_transport.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/invn/common/ml_math_func.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/invn/common/mlmath.c
SRC_FILES += $(EXTERNAL_ROOT)/icm20948/dmp3a/dmp3Default_20648.c
SRC_FILES += $(SDK_ROOT)/components/ble/ble_advertising/ble_advertising.c
SRC_FILES += $(SDK_ROOT)/components/ble/ble_services/ble_dis/ble_dis.c
SRC_FILES += $(SDK_ROOT)/components/ble/common/ble_advdata.c
SRC_FILES += $(SDK_ROOT)/components/ble/common/ble_conn_params.c
SRC_FILES += $(SDK_ROOT)/components/ble/common/ble_conn_state.c
SRC_FILES += $(SDK_ROOT)/components/ble/common/ble_srv_common.c
SRC_FILES += $(SDK_ROOT)/components/ble/nrf_ble_gatt/nrf_ble_gatt.c
SRC_FILES += $(SDK_ROOT)/components/ble/nrf_ble_qwr/nrf_ble_qwr.c
SRC_FILES += $(SDK_ROOT)/components/ble/peer_manager/auth_status_tracker.c
SRC_FILES += $(SDK_ROOT)/components/ble/peer_manager/gatt_cache_manager.c
SRC_FILES += $(SDK_ROOT)/components/ble/peer_manager/gatts_cache_manager.c
SRC_FILES += $(SDK_ROOT)/components/ble/peer_manager/id_manager.c
SRC_FILES += $(SDK_ROOT)/components/ble/peer_manager/nrf_ble_lesc.c
SRC_FILES += $(SDK_ROOT)/components/ble/peer_manager/peer_data_storage.c
SRC_FILES += $(SDK_ROOT)/components/ble/peer_manager/peer_database.c
SRC_FILES += $(SDK_ROOT)/components/ble/peer_manager/peer_id.c
SRC_FILES += $(SDK_ROOT)/components/ble/peer_manager/peer_manager_handler.c
SRC_FILES += $(SDK_ROOT)/components/ble/peer_manager/peer_manager.c
SRC_FILES += $(SDK_ROOT)/components/ble/peer_manager/pm_buffer.c
SRC_FILES += $(SDK_ROOT)/components/ble/peer_manager/security_dispatcher.c
SRC_FILES += $(SDK_ROOT)/components/ble/peer_manager/security_manager.c
SRC_FILES += $(SDK_ROOT)/components/boards/boards.c
SRC_FILES += $(SDK_ROOT)/components/libraries/atomic_fifo/nrf_atfifo.c
SRC_FILES += $(SDK_ROOT)/components/libraries/atomic_flags/nrf_atflags.c
SRC_FILES += $(SDK_ROOT)/components/libraries/atomic/nrf_atomic.c
SRC_FILES += $(SDK_ROOT)/components/libraries/balloc/nrf_balloc.c
SRC_FILES += $(SDK_ROOT)/components/libraries/bsp/bsp_btn_ble.c
SRC_FILES += $(SDK_ROOT)/components/libraries/bsp/bsp.c
SRC_FILES += $(SDK_ROOT)/components/libraries/button/app_button.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crc16/crc16.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_aes_aead.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_aes.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_chacha_poly_aead.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_ecc.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_ecdh.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_ecdsa.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_eddsa.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_hash.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_hmac.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_init.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_mutex.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_rng.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/cc310/cc310_backend_shared.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_init.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng_mbedtls.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw/nrf_hw_backend_rng.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_chacha_poly_aead.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecc.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecdh.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_ecdsa.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_eddsa.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_hash.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/backend/oberon/oberon_backend_hmac.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_aead.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_aes_shared.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_aes.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecc.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecdh.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_ecdsa.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_eddsa.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_error.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hash.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hkdf.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_hmac.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_init.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_rng.c
SRC_FILES += $(SDK_ROOT)/components/libraries/crypto/nrf_crypto_shared.c
SRC_FILES += $(SDK_ROOT)/components/libraries/experimental_section_vars/nrf_section_iter.c
SRC_FILES += $(SDK_ROOT)/components/libraries/fds/fds.c
SRC_FILES += $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage_sd.c
SRC_FILES += $(SDK_ROOT)/components/libraries/fstorage/nrf_fstorage.c
SRC_FILES += $(SDK_ROOT)/components/libraries/hardfault/hardfault_implementation.c
SRC_FILES += $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_rtt.c
SRC_FILES += $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_serial.c
SRC_FILES += $(SDK_ROOT)/components/libraries/log/src/nrf_log_backend_uart.c
SRC_FILES += $(SDK_ROOT)/components/libraries/log/src/nrf_log_default_backends.c
SRC_FILES += $(SDK_ROOT)/components/libraries/log/src/nrf_log_frontend.c
SRC_FILES += $(SDK_ROOT)/components/libraries/log/src/nrf_log_str_formatter.c
SRC_FILES += $(SDK_ROOT)/components/libraries/mem_manager/mem_manager.c
SRC_FILES += $(SDK_ROOT)/components/libraries/memobj/nrf_memobj.c
SRC_FILES += $(SDK_ROOT)/components/libraries/pwr_mgmt/nrf_pwr_mgmt.c
SRC_FILES += $(SDK_ROOT)/components/libraries/queue/nrf_queue.c
SRC_FILES += $(SDK_ROOT)/components/libraries/ringbuf/nrf_ringbuf.c
SRC_FILES += $(SDK_ROOT)/components/libraries/scheduler/app_scheduler.c
SRC_FILES += $(SDK_ROOT)/components/libraries/sensorsim/sensorsim.c
SRC_FILES += $(SDK_ROOT)/components/libraries/sortlist/nrf_sortlist.c
SRC_FILES += $(SDK_ROOT)/components/libraries/strerror/nrf_strerror.c
SRC_FILES += $(SDK_ROOT)/components/libraries/timer/app_timer2.c
SRC_FILES += $(SDK_ROOT)/components/libraries/timer/drv_rtc.c
SRC_FILES += $(SDK_ROOT)/components/libraries/util/app_error_handler_gcc.c
SRC_FILES += $(SDK_ROOT)/components/libraries/util/app_error_weak.c
SRC_FILES += $(SDK_ROOT)/components/libraries/util/app_error.c
SRC_FILES += $(SDK_ROOT)/components/libraries/util/app_util_platform.c
SRC_FILES += $(SDK_ROOT)/components/libraries/util/nrf_assert.c
SRC_FILES += $(SDK_ROOT)/components/softdevice/common/nrf_sdh_ble.c
SRC_FILES += $(SDK_ROOT)/components/softdevice/common/nrf_sdh_soc.c
SRC_FILES += $(SDK_ROOT)/components/softdevice/common/nrf_sdh.c
SRC_FILES += $(SDK_ROOT)/external/fprintf/nrf_fprintf_format.c
SRC_FILES += $(SDK_ROOT)/external/fprintf/nrf_fprintf.c
SRC_FILES += $(SDK_ROOT)/external/mbedtls/library/aes.c
SRC_FILES += $(SDK_ROOT)/external/mbedtls/library/ctr_drbg.c
SRC_FILES += $(SDK_ROOT)/external/mbedtls/library/platform_util.c
SRC_FILES += $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_printf.c
SRC_FILES += $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c
SRC_FILES += $(SDK_ROOT)/external/segger_rtt/SEGGER_RTT.c
SRC_FILES += $(SDK_ROOT)/external/utf_converter/utf.c
SRC_FILES += $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_clock.c
SRC_FILES += $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_rng.c
SRC_FILES += $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_twi.c
SRC_FILES += $(SDK_ROOT)/integration/nrfx/legacy/nrf_drv_uart.c
SRC_FILES += $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_clock.c
SRC_FILES += $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_gpiote.c
SRC_FILES += $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_qspi.c
SRC_FILES += $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_rng.c
SRC_FILES += $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twi.c
SRC_FILES += $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_twim.c
SRC_FILES += $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uart.c
SRC_FILES += $(SDK_ROOT)/modules/nrfx/drivers/src/nrfx_uarte.c
SRC_FILES += $(SDK_ROOT)/modules/nrfx/drivers/src/prs/nrfx_prs.c
SRC_FILES += $(SDK_ROOT)/modules/nrfx/mdk/gcc_startup_nrf52840.S
SRC_FILES += $(SDK_ROOT)/modules/nrfx/mdk/system_nrf52840.c
SRC_FILES += $(SDK_ROOT)/modules/nrfx/soc/nrfx_atomic.c

# Include folders common to all targets
INC_FOLDERS += ./config
INC_FOLDERS += $(SOURCE_ROOT)/ble_helper
INC_FOLDERS += $(SOURCE_ROOT)/command_handler/
INC_FOLDERS += $(SOURCE_ROOT)/session_manager/
INC_FOLDERS += $(COMMON_ROOT)/
INC_FOLDERS += $(COMMON_ROOT)/ble_imu
INC_FOLDERS += $(COMMON_ROOT)/imu
INC_FOLDERS += $(COMMON_ROOT)/icm20948
INC_FOLDERS += $(COMMON_ROOT)/logger
INC_FOLDERS += $(COMMON_ROOT)/queue
INC_FOLDERS += $(COMMON_ROOT)/timestamp
INC_FOLDERS += $(COMMON_ROOT)/twi
INC_FOLDERS += $(COMMON_ROOT)/state_machine
INC_FOLDERS += $(COMMON_ROOT)/flash
INC_FOLDERS += $(COMMON_ROOT)/session_store
INC_FOLDERS += $(EXTERNAL_ROOT)/icm20948/
INC_FOLDERS += $(EXTERNAL_ROOT)/icm20948/common
INC_FOLDERS += $(EXTERNAL_ROOT)/icm20948/dmp3
INC_FOLDERS += $(EXTERNAL_ROOT)/icm20948/dmp3a
INC_FOLDERS += $(EXTERNAL_ROOT)/icm20948/driver
INC_FOLDERS += $(EXTERNAL_ROOT)/icm20948/invn/common
INC_FOLDERS += $(SDK_ROOT)/components
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_advertising
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_dtm
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_racp
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_ancs_c
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_ans_c
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_bas
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_bas_c
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_cscs
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_cts_c
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_dfu
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_dis
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_gls
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_hids
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_hts
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_ias
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_ias_c
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_lbs
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_lbs_c
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_lls
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_nus
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_nus_c
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_rscs
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_rscs_c
INC_FOLDERS += $(SDK_ROOT)/components/ble/ble_services/ble_tps
INC_FOLDERS += $(SDK_ROOT)/components/ble/common
INC_FOLDERS += $(SDK_ROOT)/components/ble/nrf_ble_gatt
INC_FOLDERS += $(SDK_ROOT)/components/ble/nrf_ble_qwr
INC_FOLDERS += $(SDK_ROOT)/components/ble/peer_manager
INC_FOLDERS += $(SDK_ROOT)/components/boards
INC_FOLDERS += $(SDK_ROOT)/components/libraries/atomic
INC_FOLDERS += $(SDK_ROOT)/components/libraries/atomic_fifo
INC_FOLDERS += $(SDK_ROOT)/components/libraries/atomic_flags
INC_FOLDERS += $(SDK_ROOT)/components/libraries/balloc
INC_FOLDERS += $(SDK_ROOT)/components/libraries/bootloader/ble_dfu
INC_FOLDERS += $(SDK_ROOT)/components/libraries/bsp
INC_FOLDERS += $(SDK_ROOT)/components/libraries/button
INC_FOLDERS += $(SDK_ROOT)/components/libraries/cli
INC_FOLDERS += $(SDK_ROOT)/components/libraries/crc16
INC_FOLDERS += $(SDK_ROOT)/components/libraries/crc32
INC_FOLDERS += $(SDK_ROOT)/components/libraries/crypto
INC_FOLDERS += $(SDK_ROOT)/components/libraries/crypto/backend/cc310
INC_FOLDERS += $(SDK_ROOT)/components/libraries/crypto/backend/cc310_bl
INC_FOLDERS += $(SDK_ROOT)/components/libraries/crypto/backend/cifra
INC_FOLDERS += $(SDK_ROOT)/components/libraries/crypto/backend/mbedtls
INC_FOLDERS += $(SDK_ROOT)/components/libraries/crypto/backend/micro_ecc
INC_FOLDERS += $(SDK_ROOT)/components/libraries/crypto/backend/nrf_hw
INC_FOLDERS += $(SDK_ROOT)/components/libraries/crypto/backend/nrf_sw
INC_FOLDERS += $(SDK_ROOT)/components/libraries/crypto/backend/oberon
INC_FOLDERS += $(SDK_ROOT)/components/libraries/crypto/backend/optiga
INC_FOLDERS += $(SDK_ROOT)/components/libraries/csense
INC_FOLDERS += $(SDK_ROOT)/components/libraries/csense_drv
INC_FOLDERS += $(SDK_ROOT)/components/libraries/delay
INC_FOLDERS += $(SDK_ROOT)/components/libraries/ecc
INC_FOLDERS += $(SDK_ROOT)/components/libraries/experimental_section_vars
INC_FOLDERS += $(SDK_ROOT)/components/libraries/experimental_task_manager
INC_FOLDERS += $(SDK_ROOT)/components/libraries/fds
INC_FOLDERS += $(SDK_ROOT)/components/libraries/fstorage
INC_FOLDERS += $(SDK_ROOT)/components/libraries/gfx
INC_FOLDERS += $(SDK_ROOT)/components/libraries/gpiote
INC_FOLDERS += $(SDK_ROOT)/components/libraries/hardfault
INC_FOLDERS += $(SDK_ROOT)/components/libraries/hci
INC_FOLDERS += $(SDK_ROOT)/components/libraries/led_softblink
INC_FOLDERS += $(SDK_ROOT)/components/libraries/log
INC_FOLDERS += $(SDK_ROOT)/components/libraries/log/src
INC_FOLDERS += $(SDK_ROOT)/components/libraries/low_power_pwm
INC_FOLDERS += $(SDK_ROOT)/components/libraries/mem_manager
INC_FOLDERS += $(SDK_ROOT)/components/libraries/memobj
INC_FOLDERS += $(SDK_ROOT)/components/libraries/mpu
INC_FOLDERS += $(SDK_ROOT)/components/libraries/mutex
INC_FOLDERS += $(SDK_ROOT)/components/libraries/pwm
INC_FOLDERS += $(SDK_ROOT)/components/libraries/pwr_mgmt
INC_FOLDERS += $(SDK_ROOT)/components/libraries/queue
INC_FOLDERS += $(SDK_ROOT)/components/libraries/ringbuf
INC_FOLDERS += $(SDK_ROOT)/components/libraries/scheduler
INC_FOLDERS += $(SDK_ROOT)/components/libraries/sdcard
INC_FOLDERS += $(SDK_ROOT)/components/libraries/sensorsim
INC_FOLDERS += $(SDK_ROOT)/components/libraries/slip
INC_FOLDERS += $(SDK_ROOT)/components/libraries/sortlist
INC_FOLDERS += $(SDK_ROOT)/components/libraries/spi_mngr
INC_FOLDERS += $(SDK_ROOT)/components/libraries/stack_guard
INC_FOLDERS += $(SDK_ROOT)/components/libraries/stack_info
INC_FOLDERS += $(SDK_ROOT)/components/libraries/strerror
INC_FOLDERS += $(SDK_ROOT)/components/libraries/svc
INC_FOLDERS += $(SDK_ROOT)/components/libraries/timer
INC_FOLDERS += $(SDK_ROOT)/components/libraries/twi_mngr
INC_FOLDERS += $(SDK_ROOT)/components/libraries/twi_sensor
INC_FOLDERS += $(SDK_ROOT)/components/libraries/usbd
INC_FOLDERS += $(SDK_ROOT)/components/libraries/usbd/class/audio
INC_FOLDERS += $(SDK_ROOT)/components/libraries/usbd/class/cdc
INC_FOLDERS += $(SDK_ROOT)/components/libraries/usbd/class/cdc/acm
INC_FOLDERS += $(SDK_ROOT)/components/libraries/usbd/class/hid
INC_FOLDERS += $(SDK_ROOT)/components/libraries/usbd/class/hid/generic
INC_FOLDERS += $(SDK_ROOT)/components/libraries/usbd/class/hid/kbd
INC_FOLDERS += $(SDK_ROOT)/components/libraries/usbd/class/hid/mouse
INC_FOLDERS += $(SDK_ROOT)/components/libraries/usbd/class/msc
INC_FOLDERS += $(SDK_ROOT)/components/libraries/util
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/conn_hand_parser
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/conn_hand_parser/ac_rec_parser
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/conn_hand_parser/ble_oob_advdata_parser
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/conn_hand_parser/le_oob_rec_parser
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/connection_handover/ac_rec
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/connection_handover/ble_oob_advdata
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/connection_handover/ble_pair_lib
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/connection_handover/ble_pair_msg
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/connection_handover/common
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/connection_handover/ep_oob_rec
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/connection_handover/hs_rec
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/connection_handover/le_oob_rec
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/generic/message
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/generic/record
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/launchapp
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/parser/message
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/parser/record
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/text
INC_FOLDERS += $(SDK_ROOT)/components/nfc/ndef/uri
INC_FOLDERS += $(SDK_ROOT)/components/nfc/platform
INC_FOLDERS += $(SDK_ROOT)/components/nfc/t2t_lib
INC_FOLDERS += $(SDK_ROOT)/components/nfc/t2t_parser
INC_FOLDERS += $(SDK_ROOT)/components/nfc/t4t_lib
INC_FOLDERS += $(SDK_ROOT)/components/nfc/t4t_parser/apdu
INC_FOLDERS += $(SDK_ROOT)/components/nfc/t4t_parser/cc_file
INC_FOLDERS += $(SDK_ROOT)/components/nfc/t4t_parser/hl_detection_procedure
INC_FOLDERS += $(SDK_ROOT)/components/nfc/t4t_parser/tlv
INC_FOLDERS += $(SDK_ROOT)/components/softdevice/common
INC_FOLDERS += $(SDK_ROOT)/components/softdevice/s140/headers
INC_FOLDERS += $(SDK_ROOT)/components/softdevice/s140/headers/nrf52
INC_FOLDERS += $(SDK_ROOT)/components/toolchain/cmsis/include
INC_FOLDERS += $(SDK_ROOT)/external/fprintf
INC_FOLDERS += $(SDK_ROOT)/external/mbedtls/include
INC_FOLDERS += $(SDK_ROOT)/external/nrf_cc310/include
INC_FOLDERS += $(SDK_ROOT)/external/nrf_oberon
INC_FOLDERS += $(SDK_ROOT)/external/nrf_oberon/include
INC_FOLDERS += $(SDK_ROOT)/external/nrf_tls/mbedtls/nrf_crypto/config
INC_FOLDERS += $(SDK_ROOT)/external/segger_rtt
INC_FOLDERS += $(SDK_ROOT)/external/utf_converter
INC_FOLDERS += $(SDK_ROOT)/integration/nrfx
INC_FOLDERS += $(SDK_ROOT)/integration/nrfx/legacy
INC_FOLDERS += $(SDK_ROOT)/modules/nrfx
INC_FOLDERS += $(SDK_ROOT)/modules/nrfx/drivers/include
INC_FOLDERS += $(SDK_ROOT)/modules/nrfx/hal
INC_FOLDERS += $(SDK_ROOT)/modules/nrfx/mdk

# Libraries common to all targets
LIB_FILES += \
  $(SDK_ROOT)/external/nrf_cc310/lib/cortex-m4/hard-float/libnrf_cc310_0.9.13.a \
  $(SDK_ROOT)/external/nrf_oberon/lib/cortex-m4/hard-float/liboberon_3.0.8.a \

# Optimization flags
OPT = -Og -g3
# Uncomment the line below to enable link time optimization
#OPT += -flto

# C flags common to all targets
CFLAGS += $(OPT)
CFLAGS += -DAPP_TIMER_V2
CFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
CFLAGS += -DBLE_STACK_SUPPORT_REQD
CFLAGS += -DBOARD_PCA_X01
#CFLAGS += -DBOARD_PCA10056
CFLAGS += -DCONFIG_GPIO_AS_PINRESET
CFLAGS += -DFLOAT_ABI_HARD
#CFLAGS += -DMBEDTLS_CONFIG_FILE=\"nrf_crypto_mbedtls_config.h\"
CFLAGS += -DNRF52840_XXAA
CFLAGS += -DNRF_CRYPTO_MAX_INSTANCE_COUNT=1
CFLAGS += -DNRF_SD_BLE_API_VERSION=7
CFLAGS += -DS140
CFLAGS += -DSOFTDEVICE_PRESENT
CFLAGS += -DDEBUG
CFLAGS += -DIMU_ICM20948

# Flags for Invesense Drivers
CFLAGS += -DSENSOR_ICM20948
CFLAGS += -DFLOAT_ABI_HARD
CFLAGS += -DUSE_DMP
CFLAGS += -DEMPL
CFLAGS += -DMPL_LOG_NDEBUG=1
CFLAGS += -DMEMS_20648
CFLAGS += -DMEMS_SECONDARY_DEVICE
CFLAGS += -DMEMS_AUGMENTED_SENSORS
CFLAGS += -DMEMS_WEARABLE_DEVICE

CFLAGS += -mcpu=cortex-m4
CFLAGS += -mthumb -mabi=aapcs
CFLAGS += -Wall
CFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# keep every function in a separate section, this allows linker to discard unused ones
CFLAGS += -ffunction-sections -fdata-sections -fno-strict-aliasing
CFLAGS += -fno-builtin -fshort-enums

# C++ flags common to all targets
CXXFLAGS += $(OPT)
# Assembler flags common to all targets
ASMFLAGS += -g3
ASMFLAGS += -mcpu=cortex-m4
ASMFLAGS += -mthumb -mabi=aapcs
ASMFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
ASMFLAGS += -DAPP_TIMER_V2
ASMFLAGS += -DAPP_TIMER_V2_RTC1_ENABLED
ASMFLAGS += -DBLE_STACK_SUPPORT_REQD
ASMFLAGS += -DBOARD_PCA10056
ASMFLAGS += -DCONFIG_GPIO_AS_PINRESET
ASMFLAGS += -DFLOAT_ABI_HARD
ASMFLAGS += -DNRF52840_XXAA
ASMFLAGS += -DNRF_CRYPTO_MAX_INSTANCE_COUNT=1
ASMFLAGS += -DNRF_SD_BLE_API_VERSION=7
ASMFLAGS += -DS140
ASMFLAGS += -DSOFTDEVICE_PRESENT

# Linker flags
LDFLAGS += $(OPT)
LDFLAGS += -mthumb -mabi=aapcs -L$(SDK_ROOT)/modules/nrfx/mdk -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m4
LDFLAGS += -mfloat-abi=hard -mfpu=fpv4-sp-d16
# let linker dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
LDFLAGS += --specs=nano.specs

nrf52840_xxaa: CFLAGS += -D__HEAP_SIZE=8192
nrf52840_xxaa: CFLAGS += -D__STACK_SIZE=8192
nrf52840_xxaa: ASMFLAGS += -D__HEAP_SIZE=8192
nrf52840_xxaa: ASMFLAGS += -D__STACK_SIZE=8192

# Add standard libraries at the very end of the linker input, after all objects
# that may need symbols provided by these libraries.
LIB_FILES += -lc -lnosys -lm


.PHONY: default help

# Default target - first one defined
default: nrf52840_xxaa

# Print all targets that can be built
help:
	@echo following targets are available:
	@echo		nrf52840_xxaa
	@echo		flash_softdevice
	@echo		sdk_config - starting external tool for editing sdk_config.h
	@echo		flash      - flashing binary

TEMPLATE_PATH := $(SDK_ROOT)/components/toolchain/gcc


include $(TEMPLATE_PATH)/Makefile.common

$(foreach target, $(TARGETS), $(call define_target, $(target)))

.PHONY: flash flash_softdevice erase vscode_config

# Flash the program
flash: default
	@echo Flashing: $(OUTPUT_DIRECTORY)/nrf52840_xxaa.hex
	nrfjprog -f nrf52 --program $(OUTPUT_DIRECTORY)/nrf52840_xxaa.hex --sectorerase
	nrfjprog -f nrf52 --reset

# Flash softdevice
flash_softdevice:
	@echo Flashing: s140_nrf52_7.2.0_softdevice.hex
	nrfjprog -f nrf52 --program $(SDK_ROOT)/components/softdevice/s140/hex/s140_nrf52_7.2.0_softdevice.hex --sectorerase
	nrfjprog -f nrf52 --reset

erase:
	nrfjprog -f nrf52 --eraseall

SDK_CONFIG_FILE := ./config/sdk_config.h
CMSIS_CONFIG_TOOL := $(SDK_ROOT)/external_tools/cmsisconfig/CMSIS_Configuration_Wizard.jar
sdk_config:
	java -jar $(CMSIS_CONFIG_TOOL) $(SDK_CONFIG_FILE)

vscode_config:
	@python -m vscode_ccpp_configurer --path="$(ROOT_DIR)" --includes="$(INC_FOLDERS)"
	@python -m vscode_ccpp_configurer --path="$(ROOT_DIR)" --defines="$(CFLAGS)" --define-prefix="-D"
