#include <stddef.h>
#include <stdint.h>
#include "libc/stdio.h"
#include "sys.h"
#include "ps2.h"
#include "ports.h"

static int ps2_check_controller_exist(void);
static int ps2_check_controller_work(void);
extern void sys_cli();
extern void sys_sti();

void init_ps2(void){

    printf("Initialize PS2\n");

    if(!ps2_check_controller_work()) kernel_panic("Problem With PS2 Controller");
    else printf("PS2 Controller Ready\n");
}

static int ps2_check_controller_work(void){

    sys_cli();

    ps2_write_command(COMMAND_DISABLE_FIRST_PORT);
    ps2_write_command(COMMAND_DISABLE_SECOND_PORT);

    ps2_write_command(COMMAND_TEST_CONTROLLER);
    uint8_t test_result = ps2_read_data();

    ps2_write_command(COMMAND_ENABLE_FIRST_PORT);
    ps2_write_command(COMMAND_ENABLE_SECOND_PORT);

    sys_sti();

    if(test_result == PS2_TEST_PASSED) return 1;
    else return 0;
}

void ps2_write_command(uint8_t command){

	uint8_t status;
	do {
		status = inb(PS2_COMMAND_PORT);
	} while(status & STATUS_OUTPUT_BUFFER_STATUS);

	return outb(PS2_COMMAND_PORT, command);
}

uint8_t ps2_read_data(void){

	uint8_t status;
	do {
		status = inb(PS2_COMMAND_PORT);
	} while(!(status & STATUS_INPUT_BUFFER_STATUS));

	return inb(PS2_DATA_PORT);
}

void ps2_write_data(uint8_t data){
	uint8_t status;

	do {
		status = inb(PS2_COMMAND_PORT);
	} while(status & STATUS_OUTPUT_BUFFER_STATUS);

	return outb(PS2_DATA_PORT, data);
}

uint8_t ps2_get_config_byte(void){

    ps2_write_command(COMMAND_READ_CONFIG_BYTE);
    uint8_t byte = ps2_read_data();
    return byte;
}


void ps2_set_config_byte(uint8_t byte){

    ps2_write_command(COMMAND_SAVE_CONFIG_BYTE);
    ps2_write_data(byte);
}