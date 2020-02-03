#ifndef __PS2_h__
#define __PS2_h__

#include <stdint.h>

// Adresy portów
#define PS2_COMMAND_PORT 0x64
#define PS2_DATA_PORT 0x60

#define STATUS_INPUT_BUFFER_STATUS 0x01
#define STATUS_OUTPUT_BUFFER_STATUS 0x02

#define CONFIG_FIRST_PORT_INTERRUPT 0x01
#define CONFIG_SECOND_PORT_INTERRUPT 0x02

#define COMMAND_READ_CONFIG_BYTE 0x20
#define COMMAND_SAVE_CONFIG_BYTE 0x60
#define COMMAND_ENABLE_FIRST_PORT 0xAE 
#define COMMAND_ENABLE_SECOND_PORT 0xA8 
#define COMMAND_DISABLE_FIRST_PORT 0xAD 
#define COMMAND_DISABLE_SECOND_PORT 0xA8
#define COMMAND_TEST_CONTROLLER 0xAA 
#define COMMAND_TEST_FIRST_PORT 0xAB
#define COMMAND_TEST_SECOND_PORT 0xA9 

#define COMMAND_KB_SET_LED 0xED 
#define COMMAND_KB_SET_TYPEMATIC 0xF3 

#define PS2_TEST_PASSED 0x55
#define DEVICE_TEST_PASSED 0x00

void init_ps2(void);
void ps2_write_command(uint8_t command);
void ps2_write_mouse_command(uint8_t command);
void ps2_write_data(uint8_t data);
uint8_t ps2_read_data(void);
uint8_t ps2_get_config_byte(void);
void ps2_set_config_byte(uint8_t byte);

#endif