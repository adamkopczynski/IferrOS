#ifndef __SHELL_H__
#define __SHELL_H__

#include <stdint.h>
#include "terminal.h"

#define MAX_COMMAND_LENGTH 250

typedef void (*command_function_t)(const char* argv, uint32_t argc);

typedef struct command_entry{
    const char *command;
    const char *description;
    command_function_t fun;
} command_entry_t;

void init_shell(void);
void shell_main(void);
int run_program(char *command);
void register_shell_command(char *command, char *description, command_function_t fun);

void shell_start_insert(void);
void shell_end_insert(void);

#endif