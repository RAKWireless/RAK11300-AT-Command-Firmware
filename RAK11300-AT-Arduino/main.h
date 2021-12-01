/**
 * @file main.h
 * @author Bernd Giesecke (bernd.giesecke@rakwireless.com)
 * @brief Includes and defines
 * @version 0.1
 * @date 2021-10-09
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include <Arduino.h>
#include <LoRaWan-Arduino.h>
#include <mbed.h>
#include <rtos.h>
#include <multicore.h>
#include <time.h>

using namespace rtos;
using namespace mbed;
using namespace std::chrono_literals;
using namespace std::chrono;

// Debug
// Debug output set to 0 to disable app debug output
#ifndef APP_DEBUG
#define APP_DEBUG 0
#endif

#if APP_DEBUG > 0
#define APP_LOG(tag, ...)                \
	do                                   \
	{                                    \
		if (tag)                         \
			Serial.printf("[%s] ", tag); \
		Serial.printf(__VA_ARGS__);      \
		Serial.printf("\n");             \
	} while (0)
#else
#define APP_LOG(...)
#endif

#define DualSerial(...)              \
	do                               \
	{                                \
		Serial.printf(__VA_ARGS__);  \
		Serial1.printf(__VA_ARGS__); \
	} while (0)

// Firmware
void trigger_sending(void);
extern TimerEvent_t app_timer;
extern uint16_t g_sw_ver_1; // major version increase on API change / not backwards compatible
extern uint16_t g_sw_ver_2; // minor version increase on API change / backward compatible
extern uint16_t g_sw_ver_3; // patch version increase on bugfix, no affect on API
extern osThreadId loop_thread;

//***************************************************
// Signals to wake up the loop() binary coded!!!!
// Use only one bit per signal!!!!
//***************************************************
/** Unconfirmed TX finished */
#define SIGNAL_UNCONF_TX 0x0001
/** Confirmed TX finished, ACK received */
#define SIGNAL_CONF_TX_ACK 0x0002
/** Confirmed TX failed, no ACK received */
#define SIGNAL_CONF_TX_NAK 0x0004
/** Periodic sending triggered */
#define SIGNAL_SEND 0x0008
/** Join success */
#define SIGNAL_JOIN_SUCCESS 0x0010
/** Join success */
#define SIGNAL_JOIN_FAIL 0x0020
/** LoRaWAN packet received */
#define SIGNAL_RX 0x0040
/** Start Join */
#define SIGNAL_JOIN 0x0080

// LoRaWAN
int8_t init_lora(void);
int8_t init_lorawan(void);
bool send_p2p_packet(uint8_t *data, uint8_t size);
lmh_error_status send_lora_packet(uint8_t *data, uint8_t size, uint8_t fport = 0);
extern bool g_lpwan_has_joined;
extern bool g_rx_fin_result;
extern bool g_join_result;
extern uint32_t otaaDevAddr;

#define LORAWAN_DATA_MARKER 0x55
struct s_lorawan_settings
{
	uint8_t valid_mark_1 = 0xAA;				// Just a marker for the Flash
	uint8_t valid_mark_2 = LORAWAN_DATA_MARKER; // Just a marker for the Flash
												// OTAA Device EUI MSB
	uint8_t node_device_eui[8] = {0x00, 0x0D, 0x75, 0xE6, 0x56, 0x4D, 0xC1, 0xF3};
	// OTAA Application EUI MSB
	uint8_t node_app_eui[8] = {0x70, 0xB3, 0xD5, 0x7E, 0xD0, 0x02, 0x01, 0xE1};
	// OTAA Application Key MSB
	uint8_t node_app_key[16] = {0x2B, 0x84, 0xE0, 0xB0, 0x9B, 0x68, 0xE5, 0xCB, 0x42, 0x17, 0x6F, 0xE7, 0x53, 0xDC, 0xEE, 0x79};
	// ABP Device Address MSB
	uint32_t node_dev_addr = 0x26021FB4;
	// ABP Network Session Key MSB
	uint8_t node_nws_key[16] = {0x32, 0x3D, 0x15, 0x5A, 0x00, 0x0D, 0xF3, 0x35, 0x30, 0x7A, 0x16, 0xDA, 0x0C, 0x9D, 0xF5, 0x3F};
	// ABP Application Session key MSB
	uint8_t node_apps_key[16] = {0x3F, 0x6A, 0x66, 0x45, 0x9D, 0x5E, 0xDC, 0xA6, 0x3C, 0xBC, 0x46, 0x19, 0xCD, 0x61, 0xA1, 0x1E};
	// Flag for OTAA or ABP
	bool otaa_enabled = true;
	// Flag for ADR on or off
	bool adr_enabled = false;
	// Flag for public or private network
	bool public_network = true;
	// Flag to enable duty cycle
	bool duty_cycle_enabled = false;
	// In milliseconds: 2 * 60 * 1000 => 2 minutes
	uint32_t send_repeat_time = 120000;
	// Number of join retries
	uint8_t join_trials = 5;
	// TX power 0 .. 10
	uint8_t tx_power = 0;
	// Data rate 0 .. 15 (validity depnends on Region)
	uint8_t data_rate = 3;
	// LoRaWAN class 0: A, 2: C, 1: B is not supported
	uint8_t lora_class = 0;
	// Subband channel selection 1 .. 9
	uint8_t subband_channels = 1;
	// Flag if node joins automatically after reboot
	bool auto_join = false;
	// Data port to send data
	uint8_t app_port = 2;
	// Flag to enable confirmed messages
	lmh_confirm confirmed_msg_enabled = LMH_UNCONFIRMED_MSG;
	// Fixed LoRaWAN lorawan_region (depends on compiler option)
	uint8_t lora_region = 1;
	// Flag for LoRaWAN or LoRa P2P
	bool lorawan_enable = true;
	// Frequency in Hz
	uint32_t p2p_frequency = 916000000;
	// Tx power 0 .. 22
	uint8_t p2p_tx_power = 22;
	// Bandwidth 0: 125 kHz, 1: 250 kHz, 2: 500 kHz, 3: Reserved
	uint8_t p2p_bandwidth = 0;
	// Spreading Factor SF7..SF12
	uint8_t p2p_sf = 7;
	// Coding Rate 1: 4/5, 2: 4/6, 3: 4/7, 4: 4/8
	uint8_t p2p_cr = 1;
	// Preamble length
	uint8_t p2p_preamble_len = 8;
	// Symbol timeout
	uint16_t p2p_symbol_timeout = 0;
	// Command from BLE to reset device
	bool resetRequest = true;
};
extern s_lorawan_settings g_lorawan_settings;
extern bool g_lorawan_initialized;
extern int16_t g_last_rssi;
extern int8_t g_last_snr;
extern uint8_t g_last_fport;
extern uint8_t m_lora_app_data_buffer[];
extern bool g_lpwan_has_joined;
extern uint8_t g_rx_data_len;
extern uint8_t g_rx_lora_data[];
extern uint8_t g_tx_lora_data[];
extern uint8_t g_tx_data_len;
enum P2P_RX_MODE
{
	RX_MODE_NONE = 0,
	RX_MODE_RX = 1,
	RX_MODE_RX_TIMED = 2,
	RX_MODE_RX_WAIT = 3
};
extern uint8_t g_lora_p2p_rx_mode;
extern uint32_t g_lora_p2p_rx_time;

// AT command parser
void at_serial_input(uint8_t cmd);
bool init_serial_task(void);
extern char *region_names[];
bool user_at_handler(char *user_cmd, uint8_t cmd_size) __attribute__((weak));
void at_settings(void);

// Battery
void init_batt(void);
float read_batt(void);
uint8_t get_lora_batt(void);
uint8_t mv_to_percent(float mvolts);

// Fake Flash
void init_flash(void);
bool save_settings(void);
void log_settings(void);
void flash_reset(void);
