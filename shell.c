#include <stddef.h>
#include <stdint.h>
#include "shell.h"
#include "terminal.h"
#include "list.h"
#include "threads.h"
#include "keyboard.h"
#include "heap.h"
#include "libc/stdio.h"
#include "libc/string.h"

UNI_LIST_C(commands, command_entry_t)
UNI_LIST_C(history, char*)

//Static helpers
// static void report_fail(void);
// static uint32_t tokenize_command(char *command);
void command_help(const char* argv, uint32_t argc);

// static void add_text_to_history(char *text);
// static void shell_prev_command(void);
// static void shell_next_command(void);

struct list_commands_t *commands_list = NULL;
struct list_history_t *history_list = NULL;

void init_shell(void){

    commands_list = list_commands_create();
    history_list = list_history_create();

    // register_shell_command("help", "Display commands list", command_help);
}

void shell_main(void){

    printf("IferrOS user$ ");
    char *kb_buff = keyboard_get_buffer();
    printf("Buff: %s", kb_buff);
    
}

void command_help(const char* argv, uint32_t argc){

    printf("| Command           | Description\n");

    struct node_commands_t *current = commands_list->head;

    while(current!=NULL)
    {
        command_entry_t command_entry = current->data;

        uint32_t len = printf("| %s", command_entry.command);
        for(; len<20; len++) printf(" ");

        printf("| %s\n", command_entry.description);

        current = current->next;
    }
}