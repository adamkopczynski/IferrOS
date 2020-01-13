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

    char command[MAX_COMMAND_LENGTH];
    
    while(1){
        printf("IferrOS user$ ");
        gets(command, MAX_COMMAND_LENGTH+1);

        if(!run_program(command)) printf("\n");
        else printf("%s: command not found\n");
    }
    
}

void command_help(const char* argv, uint32_t argc){

    printf("| Command           | Description\n");

    struct node_commands_t *current = commands_list->head;

    while(current!=NULL){

        command_entry_t command_entry = current->data;

        uint32_t len = printf("| %s", command_entry.command);
        for(; len<20; len++) printf(" ");

        printf("| %s\n", command_entry.description);

        current = current->next;
    }
}

void register_shell_command(char *command, char *description, command_function_t fun){

    command_entry_t command_entry;

    command_entry.command = command;
    command_entry.description = description;
    command_entry.fun = fun;

    list_commands_push_back(commands_list, command_entry);
}

int run_program(char *command){

    struct node_commands_t *current = commands_list->head;

    while(current!=NULL){

        command_entry_t command_entry = current->data;

        if(strcmp(command_entry.command, command) == 0){

            command_function_t fun = command_entry.fun;
            fun(command, 0);

            return 1;
        }
        
        current = current->next;
    }

    return 0;
}