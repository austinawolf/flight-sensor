/** @file */ 

#include "sds_memory.h"
#include "nrf_drv_qspi.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"

#include "sds_clock.h"

#ifdef USE_SDS_LOG_CONFIG
#include "sds_log_config.h"
#define NRF_LOG_MODULE_NAME mem
#if SDS_MEM_CONFIG_LOG_ENABLED
#define NRF_LOG_LEVEL SDS_MEM_CONFIG_LOG_LEVEL
#define NRF_LOG_INFO_COLOR SDS_MEM_CONFIG_INFO_COLOR
#define NRF_LOG_DEBUG_COLOR SDS_MEM_CONFIG_DEBUG_COLOR
#else
#define NRF_LOG_LEVEL       0
#endif
#endif
#include "nrf_log.h"
NRF_LOG_MODULE_REGISTER();

#define QSPI_STD_CMD_WRSR   0x01
#define QSPI_STD_CMD_RSTEN  0x66
#define QSPI_STD_CMD_RST    0x99
#define QSPI_TEST_BLOCK_SIZE 4096
#define SESSION_INFO_BLOCK_INDEX 0
#define MIN_BLOCK_INDEX 2
#define MAX_BLOCK_INDEX 16384
#define SESSION_INFO_PREAMBLE 0xAABBCCDD

#define WAIT_FOR_PERIPH() do { \
        while (!cb.m_finished) {} \
        cb.m_finished = false;    \
    } while (0)

/**
    @brief 	If you try to write less than four bytes to memory the write will never return success.
			To fix this problem, a four byte preamble is added to the session info before it is written.
 */       
typedef struct {
	uint32_t preamble;
	session_info_t session_info;
} session_info_block_buffer_t;

/**
    @brief hold memory control info
 */         
typedef enum
{
    SDS_MEMORY_MODE_IDLE,
    SDS_MEMORY_MODE_SESSION,
    SDS_MEMORY_MODE_STREAM,
} memory_state_t; 
    
/**
    @brief Write buffer
 */    
memory_block_t m_buffer_tx;

/**
    @brief Read buffer
 */    
memory_block_t m_buffer_rx;
    
/**
    @brief hold memory control info
 */       
static struct {
    void (*event_callback) (sds_memory_evt_t event, void * p_context);
    uint32_t block_index;
    uint32_t buffer_data_index;
    uint32_t data_size;   
    bool use_next_timestamp;
	motion_mode_t motion_mode;
    volatile memory_state_t memory_state;
    volatile bool m_finished;
	uint8_t max_samples_per_block;
} cb;

static void configure_memory(void);
static void qspi_handler(nrf_drv_qspi_evt_t event, void * p_context);
static void erase_block(void);
static uint32_t write_buffer_to_block(void);
static uint32_t read_block_to_buffer(void);
static uint32_t verify_write(void);    
static void inc_block_index(void);
static void reset_block_index(void);
static void initialize_block(void);
static sds_return_t read_session_info(session_info_t * p_session_info);
static void write_session_info(session_info_t * p_session_info);
static sds_return_t get_next_sample(motion_sample_t * p_motion_sample);

void sds_mem_init(memory_init_s * p_memory_init) {
    
    uint32_t err_code;

    NRF_LOG_INFO("Initializing Memory.");    
    
    nrf_drv_qspi_config_t config = NRF_DRV_QSPI_DEFAULT_CONFIG;
    config.pins.sck_pin = BSP_QSPI_SCK_PIN;
    config.pins.csn_pin = BSP_QSPI_CSN_PIN;
    config.pins.io1_pin = BSP_QSPI_IO0_PIN;
    config.pins.io2_pin = BSP_QSPI_IO1_PIN;
    config.pins.io2_pin = BSP_QSPI_IO2_PIN;
    config.pins.io3_pin = BSP_QSPI_IO3_PIN;

    cb.event_callback = p_memory_init->event_callback;
    cb.memory_state = SDS_MEMORY_MODE_IDLE;
    cb.m_finished = false;
    
    err_code = nrf_drv_qspi_init(&config, qspi_handler, NULL);
    APP_ERROR_CHECK(err_code);

    configure_memory();
  
}

sds_return_t sds_mem_start_session(session_info_t * p_session_info) {

    NRF_LOG_DEBUG("Starting Memory Session.");        
    
	if (cb.memory_state != SDS_MEMORY_MODE_IDLE) {
		return SDS_INVALID_STATE;
	}
	    
	/* Setup control block */
    cb.motion_mode = p_session_info->motion_mode;
    
    cb.use_next_timestamp = false;
    cb.block_index = MIN_BLOCK_INDEX;
    cb.memory_state = SDS_MEMORY_MODE_SESSION;
    
	if (cb.motion_mode == QUATERNION_ONLY) {
		cb.data_size = QUATERNION_ONLY_DATA_SIZE;
		cb.max_samples_per_block = MAX_QUATERNION_ONLY_SAMPLES_PER_BLOCK;
	}
	else if (cb.motion_mode == COLLECT_ALL) {
		cb.data_size = COLLECT_ALL_DATA_SIZE;
		cb.max_samples_per_block = MAX_COLLCT_ALL_SAMPLES_PER_BLOCK;
    }
	else {
		APP_ERROR_CHECK(SDS_ERROR);
	}
	
	/* Write session info to session info block */
    write_session_info(p_session_info);
    
	/* Reset block index and setup first block for writing */
    reset_block_index();
    initialize_block();
        
    cb.event_callback(SDS_MEMORY_EVT_SESSION_STARTED, NULL);    
    return SDS_SUCCESS;
}

sds_return_t sds_mem_stop_session(void) {

    NRF_LOG_DEBUG("Starting Memory Session.");    
    
	if (cb.memory_state != SDS_MEMORY_MODE_SESSION) {
		return SDS_INVALID_STATE;
	}
    
	write_buffer_to_block();
	read_block_to_buffer();
	verify_write();
	inc_block_index();
	initialize_block(); 	
	
	
    cb.memory_state = SDS_MEMORY_MODE_IDLE;
    
    cb.event_callback(SDS_MEMORY_EVT_SESSION_TERMINATED, NULL);
    
    return SDS_SUCCESS;
}



sds_return_t sds_mem_save_motion_data(motion_sample_t * p_motion_sample) { 
    
	if (cb.memory_state != SDS_MEMORY_MODE_SESSION) {
		return SDS_INVALID_STATE;
	}
	
		// Blink LED 2 every time the data saved in memory 
		bsp_board_led_invert(2);  // YUSUF
	
	if (m_buffer_tx.sample_num == 0) {
		NRF_LOG_DEBUG("Block Timestamped @ %d ms", p_motion_sample->timestamp);
		m_buffer_tx.block_timestamp = p_motion_sample->timestamp;
	}
    
    switch (cb.motion_mode) {
        
        case QUATERNION_ONLY:
			NRF_LOG_DEBUG("Quat Save. index=%d.", m_buffer_tx.sample_num);
			memcpy(&m_buffer_tx.data.quaternion_only[m_buffer_tx.sample_num],p_motion_sample->quat, sizeof(p_motion_sample->quat));
			NRF_LOG_DEBUG("q0=%i,q1=%i,q2=%i,q3=%i",m_buffer_tx.data.quaternion_only[m_buffer_tx.sample_num][0],\
														m_buffer_tx.data.quaternion_only[m_buffer_tx.sample_num][1],\
														m_buffer_tx.data.quaternion_only[m_buffer_tx.sample_num][2],\
														m_buffer_tx.data.quaternion_only[m_buffer_tx.sample_num][3]);
            break;
		
        case COLLECT_ALL:
			NRF_LOG_DEBUG("CA Save. index=%d.", m_buffer_tx.sample_num);
			memcpy(&m_buffer_tx.data.collect_all[m_buffer_tx.sample_num].quat,p_motion_sample->quat, sizeof(p_motion_sample->quat));			
			memcpy(&m_buffer_tx.data.collect_all[m_buffer_tx.sample_num].gyro,p_motion_sample->gyro, sizeof(p_motion_sample->gyro));			
			memcpy(&m_buffer_tx.data.collect_all[m_buffer_tx.sample_num].accel,p_motion_sample->accel, sizeof(p_motion_sample->accel));			
			memcpy(&m_buffer_tx.data.collect_all[m_buffer_tx.sample_num].compass,p_motion_sample->compass, sizeof(p_motion_sample->compass));			     
            break;
		
		
    }
    m_buffer_tx.sample_num++;
    
    if (m_buffer_tx.sample_num >= cb.max_samples_per_block) {
        NRF_LOG_DEBUG("Buffer Full. Writing to block.");
		
        write_buffer_to_block();
        read_block_to_buffer();
        verify_write();
        inc_block_index();
        initialize_block();        
    }
      
    return SDS_SUCCESS;
}

sds_return_t sds_mem_request_samples(void) {
    NRF_LOG_DEBUG("Samples Requested.");
    
    static motion_sample_t motion_sample;
    sds_return_t err_code;
	
	if (cb.memory_state != SDS_MEMORY_MODE_STREAM) {
		return SDS_INVALID_STATE;
	}
    	
    while (cb.memory_state == SDS_MEMORY_MODE_STREAM) {
        err_code = get_next_sample(&motion_sample);
		if (err_code == SDS_INVALID_PREAMBLE) {
			break;
		}
        
        memset(&motion_sample, 0, sizeof(motion_sample));
    }  
    return SDS_SUCCESS;
}

sds_return_t sds_mem_start_stream(session_info_t * p_session_info) {

	sds_return_t err_code;
    NRF_LOG_DEBUG("Starting Memory Stream.");    
    
	if (cb.memory_state != SDS_MEMORY_MODE_IDLE) {
		return SDS_INVALID_STATE;
	}
       
    reset_block_index();

    err_code = read_session_info(p_session_info);
	if (err_code) {
		return err_code;
	}
	
    cb.motion_mode = p_session_info->motion_mode;
    cb.use_next_timestamp = true;
    cb.memory_state = SDS_MEMORY_MODE_STREAM;

	if (cb.motion_mode == QUATERNION_ONLY) {
		cb.data_size = QUATERNION_ONLY_DATA_SIZE;
	}
	else if (cb.motion_mode == COLLECT_ALL) {
		cb.data_size = COLLECT_ALL_DATA_SIZE;
	}
	else {
		return SDS_UNKNOWN_ERROR;
	}
	cb.data_size = QUATERNION_ONLY_DATA_SIZE;
 
    cb.event_callback(SDS_MEMORY_EVT_STREAM_STARTED, NULL);
	
    read_block_to_buffer();
    
    //check preamble   
    if (m_buffer_rx.block_preamble != MEM_BLOCK_PREAMBLE) {
        return SDS_MEMORY_NO_DATA;
    }      
    return SDS_SUCCESS;   
}

sds_return_t sds_mem_stop_stream(void) {

    NRF_LOG_DEBUG("Stopping Memory Stream.");        
    
	if (cb.memory_state != SDS_MEMORY_MODE_STREAM) {
		return SDS_INVALID_STATE;
	}
    
    cb.memory_state = SDS_MEMORY_MODE_IDLE;    
    
    return SDS_SUCCESS;
}

/**
    * @brief called when a read/write/erase is done
*/
static void qspi_handler(nrf_drv_qspi_evt_t event, void * p_context)
{
    UNUSED_PARAMETER(event);
    UNUSED_PARAMETER(p_context);
    cb.m_finished = true;
}

/**
    * @brief read session info from the session info block
*/
static sds_return_t read_session_info(session_info_t * p_session_info) {
    
    uint32_t err_code;
	session_info_block_buffer_t sib_buffer;
	
    err_code = nrf_drv_qspi_read(&sib_buffer, sizeof(session_info_block_buffer_t), SESSION_INFO_BLOCK_INDEX * QSPI_TEST_BLOCK_SIZE);
    APP_ERROR_CHECK(err_code);
    WAIT_FOR_PERIPH();

    NRF_LOG_HEXDUMP_DEBUG(&sib_buffer, sizeof(session_info_block_buffer_t));
	
	if (sib_buffer.preamble != SESSION_INFO_PREAMBLE) {
		return SDS_MEMORY_NO_DATA;
	}
	
	
	*p_session_info = sib_buffer.session_info;
    return SDS_SUCCESS;  
}

/**
    * @brief writes session info to the session info block
*/
static void write_session_info(session_info_t * p_session_info) {

    uint32_t err_code;
	
	const session_info_block_buffer_t sib_buffer = {
		.preamble = SESSION_INFO_PREAMBLE,
		.session_info = *p_session_info,
	};

	
    NRF_LOG_DEBUG("Writing Session Info to memory.");
    
	cb.m_finished = true;
    err_code = nrf_drv_qspi_erase(NRF_QSPI_ERASE_LEN_4KB, SESSION_INFO_BLOCK_INDEX * QSPI_TEST_BLOCK_SIZE);
    APP_ERROR_CHECK(err_code);  
	
	uint32_t ms_time = 0;
	uint32_t timeout_ms = 100;
	while (nrf_drv_qspi_mem_busy_check() == NRF_ERROR_BUSY)
	{
		if (ms_time++ > timeout_ms) {
			APP_ERROR_CHECK(NRF_ERROR_TIMEOUT);
		}
		nrf_delay_ms(1);
	};

    NRF_LOG_HEXDUMP_DEBUG(&sib_buffer, sizeof(session_info_block_buffer_t));
    
    ms_time = 0;
    err_code = nrf_drv_qspi_write(&sib_buffer, sizeof(session_info_block_buffer_t), SESSION_INFO_BLOCK_INDEX * QSPI_TEST_BLOCK_SIZE);
    APP_ERROR_CHECK(err_code);

	while (nrf_drv_qspi_mem_busy_check() == NRF_ERROR_BUSY)
	{
		if (ms_time++ > timeout_ms) {
			APP_ERROR_CHECK(NRF_ERROR_TIMEOUT);
		}
		nrf_delay_ms(1);
	};
    
}


static sds_return_t get_next_sample(motion_sample_t * p_motion_sample) {
    
    p_motion_sample->data_flags |= MEMORY_DATA;    
    
    if (cb.buffer_data_index >= m_buffer_rx.sample_num) {
        NRF_LOG_DEBUG("End of buffer.");
      
        //get data
        inc_block_index();
        read_block_to_buffer();

        //check preamble
		if (m_buffer_rx.block_preamble != MEM_BLOCK_PREAMBLE) {
			sds_mem_stop_stream();
			cb.event_callback(SDS_MEMORY_EVT_STREAM_TERMINATED, NULL);
			return SDS_INVALID_PREAMBLE;
		}  		
    }
	
    if (m_buffer_rx.block_timestamp) {
        //get timestamp
        p_motion_sample->timestamp = m_buffer_rx.block_timestamp;
        p_motion_sample->data_flags |= TIMESTAMP_DATA| MEMORY_DATA;
		cb.event_callback(SDS_MEMORY_EVT_MOTION_DATA, p_motion_sample);
		memset(p_motion_sample, 0, sizeof(motion_sample_t));
		
		m_buffer_rx.block_timestamp = NULL;
		
		NRF_LOG_INFO("Streaming Timestamp: %d ms.", p_motion_sample->timestamp);
    }
    
    switch (cb.motion_mode) {
        
        case QUATERNION_ONLY:
			
            memcpy(p_motion_sample->quat, &m_buffer_rx.data.quaternion_only[cb.buffer_data_index], sizeof(p_motion_sample->quat));
            p_motion_sample->data_flags |= QUATERNION_DATA | MEMORY_DATA;
			NRF_LOG_DEBUG("Streaming Quaternion Sample.");
			cb.event_callback(SDS_MEMORY_EVT_MOTION_DATA, p_motion_sample);
		    memset(p_motion_sample, 0, sizeof(motion_sample_t));
            break;
        
        case COLLECT_ALL:
                           
            memcpy(p_motion_sample->quat, &m_buffer_rx.data.collect_all[cb.buffer_data_index].quat, sizeof(p_motion_sample->quat));
            p_motion_sample->data_flags |= QUATERNION_DATA| MEMORY_DATA;      
			cb.event_callback(SDS_MEMORY_EVT_MOTION_DATA, p_motion_sample);
			memset(p_motion_sample, 0, sizeof(motion_sample_t));

            memcpy(p_motion_sample->gyro, &m_buffer_rx.data.collect_all[cb.buffer_data_index].gyro, sizeof(p_motion_sample->gyro));
            memcpy(p_motion_sample->accel, &m_buffer_rx.data.collect_all[cb.buffer_data_index].accel, sizeof(p_motion_sample->accel));
            p_motion_sample->data_flags |= IMU_DATA| MEMORY_DATA;         
			cb.event_callback(SDS_MEMORY_EVT_MOTION_DATA, p_motion_sample);
			memset(p_motion_sample, 0, sizeof(motion_sample_t));
		
            memcpy(p_motion_sample->compass, &m_buffer_rx.data.collect_all[cb.buffer_data_index].compass, sizeof(p_motion_sample->compass));
            p_motion_sample->data_flags |= COMPASS_DATA| MEMORY_DATA;  
			cb.event_callback(SDS_MEMORY_EVT_MOTION_DATA, p_motion_sample);
			memset(p_motion_sample, 0, sizeof(motion_sample_t));
		
            NRF_LOG_DEBUG("Streaming Quat/IMU/Compass Sample.");
			break;
		
		
    }
    cb.buffer_data_index++;
	return SDS_SUCCESS;
}

/**
    * @brief configures qspi and memory
*/
static void configure_memory() {
    
    uint8_t temporary = 0x40;
    uint32_t err_code;
    nrf_qspi_cinstr_conf_t cinstr_cfg = {
        .opcode    = QSPI_STD_CMD_RSTEN,
        .length    = NRF_QSPI_CINSTR_LEN_1B,
        .io2_level = true,
        .io3_level = true,
        .wipwait   = true,
        .wren      = true
    };

    // Send reset enable
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    // Send reset command
    cinstr_cfg.opcode = QSPI_STD_CMD_RST;
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, NULL, NULL);
    APP_ERROR_CHECK(err_code);

    // Switch to qspi mode
    cinstr_cfg.opcode = QSPI_STD_CMD_WRSR;
    cinstr_cfg.length = NRF_QSPI_CINSTR_LEN_2B;
    err_code = nrf_drv_qspi_cinstr_xfer(&cinstr_cfg, &temporary, NULL);
    APP_ERROR_CHECK(err_code);
        
}

/**
    * @brief resets block address pointer and erases block, sets up block to be timestamped
*/
static void initialize_block(void) {
    
    NRF_LOG_DEBUG("Initializing Block.");
    
    memset(&m_buffer_tx, 0, sizeof(m_buffer_tx));
    erase_block();
       
    m_buffer_tx.block_preamble = MEM_BLOCK_PREAMBLE;    
}

/**
    * @brief erases block at block index
*/
static void erase_block(void) {

    uint32_t err_code;    
    
    cb.m_finished = false;
	       
    err_code = nrf_drv_qspi_erase(NRF_QSPI_ERASE_LEN_4KB, cb.block_index * QSPI_TEST_BLOCK_SIZE);
    APP_ERROR_CHECK(err_code);
	
	WAIT_FOR_PERIPH();
	
    NRF_LOG_DEBUG("Block %d Erase Complete.", cb.block_index);
}



/**
    * @brief writes tx buffer to current block
*/
static uint32_t write_buffer_to_block(void) {

    uint32_t err_code;
    
    err_code = nrf_drv_qspi_write(&m_buffer_tx, sizeof(m_buffer_tx), cb.block_index * QSPI_TEST_BLOCK_SIZE);
    APP_ERROR_CHECK(err_code);
    WAIT_FOR_PERIPH();
    NRF_LOG_DEBUG("Block %d Write Complete.", cb.block_index);

    return err_code;
    
}

/**
    * @brief reads current block to rx buffer
*/
static uint32_t read_block_to_buffer(void) {

    uint32_t err_code;

    memset(&m_buffer_rx, 0, sizeof(m_buffer_rx));
    
    err_code = nrf_drv_qspi_read(&m_buffer_rx, sizeof(m_buffer_rx), cb.block_index * QSPI_TEST_BLOCK_SIZE);
    WAIT_FOR_PERIPH();
    NRF_LOG_DEBUG("Block %d Read Complete.", cb.block_index);

    return err_code;
    
}

/**
    * @brief checks that write buffer and read buffer match
*/ 
static uint32_t verify_write(void) {
    
    uint32_t err_code;

    NRF_LOG_DEBUG("Compare...");
    if (memcmp(&m_buffer_tx, &m_buffer_rx, sizeof(m_buffer_rx)) == 0)
    {
        NRF_LOG_DEBUG("Data consistent");
    }
    else
    {
        NRF_LOG_WARNING("Data inconsistent");
    }
    return err_code;
}

/**
    * @brief increments block index
*/ 
static void inc_block_index(void) {
    
    cb.block_index++;
    cb.buffer_data_index = 0;
    
    if (cb.block_index > MAX_BLOCK_INDEX) {
        
        if (cb.memory_state == SDS_MEMORY_MODE_SESSION) {
            
            cb.event_callback(SDS_MEMORY_EVT_FLASH_FULL,NULL);
            sds_mem_stop_session();
            
        }
        else if (cb.memory_state == SDS_MEMORY_MODE_STREAM) {

            sds_mem_stop_stream();
            
        }
        else {
            NRF_LOG_ERROR("Unexpected Result");
        }        
        
    }
    
}

/**
    * @brief moves block index back to minimum value
*/ 
static void reset_block_index(void) {
    cb.block_index = MIN_BLOCK_INDEX;
    cb.buffer_data_index = 0;    
}
